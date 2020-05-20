#include "lart/taint.hpp"

#include <catch2/catch.hpp>

using namespace __lart::runtime;

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
        int *ptr = reinterpret_cast< int * >( &p );

        REQUIRE_FALSE( is_tainted( ptr[ 0 ] ) );
        REQUIRE( is_tainted( ptr[ 1 ] ) );
    }
}

TEST_CASE( "array", "[taint]" )
{
    auto t = make_tainted( 2 );

    int arr[ 10 ] = { 0 };

    int idx    = 3;
    arr[ idx ] = t;

    for ( int i = 0; i < 10; ++i )
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
        int i;
        long l;
        char buff[ 8 ];
    } data {};

    data.i = t;

    REQUIRE( is_tainted( data.i ) );
    REQUIRE( is_tainted( data.l ) );
    REQUIRE( is_tainted( data.buff[ 3 ] ) );
    REQUIRE_FALSE( is_tainted( data.buff[ 7 ] ) );
}