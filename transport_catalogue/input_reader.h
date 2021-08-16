//input_reader.h, input_reader.cpp — чтение запросов на заполнение базы

#pragma once

#include <string>
#include <vector>
#include <string_view>

#include "transport_catalogue.h"

class InputReader {
public:
    InputReader() = delete;
    InputReader(TransportCatalogue& tc) 
        : tc_(tc) {

    }

    //Stop X: latitude, longitude
    void AddStop(const std::string input);

    //Bus X: описание маршрута
    void AddBus(const std::string input);

    void ProcessInputRequests(const std::vector<std::string>& requests);

private:
    TransportCatalogue& tc_;

    Bus parseBus(const std::string input);
};
