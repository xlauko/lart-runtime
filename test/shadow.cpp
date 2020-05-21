// Copyright (C) 2020 Henrich Lauko <xlauko@mail.muni.cz>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "lart/shadow.hpp"

#include <catch2/catch.hpp>
#include <cstdlib>

using namespace __lart::runtime;

TEST_CASE( "simple", "[shadow]" )
{
    auto *ptr = new int();
    auto *val = new int();

    poke( ptr, 4, val );
    REQUIRE( peek( ptr ) == val );

    delete ptr;
    delete val;
}

TEST_CASE( "memory", "[shadow]" )
{
    auto *arr = new unsigned int[ 5 ]();

    unsigned int val = 42;
    poke( &arr[ 3 ], 4, &val );

    REQUIRE( peek( &arr[ 3 ] ) == &val );

    delete[] arr;
}