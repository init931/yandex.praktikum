#include <algorithm>
#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include <deque>

#include "read_input_functions.h"
#include "test_example_functions.h"
#include "paginator.h"
#include "document.h"
#include "search_server.h"
#include "request_queue.h"
#include "remove_duplicates.h"
#include "log_duration.h"
#include "process_queries.h"

//почему без этих вставок локальный линковщик не может собрать файл?
//хотя в тренажере запускается без этих include
//(еще один такой include есть в search_server.cpp)
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
            "funny pet and nasty rat"s,
            "funny pet with curly hair"s,
            "funny pet and not very nasty rat"s,
            "pet with rat and rat and rat"s,
            "nasty rat with curly hair"s,
        }
    ) {
        search_server.AddDocument(++id, text, DocumentStatus::ACTUAL, {1, 2});
    }

    const std::string query = "curly and funny"s;

    auto report = [&search_server, &query] {
        std::cout << search_server.GetDocumentCount() << " documents total, "s
            << search_server.FindTopDocuments(query).size() << " documents for query ["s << query << "]"s << std::endl;
    };

    report();
    // однопоточная версия
    search_server.RemoveDocument(5);
    report();
    // однопоточная версия
    search_server.RemoveDocument(std::execution::seq, 1);
    report();
    // многопоточная версия
    search_server.RemoveDocument(std::execution::par, 2);
    report();

    return 0;
}
