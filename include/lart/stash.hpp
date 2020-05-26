// Copyright (C) 2020 Henrich Lauko <xlauko@mail.muni.cz>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#pragma once

#include <cstdint>
#include <type_traits>

namespace __lart::runtime
{
    using stash_register_t = std::uint64_t;

    extern thread_local stash_register_t stash_register;

    template< typename T > void stash( T value )
    {
        if constexpr ( std::is_pointer_v< T > )
            stash_register = reinterpret_cast< std::uintptr_t >( value ); // NOLINT
        else
            stash_register = static_cast< std::uint64_t >( value );
    }

    template< typename T > T unstash()
    {
        auto ret = [] {
            if constexpr ( std::is_pointer_v< T > )
                return reinterpret_cast< T >( stash_register ); // NOLINT
            else
                return static_cast< T >( stash_register );
        }();

        stash_register = 0; // reset stash_register
        return ret;
    }

} // namespace __lart::runtime