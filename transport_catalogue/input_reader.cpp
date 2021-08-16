//input_reader.h, input_reader.cpp — чтение запросов на заполнение базы

#include "input_reader.h"

#include <charconv>

using std::string_literals::operator""s;

void InputReader::AddStop(const std::string input) {
    //Остановка с названием X и координатами latitude и longitude
    
    std::vector<std::string_view> spt = tc_.Split(input, ':');
    spt[0].remove_prefix(5);
    std::string name { spt[0] };
    std::vector<std::string_view> spt_coords = tc_.Split(spt[1], ',');

    std::string lat_str { spt_coords[0] };
    double lat = std::stod(lat_str);
    std::string lon_str { spt_coords[1] };
    double lon = std::stod(lon_str);

    Stop stop(name, lat, lon);

    std::unordered_map<std::string, int> distances;
    if (spt_coords.size() > 2) {
        //растояние до других остановок
        //Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka, 100m to Marushkino

        std::string spt_div = "m to "s;
        for (size_t i = 2; i < spt_coords.size(); ++i) {
            std::string_view item = spt_coords[i];
            std::vector<std::string_view> spt_dis = tc_.Split(item, spt_div);
            std::string distance_to_stop_str { spt_dis[0] };

            if (distance_to_stop_str.find_first_not_of(" 0123456789"s) != std::string::npos) {
                throw std::logic_error("Возможно в расстоянии km а не m. Или что-то другое. Строка = "s + distance_to_stop_str);
            }

            int distance_to_stop = std::stoi(distance_to_stop_str);
            std::string stop_name { spt_dis[1] };
            distances[stop_name] = distance_to_stop;
        }
    }

    tc_.AddStop(stop, distances);
}

void InputReader::AddBus(const std::string input) {
    Bus bus = parseBus(input);
    tc_.AddBus(bus);
}

Bus InputReader::parseBus(const std::string input) {
    // Запрос на добавление автобуса номер X. Описание маршрута может задаваться в одном из двух форматов (см. пример):
    // stop1 - stop2 - ... stopN: автобус следует от stop1 до stopN и обратно с указанными промежуточными остановками.
    // stop1 > stop2 > ... > stopN > stop1: кольцевой маршрут с конечной stop1.

    std::vector<std::string_view> spt = tc_.Split(input, ':');
    spt[0].remove_prefix(4);
    std::string name{ spt[0] };

    bool is_ring = false;
    char div = '-';
    if (spt[1].find('>') != std::string_view::npos) {
        is_ring = true;
        div = '>';
    }

    spt = tc_.Split(spt[1], div);
    std::vector<Stop*> stops;
    stops.reserve(spt.size());
    for(std::string_view& item : spt) {
        std::string_view stop_name = tc_.Trim(item);
        Stop* stop = tc_.SearchStop(stop_name);
        stops.push_back(stop);
    }

    Bus ret(name, is_ring);
    if (!is_ring) {
        std::vector<Stop*>::iterator i = stops.end() - 1;
        std::vector<Stop*> tmp;
        while (i != stops.begin()) {
            --i;
            tmp.push_back(*i);
        }
        stops.insert(stops.end(), tmp.begin(), tmp.end());
    }

    ret.Stops.reserve(stops.size());
    for(const Stop* item : stops) {
        ret.Stops.push_back(item);
    }
    return ret;
}

void InputReader::ProcessInputRequests(const std::vector<std::string>& requests) {
    for (const std::string& item : requests) {
        if (item.rfind("Stop "s, 0) == 0) {
            AddStop(item);
        }
    }
    for (const std::string& item : requests) {
        if (item.rfind("Bus "s, 0) == 0) {
            AddBus(item);
        }
    }
}
