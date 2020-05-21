// Copyright (C) 2020 Henrich Lauko <xlauko@mail.muni.cz>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "lart/shadow.hpp"

#include "utils.hpp"

#include <sanitizer/dfsan_interface.h>

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

} // namespace __lart::runtime
