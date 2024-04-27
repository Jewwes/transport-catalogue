#pragma once

#include <iosfwd>
#include <string_view>
#include <iomanip>
#include "transport_catalogue.h"

namespace Catalogue {

namespace detail {
        void ParseAndPrintStat(std::istream& in, TransportCatalogue& catalogue, std::ostream& out);
        void PrintRoute(std::string& line, TransportCatalogue& catalogue, std::ostream& out);
        void PrintStop(std::string& line, TransportCatalogue& catalogue, std::ostream& out);
} 
}
