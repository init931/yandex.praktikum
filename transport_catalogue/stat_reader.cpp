//stat_reader.h, stat_reader.cpp — чтение запросов на вывод и сам вывод

#include "stat_reader.h"

void StatReader::GetInfoBus(std::ostream& out, const std::string& input) {
    //Вывести информацию об автобусе X в следующем формате:
    //Bus X: R stops on route, U unique stops, L route length

    std::string_view name = parseSearchBus(input);
    Bus* bus = tc_.SearchBus(name);

    if (bus == nullptr) {
        out << "Bus "s << name << ": not found"s << std::endl;
        return;
    }

    std::unordered_set<std::string_view> stop_names;
    double distance = 0;
    for (size_t i = 0; i < bus->Stops.size(); ++i) {
        const Stop* item = bus->Stops[i];
        stop_names.insert(item->Name);

        if (i != 0) {
            const Stop* item_before = bus->Stops[i - 1];
            distance += ComputeDistance({item_before->Latitude, item_before->Longitude}, 
                                        {item->Latitude, item->Longitude});
        }
    }
    
    out << "Bus "s << name << ": "s ;
    out << bus->Stops.size() << " stops on route, "s;
    out << stop_names.size() << " unique stops, "s;
    out << distance << " route length"s;
    out << std::endl;
}

void StatReader::ProcessOutputRequests(std::ostream& out, const std::vector<std::string>& requests) {
    for (const std::string& item : requests) {
        if (item.rfind("Bus "s, 0) == 0) {
            GetInfoBus(out, item);
        }
    }
}

std::string_view StatReader::parseSearchBus(const std::string& input) {
    std::string_view sv(input);
    sv.remove_prefix(4);
    return sv;
}
