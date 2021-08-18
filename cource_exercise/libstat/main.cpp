#include "libstat.h"

//
#include "libstat.cpp"
#include "libstat_test.cpp"
//

#include <iostream>
using namespace statistics::tests;

int main() {
    AggregSum();
    AggregMax();
    AggregMean();
    AggregStandardDeviation();
    AggregMode();
    statistics::tests::AggregPrinter();

    std::cout << "Test passed!"sv << std::endl;
}
