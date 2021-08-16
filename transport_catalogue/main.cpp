#include "input_reader.h"
#include "stat_reader.h"
#include "transport_catalogue.h"

//
#include "input_reader.cpp"
#include "stat_reader.cpp"
#include "transport_catalogue.cpp"
//

#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <vector>

using std::string_literals::operator""s;

int main() {
    TransportCatalogue tc;
    InputReader ir(tc);
    StatReader sr(tc);

    std::cout << std::setprecision(6);
    
    std::ifstream input_file("../transport_catalogue/input1.txt");
    //std::istream& input_file = std::cin;

    int req_count;
	input_file >> req_count;
    std::vector<std::string> requests;
    requests.reserve(req_count);
	for (int i = 0; i <= req_count; ++i) {
		std::string line;
        getline(input_file, line);
        //std::cout << line << std::endl;
        requests.push_back(move(line));
	}
    ir.ProcessInputRequests(requests);

	input_file >> req_count;
    requests.clear();
    requests.reserve(req_count);
	for (int i = 0; i <= req_count; ++i) {
		std::string line;
        getline(input_file, line);
        //std::cout << line << std::endl;
        requests.push_back(move(line));
	}
    sr.ProcessOutputRequests(std::cout, requests);

    input_file.close();
    return 0;
}
