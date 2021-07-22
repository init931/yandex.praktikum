#include "remove_duplicates.h"
//#include "search_server.cpp" //???

using std::string_literals::operator""s;

void RemoveDuplicates(SearchServer& search_server) {
    std::set<std::set<std::string_view>> docs;
    std::set<int> buffer_doc_ids;
    for (const int document_id : search_server) {
        std::set<std::string_view> words;
        const std::map<std::string_view, double> word_freq_map = search_server.GetWordFrequencies(document_id);
        std::transform(
            word_freq_map.begin(),
            word_freq_map.end(),
            std::inserter(words, words.end()),
            [] (const std::pair<std::string_view, double>& word_freq_map) -> std::string_view {
                return word_freq_map.first;
            }
        );

        if (!std::count(docs.begin(), docs.end(), words)) {
            docs.insert(words);
            buffer_doc_ids.insert(document_id);
        }
    }
    
    for (auto doc_id : buffer_doc_ids) {
        search_server.RemoveDocument(doc_id);
        std::cout << "Del duplicate id = "s << doc_id << std::endl;
    }
}
