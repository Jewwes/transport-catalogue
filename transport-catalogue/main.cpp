#include <iostream>
#include <string>
#include <vector>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    TransportCatalogue::TransportCatalogue catalogue;
    TransportCatalogue::detail::InputReader::ReadInput(cin, catalogue);
    TransportCatalogue::detail::ParseAndPrintStat(cin,catalogue, cout);
}
