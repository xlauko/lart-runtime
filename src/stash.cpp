// Copyright (C) 2020 Henrich Lauko <xlauko@mail.muni.cz>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "lart/stash.hpp"

namespace __lart::runtime
{
    thread_local stash_register_t stash_register = 0;
}