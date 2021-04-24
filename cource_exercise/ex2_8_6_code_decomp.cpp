#include <cassert>
#include <iostream>
#include <sstream>
#include <map>
#include <string>
#include <vector>
#include <set>
#include <algorithm>

using namespace std;

enum class QueryType {
    NewBus,
    BusesForStop,
    StopsForBus,
    AllBuses,
};

struct Query {
    QueryType type;
    string bus;
    string stop;
    vector<string> stops;
};

istream& operator>>(istream& is, Query& q) {
    string operation_code;
    is >> operation_code;
    q.stops = {};
    q.stop = "";
    q.bus = "";

    if (operation_code == "ALL_BUSES"s) {
        q.type = QueryType::AllBuses;
    }
    else if (operation_code == "BUSES_FOR_STOP"s) {
        q.type = QueryType::BusesForStop;
        is >> q.stop;
    }
    else if (operation_code == "STOPS_FOR_BUS"s) {
        q.type = QueryType::StopsForBus;
        is >> q.bus;
    }
    else if (operation_code == "NEW_BUS"s) {
        q.type = QueryType::NewBus;
        is >> q.bus;
        int count_stops = 0;
        is >> count_stops;
        for (int i = 0; i < count_stops; ++i) {
            string stop_name;
            is >> stop_name;
            q.stops.push_back(stop_name);
        }
    }

    return is;
}

struct BusesForStopResponse {
    vector<string> buses;
};

ostream& operator<<(ostream& os, const BusesForStopResponse& r) {
    if (r.buses.size() == 0) {
        os << "No stop"s;
    } else {
        bool is_first = true;
        for (const string& bus : r.buses) {
            if (is_first) {
                os << bus;
                is_first = false;
            }
            else {
                os << " "s << bus;
            }
        }
    }

    return os;
}

struct StopsForBusResponse {
    string bus;
    vector<pair<string, vector<string>>> stop_to_buses;
};

ostream& operator<<(ostream& os, const StopsForBusResponse& r) {
    if (r.stop_to_buses.size() == 0) {
        os << "No bus"s;
    } else {
        bool is_first_line = true;
        for (const auto& [stop, buses] : r.stop_to_buses) {
            if (!is_first_line) {
                os << std::endl;
            }
            else {
                is_first_line = false;
            }
            os << "Stop "s << stop << ": "s;
            if (buses.size() == 1) {
                os << "no interchange"s;
            } else {
                bool is_first = true;
                for (const string& other_bus : buses) {
                    if (r.bus != other_bus) {
                        if (is_first) {
                            os << other_bus;
                            is_first = false;
                        }
                        else {
                            os << " "s << other_bus;
                        }
                    }
                }
            }
        }
    }
    return os;
}

struct AllBusesResponse {
    vector<pair<string, vector<string>>> buses_to_stops;
};

ostream& operator<<(ostream& os, const AllBusesResponse& r) {
    if (r.buses_to_stops.size() == 0) {
        os << "No buses"s;
    } else {
        bool is_first_line = true;
        for (const auto& [bus, stops] : r.buses_to_stops) {
            if (!is_first_line) {
                os << std::endl;
            }
            else {
                is_first_line = false;
            }
            os << "Bus "s << bus << ": "s;
            bool is_first = true;
            for (const string& stop : stops) {
                if (is_first) {
                    os << stop;
                    is_first = false;
                }
                else {
                    os << " "s << stop;
                }
            }
        }
    }
    return os;
}

class BusManager {
private:
    map<string, vector<string>> buses_to_stops, stops_to_buses;

public:
    void AddBus(const string& bus, const vector<string>& stops) {
        for (const string& stop : stops) {
            buses_to_stops[bus].push_back(stop);
            stops_to_buses[stop].push_back(bus);
        }
    }

    BusesForStopResponse GetBusesForStop(const string& stop) const {
        BusesForStopResponse resp = {};
        if (stops_to_buses.count(stop) > 0) {
            for (const string& bus : stops_to_buses.at(stop)) {
                resp.buses.push_back(bus);
            }
        }
        return resp;
    }

    StopsForBusResponse GetStopsForBus(const string& bus) const {
        StopsForBusResponse resp = {};
        resp.bus = bus;
        if (buses_to_stops.count(bus) > 0) {
            for (const string& stop : buses_to_stops.at(bus)) {
                vector<string> vec = {bus};
                for (const string& other_bus : stops_to_buses.at(stop)) {
                    if (bus != other_bus) {
                        vec.push_back(other_bus);
                    }
                }
                resp.stop_to_buses.push_back({stop, vec});
            }
        }
        return resp;
    }

    AllBusesResponse GetAllBuses() const {
        AllBusesResponse resp = {};
        for (const auto& [bus, stops] : buses_to_stops) {
            resp.buses_to_stops.push_back({bus, stops});
        }
        return resp;
    }
};

//################################ TESTS
void TestParseInputQuery_AllBuses() {
    istringstream input;
    input.str("ALL_BUSES"s);
    Query q;
    input >> q;
    assert(q.type == QueryType::AllBuses);
}

void TestParseInputQuery_BusesForStop() {
    istringstream input;
    input.str("BUSES_FOR_STOP Marushkino"s);
    Query q;
    input >> q;
    assert(q.type == QueryType::BusesForStop);
    assert(q.stop == "Marushkino");
}

