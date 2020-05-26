// Copyright (C) 2020 Henrich Lauko <xlauko@mail.muni.cz>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "lart/taint.hpp"

#include <array>
#include <catch2/catch.hpp>

using namespace __lart::runtime; // NOLINT

TEST_CASE( "taint constant", "[taint]" )
{
    auto t = make_tainted( 42 );
    REQUIRE( is_tainted( t ) );
    REQUIRE( t == 42 );
}

TEST_CASE( "taint variable", "[taint]" )
{
    int value = 42;
    auto t    = make_tainted( value );
    REQUIRE( is_tainted( t ) );
    REQUIRE( t == 42 );
}

TEST_CASE( "taint manipulations", "[taint]" )
{
    auto t = make_tainted( 0 );
    REQUIRE( is_tainted( t ) );

    SECTION( "assignment" )
    {
        int x = t;
        REQUIRE( is_tainted( x ) );
    }

    SECTION( "arithmetic" )
    {
        int x = 10 + t;
        REQUIRE( is_tainted( x ) );
    }

    SECTION( "relation" )
    {
        bool b = ( t == 0 );
        REQUIRE( is_tainted( b ) );
    }
}

template< typename T > auto dummy( T value )
{
    return value;
}

TEST_CASE( "taint function propagation", "[taint]" )
{
    auto t = make_tainted( 0 );
    auto r = dummy( t );
    REQUIRE( is_tainted( r ) );
    REQUIRE( r == 0 );
}

template< typename T > struct pair
{
    T first, second;
};

TEST_CASE( "structures", "[taint]" )
{
    auto t = make_tainted( 2 );

    SECTION( "first" )
    {
        pair< int > p { t, 10 };

        REQUIRE( is_tainted( p.first ) );
        REQUIRE_FALSE( is_tainted( p.second ) );
    }

    SECTION( "second" )
    {
        pair< int > p { 10, t };

        REQUIRE_FALSE( is_tainted( p.first ) );
        REQUIRE( is_tainted( p.second ) );
    }

    SECTION( "both" )
    {
        pair< int > p { t, t };

        REQUIRE( is_tainted( p.first ) );
        REQUIRE( is_tainted( p.second ) );
    }

    SECTION( "address taken" )
    {
        pair< int > p { 10, t };
        int *ptr = reinterpret_cast< int * >( &p ); // NOLINT

        REQUIRE_FALSE( is_tainted( ptr[ 0 ] ) ); // NOLINT
        REQUIRE( is_tainted( ptr[ 1 ] ) );       // NOLINT
    }
}

TEST_CASE( "array", "[taint]" )
{
    auto t = make_tainted( 2 );

    std::array< int, 10 > arr = {};

    std::size_t idx = 3;
    arr[ idx ]      = t;

    for ( std::size_t i = 0; i < 10; ++i )
        if ( i == idx )
            REQUIRE( is_tainted( arr[ i ] ) );
        else
            REQUIRE_FALSE( is_tainted( arr[ i ] ) );
}

TEST_CASE( "union", "[taint]" )
{
    auto t = make_tainted( 2 );

    union
    {
        std::uint32_t i;
        std::uint64_t l;
        char buff[ 8 ]; // NOLINT
    } data {};

    data.i = static_cast< std::uint32_t >( t );    // NOLINT
    REQUIRE( is_tainted( data.i ) );               // NOLINT
    REQUIRE( is_tainted( data.l ) );               // NOLINT
    REQUIRE( is_tainted( data.buff[ 3 ] ) );       // NOLINT
    REQUIRE_FALSE( is_tainted( data.buff[ 7 ] ) ); // NOLINT
}