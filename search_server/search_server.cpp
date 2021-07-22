#include "search_server.h"
#include "string_processing.cpp" //???

using std::string_literals::operator""s;

SearchServer::SearchServer(const std::string& stop_words_text)
    : SearchServer(SplitIntoWords(stop_words_text))
{

}

SearchServer::SearchServer(const std::string_view& stop_words_text)
    : SearchServer(SplitIntoWords(stop_words_text))
{
    
}

void SearchServer::AddDocument(int document_id, const std::string_view& document, 
    DocumentStatus status, const std::vector<int>& ratings) {

    if ((document_id < 0) || (documents_.count(document_id) > 0)) {
        throw std::invalid_argument("Invalid document_id"s);
    }
    const auto words = SplitIntoWordsNoStop(document);

    const double inv_word_count = 1.0 / words.size();
    for (const std::string& word : words) {
        word_to_document_freqs_[word][document_id] += inv_word_count;
        document_to_word_freqs_[document_id][word] += inv_word_count;
    }
    documents_.emplace(document_id, DocumentData{ComputeAverageRating(ratings), status});
    document_ids_.insert(document_id);
}

std::vector<Document> SearchServer::FindTopDocuments(
    const std::string_view& raw_query, DocumentStatus status) const {

    return FindTopDocuments(raw_query, [status](int document_id, DocumentStatus document_status, int rating) {
        return document_status == status;
    });
}

std::vector<Document> SearchServer::FindTopDocuments(const std::string_view& raw_query) const {
    return FindTopDocuments(raw_query, DocumentStatus::ACTUAL);
}

int SearchServer::GetDocumentCount() const {
    return documents_.size();
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(
    const std::string_view& raw_query, int document_id) const {

    return MatchDocument(std::execution::seq, raw_query, document_id);
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(
    const std::execution::parallel_policy& p, const std::string_view& raw_query, int document_id) const {

    return MatchDocument(std::execution::seq, raw_query, document_id);
}

std::tuple<std::vector<std::string_view>, DocumentStatus> SearchServer::MatchDocument(
    const std::execution::sequenced_policy& p, const std::string_view& raw_query, int document_id) const {

    Query query;
    std::vector<std::string_view> matched_words;

    bool parseRes = ParseQuery(raw_query, query);
    if (!parseRes) {
        throw std::invalid_argument("invalid_argument");
    }

    for (const std::string& word : query.plus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.push_back(std::string_view(
                document_to_word_freqs_.at(document_id).find(word)->first)
            );
        }
    }
    for (const std::string& word : query.minus_words) {
        if (word_to_document_freqs_.count(word) == 0) {
            continue;
        }
        if (word_to_document_freqs_.at(word).count(document_id)) {
            matched_words.clear();
            break;
        }
    }
    return {matched_words, documents_.at(document_id).status};
}

std::set<int>::const_iterator SearchServer::begin() const {
    return document_ids_.cbegin();
}
std::set<int>::const_iterator SearchServer::end() const {
    return document_ids_.cend();
}

static const std::map<std::string_view, double> STATIC_EMPTY_MAP; //reference to stack memory associated with local
const std::map<std::string_view, double>& SearchServer::GetWordFrequencies(int document_id) const {
    if (document_to_word_freqs_.count(document_id)) {
        auto it = document_to_word_freqs_.find(document_id);
        std::map<std::string_view, double>* ret = new std::map<std::string_view, double>;
        ret->insert(it->second.begin(), it->second.end());
        return *ret;
    } else {
        //std::map<std::string, double> empty_map;
        //return empty_map;
        //return {};
        return STATIC_EMPTY_MAP;
    }
}

void SearchServer::RemoveDocument(int document_id) {
    RemoveDocument(std::execution::seq, document_id);
}

void SearchServer::RemoveDocument(const std::execution::parallel_policy& p, int document_id) {
    RemoveDocument(std::execution::seq, document_id);
}

void SearchServer::RemoveDocument(const std::execution::sequenced_policy& p, int document_id) {
    if (document_to_word_freqs_.count(document_id) == 0) {
        return;
    }
    documents_.erase(document_id);
    document_ids_.erase(document_id);

    for (const auto& [word, freq] : document_to_word_freqs_.at(document_id)) {
        std::map<int, double>& word_doc_id_map = word_to_document_freqs_[word];
        if (word_doc_id_map.size() > 1) {
            word_doc_id_map.erase(document_id);
        } else {
            word_to_document_freqs_.erase(word);
        }
    }
    document_to_word_freqs_.erase(document_id);
}

bool SearchServer::IsStopWord(const std::string& word) const {
    return stop_words_.count(word) > 0;
}

bool SearchServer::IsValidWord(const std::string& word) {
    // A valid word must not contain special characters
    return none_of(word.begin(), word.end(), [](char c) {
        return c >= '\0' && c < ' ';
    });
}

std::vector<std::string> SearchServer::SplitIntoWordsNoStop(const std::string_view& text) const {
    std::vector<std::string> words;
    for (const std::string& word : SplitIntoWords(text)) {
        if (!IsValidWord(word)) {
            throw std::invalid_argument("Word "s + word + " is invalid"s);
        }
        if (!IsStopWord(word)) {
            words.push_back(word);
        }
    }
    return words;
}

int SearchServer::ComputeAverageRating(const std::vector<int>& ratings) {
    if (ratings.empty()) {
        return 0;
    }
    int rating_sum = 0;
    for (const int rating : ratings) {
        rating_sum += rating;
    }
    return rating_sum / static_cast<int>(ratings.size());
}

bool SearchServer::ParseQueryWord(const std::string& text, SearchServer::QueryWord& ret) const {
    if (text.empty()) {
        return false;
    }
    std::string word = text;
    bool is_minus = false;
    if (word[0] == '-') {
        is_minus = true;
        word = word.substr(1);
    }
    if (word.empty() || word[0] == '-' || !IsValidWord(word)) {
        return false;
    }

    ret = SearchServer::QueryWord{word, is_minus, IsStopWord(word)};
    return true;
}

bool SearchServer::ParseQuery(const std::string_view& text, SearchServer::Query& ret) const {
    ret = {};
    for (const std::string& word : SplitIntoWords(text)) {
        QueryWord query_word;
        bool parseRes = ParseQueryWord(word, query_word);
        if (!parseRes) {
            return false;
        }
        if (!query_word.is_stop) {
            if (query_word.is_minus) {
                ret.minus_words.insert(query_word.data);
            } else {
                ret.plus_words.insert(query_word.data);
            }
        }
    }
    return true;
}

double SearchServer::ComputeWordInverseDocumentFreq(const std::string& word) const {
    return log(GetDocumentCount() * 1.0 / word_to_document_freqs_.at(word).size());
}
