#include "common.h"
#include "bgfx_utils.h"
#include <GDAL/gdal_priv.h>

struct TexCoordVertex
{
    float posX;
    float posY;
    float coordX;
    float coordY;

    static void init()
    {
        decl
            .begin()
            .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
            .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float);
    }
    static bgfx::VertexDecl decl;
};

bgfx::VertexDecl TexCoordVertex::decl;

static TexCoordVertex textureVertices[4] =
{
    {-1.0f, 1.0f, 0.0f, 0.0f},
    { 1.0f, 1.0f, 1.0f, 0.0f},
    { 1.0f, -1.0f, 1.0f, 1.0f},
    {-1.0f, -1.0f, 0.0f, 1.0f}
};

static const uint16_t indices[6] =
{
    0, 1, 2,
    2, 3, 0
};

class Renderer : public entry::AppI
{
    bgfx::VertexBufferHandle m_vbh;
    bgfx::IndexBufferHandle  m_ibh;
    bgfx::UniformHandle s_texColor;
    bgfx::UniformHandle s_texNormal;
    bgfx::UniformHandle u_lightPosRadius;
    bgfx::UniformHandle u_lightRgbInnerR;
    bgfx::ProgramHandle m_program;
    bgfx::TextureHandle m_textureColor;
    bgfx::TextureHandle m_textureNormal;
    uint16_t m_numLights;
    bool m_instancingSupported;

    uint32_t m_width;
    uint32_t m_height;
    uint32_t m_debug;
    uint32_t m_reset;

    /*
     * Load gdal image
     */
    bgfx::TextureHandle loadImage(const char *filepath)
    {
        GDALAllRegister();
        GDALDataset *dataset = (GDALDataset *) GDALOpen(filepath, GA_ReadOnly);
        GDALRasterBand *band = (GDALRasterBand *) dataset->GetRasterBand(1);
        if(dataset == NULL)
        {
            puts("JOUSTON WE HAVE A PROBLEM");
        }
        int width = dataset->GetRasterXSize();
        int height = dataset->GetRasterYSize();
        void *buffer = malloc(sizeof(GByte) * m_width * m_height);
        CPLErr err = band->RasterIO(GF_Read, 0, 0, width, height, buffer,
                m_width, m_height, GDT_Byte, 0, 0, NULL);
        if(err == CE_Failure)
            puts("AHHHH SHIT");
        dataset->Release();
        return bgfx::createTexture2D(m_width, m_height, 0, bgfx::TextureFormat::R8
                ,BGFX_TEXTURE_U_CLAMP, bgfx::makeRef(buffer, sizeof(GByte) * m_width * m_height));
    }

    void init(int _argc, char **_argv) BX_OVERRIDE
    {
        Args args(_argc, _argv);

        m_width = 1280;
        m_height = 720;
        m_debug = BGFX_DEBUG_TEXT;
        m_reset = BGFX_RESET_VSYNC;

        bgfx::init(args.m_type, args.m_pciId);
        bgfx::reset(m_width, m_height, m_reset);

        bgfx::setDebug(m_debug);

        // Set view 0 clear state.
        bgfx::setViewClear(0
                        , BGFX_CLEAR_COLOR|BGFX_CLEAR_DEPTH
                        , 0x303030ff
                        , 1.0f
                        , 0
                        );

        // Get renderer capabilities info.
        const bgfx::Caps* caps = bgfx::getCaps();
        m_instancingSupported = 0 != (caps->supported & BGFX_CAPS_INSTANCING);

        TexCoordVertex::init();
        calcTangents(textureVertices
                    , BX_COUNTOF(textureVertices)
                    , TexCoordVertex::decl
                    , indices
                    , BX_COUNTOF(indices)
                    );
        m_vbh = bgfx::createVertexBuffer(
                                    bgfx::makeRef(textureVertices, sizeof(textureVertices))
                                  , TexCoordVertex::decl);
        m_ibh = bgfx::createIndexBuffer(bgfx::makeRef(indices, sizeof(indices)));

        s_texColor = bgfx::createUniform("tex", bgfx::UniformType::Int1);
        m_program = loadProgram("vertex_shader", "frag_shader");

        m_textureColor = loadImage("/home/leah/Pictures/colorhillshade_mola128_mola64_merge_90Nto90S_Simp_clon0.tif");
    }

    virtual int shutdown() BX_OVERRIDE
    {
        bgfx::destroyIndexBuffer(m_ibh);
        bgfx::destroyVertexBuffer(m_vbh);
        bgfx::destroyProgram(m_program);
        bgfx::destroyTexture(m_textureColor);
        bgfx::destroyUniform(s_texColor);
        bgfx::shutdown();
        return 0;
    }

    bool update() BX_OVERRIDE
    {
        if (!entry::processEvents(m_width, m_height, m_debug, m_reset))
        {
            bgfx::setViewRect(0, 0, 0, m_width, m_height);
            bgfx::touch(0);

            bgfx::setVertexBuffer(m_vbh);
            bgfx::setIndexBuffer(m_ibh);

            bgfx::setTexture(0, s_texColor, m_textureColor);
            bgfx::setState(0
                    | BGFX_STATE_RGB_WRITE
                    | BGFX_STATE_ALPHA_WRITE
                    | BGFX_STATE_DEPTH_WRITE
                    | BGFX_STATE_DEPTH_TEST_LESS
                    | BGFX_STATE_MSAA
                    );
            bgfx::submit(0, m_program);
        }
        bgfx::frame();
        return true;
    }
};
