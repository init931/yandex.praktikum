//stat_reader.h, stat_reader.cpp — чтение запросов на вывод и сам вывод

#include "stat_reader.h"

void StatReader::GetInfoBus(std::ostream& out, const std::string& input) {
    //Вывести информацию об автобусе X в следующем формате:
    //Bus X: R stops on route, U unique stops, L route length
    //Bus 750: 7 stops on route, 3 unique stops, 27400 route length, 1.30853 curvature

    std::string_view name = parseSearchBus(input);
    Bus* bus = tc_.SearchBus(name);

    if (bus == nullptr) {
        out << "Bus "s << name << ": not found"s << std::endl;
        return;
    }

    std::unordered_set<std::string_view> stop_names;
    double distance = 0;
    double fact_len = 0;
    for (size_t i = 0; i < bus->Stops.size(); ++i) {
        const Stop* item = bus->Stops[i];
        stop_names.insert(item->Name);

        if (i != 0) {
            const Stop* item_before = bus->Stops[i - 1];
            distance += ComputeDistance({item_before->Latitude, item_before->Longitude}, 
                                        {item->Latitude, item->Longitude});

            std::optional<int> dis = tc_.GetStopToStopDistance(item_before, item);
            if (dis.has_value()) {
                fact_len += dis.value();
            }
            else {
                //???
            }
        }
    }
    double curvature = fact_len / distance;
    
    out << "Bus "s << name << ": "s ;
    out << bus->Stops.size() << " stops on route, "s;
    out << stop_names.size() << " unique stops, "s;
    out << fact_len << " route length, "s;
    out << curvature << " curvature"s;
    out << std::endl;
}

void StatReader::ProcessOutputRequests(std::ostream& out, const std::vector<std::string>& requests) {
    for (const std::string& item : requests) {
        if (item.rfind("Bus "s, 0) == 0) {
            GetInfoBus(out, item);
        }

        if (item.rfind("Stop "s, 0) == 0) {
            GetInfoStop(out, item);
        }
    }
}

std::string_view StatReader::parseSearchBus(const std::string& input) {
    std::string_view sv(input);
    sv.remove_prefix(4);
    return sv;
}

void StatReader::GetInfoStop(std::ostream& out, const std::string& input) {
    // Запрос должен вывести информацию об остановке X в следующем формате:
    // Stop X: buses bus1 bus2 ... busN

    // Stop Samara: not found
    // Stop Prazhskaya: no buses
    // Stop Biryulyovo Zapadnoye: buses 256 828

    std::string_view name(input);
    name.remove_prefix(5);
    const auto stop = tc_.SearchStop(name);
    if (stop == nullptr) {
        out << "Stop "s << name << ": not found"s << std::endl;
        return;
    }

    std::set<std::string> buses;
    tc_.GetBusesByStop(stop, buses);
    if (buses.size() > 0) {
        out << "Stop "s << name << ": buses"s;
        for (const std::string& item : buses) {
            //auto t1 = item->Name;
            out << " "s << item;
        }
        out << std::endl;
    }
    else {
        out << "Stop "s << name << ": no buses"s << std::endl;
    }
}