void TestParseInputQuery_StopsForBus() {
    istringstream input;
    input.str("STOPS_FOR_BUS 32K"s);
    Query q;
    input >> q;
    assert(q.type == QueryType::StopsForBus);
    assert(q.bus == "32K");
}

void TestParseInputQuery_NewBus() {
    istringstream input;
    input.str("NEW_BUS 32 3 Tolstopaltsevo Marushkino Vnukovo"s);
    Query q;
    input >> q;
    assert(q.type == QueryType::NewBus);
    assert(q.bus == "32");
    assert(q.stops.size() == 3);
    assert(q.stops[0] == "Tolstopaltsevo"s);
    assert(q.stops[1] == "Marushkino"s);
    assert(q.stops[2] == "Vnukovo"s);
}

void TestOutput_BusesForStop() {
    BusesForStopResponse resp = {};
    ostringstream output;
    output << resp;
    assert(output.str() == "No stop"s);
    
    output.str(""s);
    resp.buses.push_back("bus1"s);
    output << resp;
    assert(output.str() == "bus1"s);

    output.str(""s);
    resp.buses.push_back("bus2"s);
    output << resp;
    assert(output.str() == "bus1 bus2"s);
}

void TestOutput_StopsForBus() {
    StopsForBusResponse resp;
    ostringstream output;

    output << resp;
    assert(output.str() == "No bus"s);

    output.str(""s);
    resp.bus = "272"s;
    resp.stop_to_buses.push_back({"Vnukovo"s, {"272"s, "32"s, "32K"s, "950"s}});
    resp.stop_to_buses.push_back({"Moskovsky"s, {"272"s}});
    resp.stop_to_buses.push_back({"Rumyantsevo"s, {"272"s}});
    resp.stop_to_buses.push_back({"Troparyovo"s, {"272"s, "950"s}});
    output << resp;
    assert(output.str() == 
        "Stop Vnukovo: 32 32K 950\n"s + 
        "Stop Moskovsky: no interchange\n"s +
        "Stop Rumyantsevo: no interchange\n"s + 
        "Stop Troparyovo: 950"s);
}

void TestOutput_AllBuses() {
    AllBusesResponse resp;
    ostringstream output;

    output << resp;
    assert(output.str() == "No buses"s);

    output.str(""s);
    resp.buses_to_stops.push_back({"272"s, {"Vnukovo"s, "Moskovsky"s, "Rumyantsevo"s, "Troparyovo"s}});
    resp.buses_to_stops.push_back({"32"s, {"Tolstopaltsevo"s}});
    output << resp;
    assert(output.str() == 
        "Bus 272: Vnukovo Moskovsky Rumyantsevo Troparyovo\n"s + 
        "Bus 32: Tolstopaltsevo"s);
}

void TestBusManager_AddBus() {
    BusManager bm = {};
    bm.AddBus("32"s, {"Vnukovo"s, "Moskovsky"s, "Rumyantsevo"s});

    ostringstream output;
    output << bm.GetAllBuses();
    assert(output.str() == "Bus 32: Vnukovo Moskovsky Rumyantsevo"s);
}

void TestBusManager_GetBusesForStop() {
    BusManager bm = {};
    bm.AddBus("32"s, {"Vnukovo"s, "Moskovsky"s, "Rumyantsevo"s});
    bm.AddBus("32K"s, {"Moskovsky"s, "Rumyantsevo"s});

    ostringstream output;
    output << bm.GetBusesForStop("Moskovsky"s);
    assert(output.str() == "32 32K"s);
}

void TestBusManager_GetStopsForBus() {
    BusManager bm = {};
    bm.AddBus("32"s, {"Vnukovo"s, "Moskovsky"s, "Rumyantsevo"s});
    bm.AddBus("32K"s, {"Moskovsky"s, "Rumyantsevo"s});

    ostringstream output;
    output << bm.GetStopsForBus("32K"s);
    assert(output.str() == "Stop Moskovsky: 32\nStop Rumyantsevo: 32"s);
}

void TestBuses() {
    TestParseInputQuery_AllBuses();
    TestParseInputQuery_BusesForStop();
    TestParseInputQuery_StopsForBus();
    TestParseInputQuery_NewBus();

    TestOutput_BusesForStop();
    TestOutput_StopsForBus();
    TestOutput_AllBuses();

    TestBusManager_AddBus();
    TestBusManager_GetBusesForStop();
    TestBusManager_GetStopsForBus();
}
//################################

int main() {
    TestBuses();

    int query_count;
    Query q;

    cin >> query_count;

    BusManager bm;
    for (int i = 0; i < query_count; ++i) {
        cin >> q;
        switch (q.type) {
            case QueryType::NewBus:
                bm.AddBus(q.bus, q.stops);
                break;
            case QueryType::BusesForStop:
                cout << bm.GetBusesForStop(q.stop) << endl;
                break;
            case QueryType::StopsForBus:
                cout << bm.GetStopsForBus(q.bus) << endl;
                break;
            case QueryType::AllBuses:
                cout << bm.GetAllBuses() << endl;
                break;
        }
    }
}