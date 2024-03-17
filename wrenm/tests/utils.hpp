#pragma once

#include <ostream>

auto operator << ( std::ostream& os, T const& value ) -> std::ostream& {
    os << convertMyTypeToString( value );
    return os;
}
