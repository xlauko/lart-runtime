// Copyright (C) 2020 Henrich Lauko <xlauko@mail.muni.cz>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "lart/stash.hpp"

#include <catch2/catch.hpp>
#include <latch>
#include <memory>
#include <thread>

using namespace __lart::runtime; // NOLINT

TEST_CASE( "simple stash", "[stash]" )
{
    auto v = 10;
    stash( v );

    REQUIRE( unstash< decltype( v ) >() == v );
    // check that stash resets its content
    REQUIRE( unstash< decltype( v ) >() == 0 );
}

TEST_CASE( "simple pointer", "[stash]" )
{
    auto ptr = std::make_unique< int >();
    stash( ptr.get() );

    REQUIRE( unstash< void * >() == ptr.get() );
    // check that stash resets its content
    REQUIRE( unstash< void * >() == nullptr );
}

TEST_CASE( "stash over", "[stash]" )
{
    auto a = std::make_unique< int >();
    auto b = std::make_unique< int >();

    stash( a.get() );
    stash( b.get() );

    REQUIRE( unstash< void * >() == b.get() );
}

TEST_CASE( "unstash in function", "[stash]" )
{
    auto a = std::make_unique< int >();

    stash( a.get() );
    [ & ] { REQUIRE( unstash< void * >() == a.get() ); }();
}

TEST_CASE( "stash in function", "[stash]" )
{
    auto a = std::make_unique< int >();

    [ & ] { stash( a.get() ); }();
    REQUIRE( unstash< void * >() == a.get() );
}

TEST_CASE( "thread unstash", "[stash]" )
{
    std::latch stashed( 2 );

    auto job = [ & ] {
        auto ptr = std::make_unique< int >();
        stash( ptr.get() );
        stashed.arrive_and_wait();

        REQUIRE( unstash< void * >() == ptr.get() );
    };

    std::thread thr1 { job };
    std::thread thr2 { job };

    thr1.join();
    thr2.join();
}