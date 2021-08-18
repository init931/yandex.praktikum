//transport_catalogue.h, transport_catalogue.cpp — класс транспортного справочника

#include "transport_catalogue.h"

#include <algorithm>

namespace Transport::Core {
    void TransportCatalogue::AddBus(Bus& bus) {
        std::string name = bus.Name;
        bus_index_.push_back(bus); //??? copy?

        auto bus_shure = std::find_if(bus_index_.begin(), bus_index_.end(), [&name](Bus& item) {
            return item.Name == name;
        });

        for (const Stop* item : bus_shure->Stops) {
            auto it = stop_to_bus_.find(item->Name);
            if (it == stop_to_bus_.end()) {
                stop_to_bus_.insert({ item->Name, {} });
                it = stop_to_bus_.find(item->Name);
            }

            if (it->second.find(bus_shure->Name) == it->second.end()) {
                it->second.insert(bus_shure->Name);
            }
        }
    }

    void TransportCatalogue::AddStop(Stop& stop, std::unordered_map<std::string, int> distances) {
        stop_index_.push_back(stop); //??? copy?

        if (stop_to_stop_distance_.count(stop.Name) == 0) {
            std::pair<std::string, std::unordered_map<std::string, int>> elem (stop.Name, {});
            stop_to_stop_distance_.insert(elem);
        }

        //Возможно задание разного расстояния между двумя остановками в разных направлениях
        std::unordered_map<std::string, int>& stops = stop_to_stop_distance_[stop.Name];
        for(const std::pair<std::string, int>& item : distances) {
            //жестко переопределяем
            stops[item.first] = item.second;
        }

        //мягко переопределяем
        for(const std::pair<std::string, int>& item : distances) {
            if (stop_to_stop_distance_.count(item.first) == 0) {
                std::pair<std::string, std::unordered_map<std::string, int>> elem (item.first, {});
                stop_to_stop_distance_.insert(elem);
            }
            std::unordered_map<std::string, int>& item_stops = stop_to_stop_distance_[item.first];
            if (item_stops.count(stop.Name) == 0) {
                item_stops[stop.Name] = item.second;
            }
        }
    }

    Bus* TransportCatalogue::SearchBus(std::string_view& name) {
        auto f = std::find_if(bus_index_.begin(), bus_index_.end(), [name](Bus& item){
            return item.Name == name;
        });
        if (f == bus_index_.end()) {
            return nullptr;
        }
        return &*f;
    }

    Stop* TransportCatalogue::SearchStop(std::string_view& name) {
        auto f = std::find_if(stop_index_.begin(), stop_index_.end(), [name](Stop& item){
            return item.Name == name;
        });
        if (f == stop_index_.end()) {
            return nullptr;
        }
        return &*f;
    }

    std::vector<std::string_view> TransportCatalogue::Split(std::string_view input, char c) {
        std::vector<std::string_view> res;
        while (true) {
            std::string_view l;
            std::string_view r;
            auto cpos = input.find(c);
            if (cpos == input.npos) {
                l = input;
                r = {};
            }
            else {
                l = input.substr(0, cpos);
                r = input.substr(cpos + 1);
            }

            res.push_back(l);
            if (r.size() == 0) {
                return res;
            }
            input = r;
        }
    }

    std::vector<std::string_view> TransportCatalogue::Split(std::string_view input, std::string& c) {
        std::vector<std::string_view> res;
        while (true) {
            std::string_view l;
            std::string_view r;
            auto cpos = input.find(c);
            if (cpos == input.npos) {
                l = input;
                r = {};
            }
            else {
                l = input.substr(0, cpos);
                r = input.substr(cpos + c.size());
            }

            res.push_back(l);
            if (r.size() == 0) {
                return res;
            }
            input = r;
        }
    }

    std::string_view TransportCatalogue::Trim(std::string_view in) {
        auto left = in.begin();
        for (;; ++left) {
        if (left == in.end())
            return std::string_view();
        if (!isspace(*left))
            break;
        }
        auto right = in.end() - 1;
        for (; right > left && isspace(*right); --right);
        return std::string_view(left, std::distance(left, right) + 1);
    }

    void TransportCatalogue::GetBusesByStop(const Stop* stop, std::set<std::string>& out) {
        const std::string& name = stop->Name;

        if (std::count_if(stop_index_.begin(), stop_index_.end(), [name](Stop& stop){return stop.Name==name;}) > 0) {
            out = stop_to_bus_[stop->Name];
        }
    }

    std::optional<int> TransportCatalogue::GetStopToStopDistance(const Stop* stopStart, const Stop* stopEnd) {
        std::optional<int> ret;
        auto itStart = stop_to_stop_distance_.find(stopStart->Name);
        if (itStart != stop_to_stop_distance_.end()) {
            auto it = itStart->second.find(stopEnd->Name);
            if (it != itStart->second.end()) {
                ret = (*it).second;
            }
        }
        return ret;
    }
}
