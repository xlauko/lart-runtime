#include <catch2/catch.hpp>

#include "lart/taint.hpp"

using namespace __lart::runtime;

TEST_CASE("taint", "[shadow]")
{
    auto x = 10;
    auto t = taint(x);
    REQUIRE(is_tainted(t));
    REQUIRE(t == 10);
}