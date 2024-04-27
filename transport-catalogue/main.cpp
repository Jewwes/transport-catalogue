#include <iostream>
#include <string>
#include <vector>

#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
    Catalogue::TransportCatalogue catalogue;
    Catalogue::detail::InputReader::ReadInput(cin, catalogue);
    Catalogue::detail::ParseAndPrintStat(cin,catalogue, cout);
}
