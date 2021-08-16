//transport_catalogue.h, transport_catalogue.cpp — класс транспортного справочника#include <vector>

#pragma once

#include <vector>
#include <string_view>
#include <set>
#include <algorithm>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <optional>

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
        : stop_index_({}), bus_index_({}), stop_to_bus_({}) {

    }

    // добавление маршрута в базу
    void AddBus(Bus& bus);

    // добавление остановки в базу
    void AddStop(Stop& stop, std::unordered_map<std::string, int> distances);

    // поиск маршрута по имени
    Bus* SearchBus(std::string_view& name);

    // поиск остановки по имени
    Stop* SearchStop(std::string_view& name);

    //эти 3 метода тут потому что это единственный общий класс для чтения и вывода
    std::vector<std::string_view> Split(std::string_view input, char c);
    std::vector<std::string_view> Split(std::string_view input, std::string& c);
    std::string_view Trim(std::string_view in);

    // получение всех маршрутов у остановки
    void GetBusesByStop(const Stop* stop, std::set<std::string>& out);

    // получение фактического расстояния между двумя остановками
    std::optional<int> GetStopToStopDistance(const Stop* stopStart, const Stop* stopEnd);

private:
    std::vector<Stop> stop_index_;
    std::vector<Bus> bus_index_;
    //std::unordered_map<const Stop*, std::unordered_set<Bus*>> stop_to_bus_; //не работается нормально с такой структурой. где-то в память проваливаюсь не туда. скорее всего на it->second.insert(&*bus_shure);
    std::unordered_map<std::string, std::set<std::string>> stop_to_bus_; //почему строки? коммент выше
    std::unordered_map<std::string, std::unordered_map<std::string, int>> stop_to_stop_distance_;
};
