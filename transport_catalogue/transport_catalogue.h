//transport_catalogue.h, transport_catalogue.cpp — класс транспортного справочника#include <vector>

#pragma once

#include <vector>
#include <string_view>
#include <set>
#include <algorithm>
#include <string>

//Остановка
struct Stop {
    Stop() = delete;
    Stop(std::string name, double latitude, double longitude)
        : Name(move(name)), Latitude(latitude), Longitude(longitude) {

    }

    std::string Name;
    double Latitude;
    double Longitude;
};

//Маршрут
struct Bus {
    Bus() = delete;
    Bus(std::string name, bool is_ring) 
        : Name(move(name)), Is_ring(is_ring), Stops({}) {

    }

    std::string Name;
    bool Is_ring;
    std::vector<const Stop*> Stops;
};

class TransportCatalogue {
public:
    TransportCatalogue()
        : stop_index_({}), bus_index_({}) {

    }

    // добавление маршрута в базу
    void AddBus(Bus& bus);

    // добавление остановки в базу
    void AddStop(Stop& stop);

    // поиск маршрута по имени
    Bus* SearchBus(std::string_view& name);

    // поиск остановки по имени
    Stop* SearchStop(std::string_view& name);

    //эти 2 метода тут потому что это единственный общий класс для чтения и вывода
    std::vector<std::string_view> Split(std::string_view input, char c);
    std::string_view Trim(std::string_view in);

private:
    std::vector<Stop> stop_index_;
    std::vector<Bus> bus_index_;
};
