// Copyright (C) 2020 Henrich Lauko <xlauko@mail.muni.cz>
// This file is subject to the license terms in the LICENSE file
// found in the top-level directory of this distribution.

#include "lart/taint.hpp"

#include "utils.hpp"
namespace __lart::runtime
{
    dfsan_label taint;

    constructor void init_taint()
    {
        taint = dfsan_create_label( "taint", nullptr );
    }

} // namespace __lart::runtime