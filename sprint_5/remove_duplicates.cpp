#include "remove_duplicates.h"
//#include "search_server.cpp" //???

using std::string_literals::operator""s;

void RemoveDuplicates(SearchServer& search_server) {
    std::set<std::set<std::string>> docs;
    std::set<int> buffer_doc_ids;
    for (const int document_id : search_server) {
        std::set<std::string> words;
        for (const auto& [word, freq] : search_server.GetWordFrequencies(document_id)) {
            words.insert(word);
        }

        auto insert_ret = docs.emplace(words);
        if (!insert_ret.second) {
            buffer_doc_ids.insert(document_id);
        }
    }
    
    for (auto doc_id : buffer_doc_ids) {
        search_server.RemoveDocument(doc_id);
        std::cout << "Del duplicate id = "s << doc_id << std::endl;
    }
}
