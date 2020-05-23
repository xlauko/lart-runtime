// Copyright (C) 2020 Henrich Lauko <xlauko@mail.muni.cz>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#pragma once

#include "detail/generator.hpp"

#include <cstddef>
#include <compare>

namespace __lart::runtime
{
    void poke( void *addr, std::size_t size, void *value );
    void *peek( void *addr );

    using shadow_t = void *;

    struct peeked
    {
        shadow_t value;
        std::size_t offset;
    };

    using cppcoro::generator;
    generator< peeked > peek( const void *addr, std::size_t size );

} // namespace __lart::runtime