#pragma once

#include <iosfwd>
#include <string_view>

#include "transport_catalogue.h"

namespace TransportCatalogue {

    namespace detail {
        void ParseAndPrintStat(const TransportCatalogue& tansport_catalogue, std::string_view request,
                               std::ostream& output);
    } 

}
