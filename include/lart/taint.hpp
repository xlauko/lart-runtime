#pragma once

#include <concepts>

namespace __lart::runtime
{
    auto taint( std::integral auto value ) 
    {
        return value;
    }

    bool is_tainted( std::integral auto /*value*/ )
    {
        return false;
    }
}