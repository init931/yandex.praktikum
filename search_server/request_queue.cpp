#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server) :
    search_server_(search_server) {
    
    empty_query_count_ = 0;
}

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, 
                                                   DocumentPredicate document_predicate) {

    auto ret = search_server_.FindTopDocuments(raw_query, document_predicate);
    QueryResult result;
    result.result_count = ret.size();
    AddQueue(ret);
    return ret;
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    auto ret = search_server_.FindTopDocuments(raw_query, status);
    QueryResult result;
    result.result_count = ret.size();
    AddQueue(result);
    return ret;
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    auto ret = search_server_.FindTopDocuments(raw_query);
    QueryResult result;
    result.result_count = ret.size();
    AddQueue(result);
    return ret;
}

int RequestQueue::GetNoResultRequests() const {
    return empty_query_count_;
}

void RequestQueue::AddQueue(const QueryResult& request) {
    if (requests_.size() == sec_in_day_) {
        if (requests_[0].result_count == 0) {
            --empty_query_count_;
        }
        requests_.pop_front();
    }

    requests_.push_back(request);
    if (request.result_count == 0) {
        ++empty_query_count_;
    }
}
