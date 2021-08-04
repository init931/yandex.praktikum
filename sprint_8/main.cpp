#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <deque>
#include <execution>

#include "read_input_functions.h"
#include "test_example_functions.h"
#include "paginator.h"
#include "document.h"
#include "search_server.h"
#include "request_queue.h"
#include "remove_duplicates.h"
#include "log_duration.h"
#include "process_queries.h"

//
#include "read_input_functions.cpp"
#include "test_example_functions.cpp"
#include "document.cpp"
#include "search_server.cpp"
#include "request_queue.cpp"
#include "remove_duplicates.cpp"
#include "process_queries.cpp"
//

using std::string_literals::operator""s;

int main() {
    SearchServer search_server("and with"s);

    int id = 0;
    for (
        const std::string& text : {
            "white cat and yellow hat"s,
            "curly cat curly tail"s,
            "nasty dog with big eyes"s,
            "nasty pigeon john"s,
        }
    ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, {1, 2});
    }

    std::cout << "ACTUAL by default:"s << std::endl;
        // последовательная версия
    for (const Document& document : search_server.FindTopDocuments("curly nasty cat"s)) {
        PrintDocument(document);
    }
    std::cout << "BANNED:"s << std::endl;
        // последовательная версия
    for (const Document& document : search_server.FindTopDocuments(std::execution::seq, "curly nasty cat"s, DocumentStatus::BANNED)) {
        PrintDocument(document);
    }

    std::cout << "Even ids:"s << std::endl;
        // параллельная версия
    for (const Document& document : search_server.FindTopDocuments(std::execution::par, "curly nasty cat"s, [](int document_id, DocumentStatus status, int rating) { return document_id % 2 == 0; })) {
        PrintDocument(document);
    }

    return 0;
}
