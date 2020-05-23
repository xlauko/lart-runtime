// Copyright (C) 2020 Henrich Lauko <xlauko@mail.muni.cz>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "lart/shadow.hpp"

#include "utils.hpp"

#include <iterator>
#include <sanitizer/dfsan_interface.h>
#include <span>

namespace __lart::runtime
{
    void poke( void *addr, size_t size, void *value )
    {
        auto shadow = dfsan_create_label( "shadow", value );
        dfsan_set_label( shadow, addr, size );
    }

    void *peek( void *addr )
    {
        auto meta = dfsan_read_label( addr, 1 ); // TODO interval melt
        auto info = dfsan_get_label_info( meta );
        return info->userdata;
    }

    shadow_t peek( const void *addr )
    {
        auto meta = dfsan_read_label( addr, 1 );
        auto info = dfsan_get_label_info( meta );
        return info->userdata;
    }

    generator< peeked > peek( const void *addr, std::size_t size )
    {
        auto current = peek( addr );
        co_yield peeked{ current, 0 }; // yield first shadow segment
        for ( std::size_t off = 1; off < size; ++off ) {
            auto at = static_cast< const std::byte * >( addr ) + off;
            if ( auto shadow = peek( at ); shadow != current ) {
                current = shadow;
                co_yield peeked{ current, off };
            }
        }
    }

} // namespace __lart::runtime
