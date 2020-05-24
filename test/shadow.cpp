// Copyright (C) 2020 Henrich Lauko <xlauko@mail.muni.cz>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "lart/shadow.hpp"

#include <array>
#include <catch2/catch.hpp>
#include <cstdlib>
#include <iterator>

using namespace __lart::runtime;

namespace __lart::runtime
{
    shadow_t peek( const void *addr );
}

TEST_CASE( "simple shadow", "[shadow]" )
{
    auto ptr = std::make_unique< char >();
    auto val = std::make_unique< char >();

    poke( ptr.get(), 1, val.get() );
    REQUIRE( peek( ptr.get() ) == val.get() );

    SECTION( "another poke" )
    {
        auto ptr2 = std::make_unique< char >();
        auto val2 = std::make_unique< char >();

        poke( ptr2.get(), 1, val2.get() );

        REQUIRE( peek( ptr2.get() ) == val2.get() );
        REQUIRE( peek( ptr.get() ) == val.get() );
    }

    SECTION( "rewrite" )
    {
        auto val2 = std::make_unique< char >();
        poke( ptr.get(), 1, val2.get() );

        REQUIRE( peek( ptr.get() ) == val2.get() );
    }
}

TEST_CASE( "memory", "[shadow]" )
{
    auto arr = std::make_unique< char[] >( 5 );

    unsigned char val = 42;
    poke( &arr[ 3 ], 1, &val );

    REQUIRE( peek( &arr[ 3 ] ) == &val );
}

TEST_CASE( "peek empty memory", "[shadow]" )
{
    auto ptr = std::make_unique< char >();
    REQUIRE( peek( ptr.get() ) == nullptr );
}

TEST_CASE( "interval peek", "[shadow]" )
{
    using P = peeked;

    auto test = []( auto peek, auto expected ) {
        std::size_t i = 0;
        for ( auto shadow : peek() ) {
                REQUIRE( shadow.value == expected[ i ].value );
                REQUIRE( shadow.offset == expected[ i ].offset );
                ++i;
            }

        REQUIRE( i == expected.size() );
    };

    constexpr std::size_t size = 10;
    auto arr                   = std::make_unique< char[] >( size );

    SECTION( "empty interval" )
    {
        std::array expected = { P { nullptr, 0 } };
        test( [ & ] { return peek( arr.get(), size ); }, expected );
    }

    auto a = std::make_unique< char >();

    SECTION( "single value at begin" )
    {
        poke( arr.get(), 1, a.get() );

        std::array expected = { P { a.get(), 0 }, P { nullptr, 1 } };
        test( [ & ] { return peek( arr.get(), size ); }, expected );
    }

    SECTION( "single value in middle" )
    {
        poke( &arr[ 4 ], 1, a.get() );

        std::array expected = { P { nullptr, 0 }, P { a.get(), 4 }, P { nullptr, 5 } };
        test( [ & ] { return peek( arr.get(), size ); }, expected );
    }

    SECTION( "single value at end" )
    {
        poke( &arr[ size - 1 ], 1, a.get() );

        std::array expected = { P { nullptr, 0 }, P { a.get(), 9 } };
        test( [ & ] { return peek( arr.get(), size ); }, expected );
    }

    auto b = std::make_unique< char >();

    SECTION( "two values" )
    {
        poke( &arr[ 3 ], 2, a.get() );
        poke( &arr[ 7 ], 1, b.get() );

        std::array expected = { P { nullptr, 0 },
                                P { a.get(), 3 },
                                P { nullptr, 5 },
                                P { b.get(), 7 },
                                P { nullptr, 8 } };
        test( [ & ] { return peek( arr.get(), size ); }, expected );
    }

    SECTION( "overlap two values" )
    {
        poke( &arr[ 2 ], 5, a.get() );
        poke( &arr[ 4 ], 2, b.get() );

        std::array expected = { P { nullptr, 0 },
                                P { a.get(), 2 },
                                P { b.get(), 4 },
                                P { a.get(), 6 },
                                P { nullptr, 7 } };
        test( [ & ] { return peek( arr.get(), size ); }, expected );
    }
}
