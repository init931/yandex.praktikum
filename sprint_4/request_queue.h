#pragma once
#include "search_server.h"

#include <deque>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);

    // сделаем "обёртки" для всех методов поиска, чтобы сохранять результаты для нашей статистики
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
        // напишите реализацию
        auto ret = search_server_.FindTopDocuments(raw_query, document_predicate);
        QueryResult result;
        result.result_count = ret.size();
        AddQueue(ret);
        return ret;
    }

    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);

    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;
private:
	const SearchServer& search_server_;
    struct QueryResult {
		int result_count;
	};
	std::deque<QueryResult> requests_;
	const static int sec_in_day_ = 1440;
	int empty_query_count_;

	void AddQueue(const QueryResult& request);
};
