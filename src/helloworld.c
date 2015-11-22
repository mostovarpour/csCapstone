/*
 * Copyright 2011-2015 Branimir Karadzic. All rights reserved.
 * License: http://www.opensource.org/licenses/BSD-2-Clause
 */

#include <bgfx/c99/bgfx.h>
#include "headers/logo.h"
#include "headers/config.h"
#include "headers/sampler.h"

extern bool entry_process_events(uint32_t* _width, uint32_t* _height, uint32_t* _debug, uint32_t* _reset);

uint16_t uint16_max(uint16_t _a, uint16_t _b)
{
    return _a < _b ? _b : _a;
}

int _main_(int _argc, char** _argv)
{
    params parms;
    SVImage img;
    parms.file = "/home/leah/Pictures/mars.tif";
    parms.xLow = 0;
    parms.xHigh = 1000;
    parms.yLow = 0;
    parms.yHigh = 1000;
    parms.target = &img;
    sample(&parms);

    uint32_t width  = 1280;
    uint32_t height = 720;
    uint32_t debug  = BGFX_DEBUG_TEXT;
    uint32_t reset  = RESET_PARAMS;
    (void)_argc;
    (void)_argv;


    bgfx_init(  RENDER_TYPE  // defined in config.h, Direct3D12 for windows and OpenGL on other systems
                , BGFX_PCI_ID_NONE // Autoselect gpu vendor
                , 0 // use default gpu...
                , NULL // Callback interface that handles events. We'll probably set this later
                , NULL // allocator? I hope we can leave this null
                );
    bgfx_reset(width, height, reset);

    // Enable debug text.
    bgfx_set_debug(debug);

    bgfx_set_view_clear(0
            , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
            , 0x303030ff
            , 1.0f
            , 0
            );

    while (!entry_process_events(&width, &height, &debug, &reset) )
    {
        // Set view 0 default viewport.
        bgfx_set_view_rect(0, 0, 0, width, height);

        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to view 0.
        bgfx_touch(0);

        // Use debug font to print information about this example.
        bgfx_dbg_text_clear(0, false);
        bgfx_dbg_text_image(uint16_max(width/2/8, 20)-20
                                         , uint16_max(height/2/16, 6)-6
                                         , 40
                                         , 12
                                         , s_logo
                                         , 160
                                         );
        bgfx_dbg_text_printf(0, 1, 0x4f, "bgfx/examples/25-c99");
        bgfx_dbg_text_printf(0, 2, 0x6f, "Description: Initialization and debug text with C99 API.");

        // Advance to next frame. Rendering thread will be kicked to
        // process submitted rendering primitives.
        bgfx_frame();
    }

    // Shutdown bgfx.
    bgfx_shutdown();

    return 0;
}
