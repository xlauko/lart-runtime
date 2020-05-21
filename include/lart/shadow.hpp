// Copyright (C) 2020 Henrich Lauko <xlauko@mail.muni.cz>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#pragma once

#include <cstddef>

namespace __lart::runtime
{
    void poke( void *addr, std::size_t size, void *value );
    void *peek( void *addr );
} // namespace __lart::runtime