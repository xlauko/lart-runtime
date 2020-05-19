#include "lart/taint.hpp"

#include <catch2/catch.hpp>

using namespace __lart::runtime;

TEST_CASE( "taint", "[taint]" )
{
    int x  = 0;
    auto t = make_tainted( x );
    REQUIRE( is_tainted( t ) );
}