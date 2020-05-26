// Copyright (C) 2020 Henrich Lauko <xlauko@mail.muni.cz>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#pragma once

#include <cstdint>
#include <sanitizer/dfsan_interface.h>

namespace __lart::runtime
{
    extern dfsan_label taint;

    template< typename integral > auto make_tainted( integral value )
    {
        dfsan_set_label( taint, &value, sizeof( value ) );
        return value;
    }

    template< typename integral > bool is_tainted( integral value )
    {
        auto label = dfsan_get_label( static_cast< std::int64_t >( value ) );
        return dfsan_has_label( label, taint );
    }

} // namespace __lart::runtime
