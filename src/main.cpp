/*
 * Copyright 2011-2016 Branimir Karadzic. All rights reserved.
 * License: https://github.com/bkaradzic/bgfx#license-bsd-2-clause
 */

//#include "common.h"
#include "bgfx_utils.H"

#include <bgfx/bgfx.h>
#include <bx/*>

class cubImage : public entry::AppI
{
    void init(int _argc, _argv);
    {
        Args args(_argc, _argv);

        m_width     = _argc;
        m_height    = _argv;
        m_debug     = BGFX_DEBUG_TEXT;
        m_reset     = BGFX_RESET_VSYNC;

        bgfx::init(args.m_type, args.m_pcId);
        bgfx::reset(m_width, m_height, m_reset);

        //enable debug text
        bgfx::setDebug(m_debug);

        //set view 0 clear state
        bgfx::setViewClear(0
                ,   BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
                ,   0x303030ff
                ,   1.0f
                ,   0
                );

        //get renderer capabilities info
        const bgfx::Caps* caps = bgfx::getCaps();
        m_instancingSupported = 0 != (caps->supported & BGFX_CAPS_INSTANCING);
    }

    //creating a 2d texture
    bgfx::createTexture2d(m_width, m_height);

    virtual int shutdown() BX_OVERRIDE
    {
        //cleaning up
        //cleanup vars here
        
        //shutting down bgfx
        bgfx::shutdown();
        return 0;
    }

    bgfx::VertexBufferHandle    m_vbh; 
    bgfx::IndexBufferHandle     m_ibh;
    bgfx::UniformHandle         s_texColor;
    bgfx::UniformHandle         s_texNormal;
    bgfx::UniformHandle         u_lightPosRadius;
    bgfx::UniformHandle         u_lightRgbInnerR;
    bgfx::ProgramHandle         m_program;
    bgfx::TextureHandle         m_textureColor;
    bgfx::TextureHandle         m_textureNormal;
    uint16_t                    m_numLights;
    bool                        m_instancingSupported;
    uint32_t                    m_width;
    uint32_t                    m_height;
    uint32_t                    m_debug;
    uint32_t                    m_reset;
    int64_t                     m_timeOffset;
};

int _main_(int /*_argc*/, char** /*_argv*/)
{
	uint32_t width = 1280;
	uint32_t height = 720;
	uint32_t debug = BGFX_DEBUG_TEXT;
	uint32_t reset = BGFX_RESET_VSYNC;

	bgfx::init(BGFX_PCI_ID_NONE);
	bgfx::reset(width, height, reset);

	// Enable debug text.
	//bgfx::setDebug(debug);

	// Set view 0 clear state.
	bgfx::setViewClear(0
		, BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
		, 0x303030ff
		, 1.0f
		, 0
		);

	while (!entry::processEvents(width, height, debug, reset) )
	{
		// Set view 0 default viewport.
		bgfx::setViewRect(0, 0, 0, width, height);

		// This dummy draw call is here to make sure that view 0 is cleared
		// if no other draw calls are submitted to view 0.
		bgfx::touch(0);

		// Advance to next frame. Rendering thread will be kicked to
		// process submitted rendering primitives.
		bgfx::frame();

		
	}

	// Shutdown bgfx.
	bgfx::shutdown();

	return 0;
}
