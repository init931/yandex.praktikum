#include "process_queries.h"

std::vector<std::vector<Document>> ProcessQueries(
        const SearchServer& search_server,
        const std::vector<std::string>& queries) {

    std::vector<std::vector<Document>> ret(queries.size());
    ret.reserve(queries.size());

    std::transform(
        std::execution::par,
        queries.begin(),
        queries.end(),
        ret.begin(),
        [&search_server] (const std::string& query) -> std::vector<Document> {
            return search_server.FindTopDocuments(query);
        }
    );

    return ret;
}
