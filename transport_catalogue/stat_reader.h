//stat_reader.h, stat_reader.cpp — чтение запросов на вывод и сам вывод

#pragma once

#include <string>
#include <string_view>
#include <iostream>
#include <algorithm>
#include <unordered_set>

#include "transport_catalogue.h"
#include "geo.h"

using std::string_literals::operator""s;

class StatReader {
public:
    StatReader() = delete;
    StatReader(TransportCatalogue& tc) 
        : tc_(tc) {

    }

    // получение информации о маршруте
    void GetInfoBus(std::ostream& out, const std::string& input);
    void ProcessOutputRequests(std::ostream& out, const std::vector<std::string>& requests);

    // Запрос должен вывести информацию об остановке X
    void GetInfoStop(std::ostream& out, const std::string& input);

private:
    TransportCatalogue& tc_;
    std::string_view parseSearchBus(const std::string& input);
};
