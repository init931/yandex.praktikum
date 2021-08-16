//transport_catalogue.h, transport_catalogue.cpp — класс транспортного справочника

#include "transport_catalogue.h"

#include <algorithm>

void TransportCatalogue::AddBus(Bus& bus) {
    std::string name = bus.Name;
    bus_index_.push_back(bus); //??? copy?

    auto bus_shure = std::find_if(bus_index_.begin(), bus_index_.end(), [&name](Bus& item) {
        return item.Name == name;
    });

    // for (const Stop* item : bus_shure->Stops) {
    //     auto it = stop_to_bus_.find(item);
    //     if (it == stop_to_bus_.end()) {
    //         stop_to_bus_.insert({ item, {} });
    //         it = stop_to_bus_.find(item);
    //     }

    //     if (it->second.find(&*bus_shure) == it->second.end()) {
    //         it->second.insert(&*bus_shure);
    //     }
    // }

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

void TransportCatalogue::AddStop(Stop& stop) {
    stop_index_.push_back(stop); //??? copy?
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
    //if (stop_to_bus_.contains(&stop)) {
        out = stop_to_bus_[stop->Name];
    //}
    }
}
