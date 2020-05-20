#pragma once

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
        auto label = dfsan_get_label( value );
        return dfsan_has_label( label, taint );
    }

} // namespace __lart::runtime
