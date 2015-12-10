#include "raster.h"

const char* raster_vertex_shader_src = 
"attribute vec4 Position1;"
"attribute vec2 SourceColor1;"
"varying vec2 DestinationColor1;"
"uniform float zoomLevel;"
"uniform float verticalOffset;"
"uniform float horizontalOffset;"
"void main(void) {"
"    gl_Position = vec4((vec2((Position1[0] + horizontalOffset), (Position1[1] + verticalOffset))),Position1[2], (1.0 / zoomLevel));"
"    DestinationColor1 = SourceColor1 ;" 
"}";

const char* raster_fragment_shader_src =
/*"precision highp float;"*/
"uniform sampler2D texture;"
"varying vec2 DestinationColor1;"
"void main(void) {"
"    gl_FragColor = texture2D(texture, vec2(DestinationColor1[0], 1.0-DestinationColor1[1]));"
"}";

GLint simple_shader(GLint shader_type, const char* shader_src) {
    GLint compile_success = 0;
    int shader_id = glCreateShader(shader_type);
    glShaderSource(shader_id, 1, &shader_src, 0);
    glCompileShader(shader_id);
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_success);
    if (compile_success == GL_FALSE) {
        GLchar message[256];
        glGetShaderInfoLog(shader_id, sizeof(message), 0, message);
        printf("glCompileShader Error: %s\n", message);
        exit(1);
    }

    return shader_id;
}

GLint init_vertex_shader() 
{
    return simple_shader(GL_VERTEX_SHADER, raster_vertex_shader_src);
}

GLint init_fragment_shader()
{
    return simple_shader(GL_FRAGMENT_SHADER, raster_fragment_shader_src);
}

GLint init_shaders()
{
    GLint link_success;
    GLint program_id = glCreateProgram();
    GLint vertex_shader = init_vertex_shader();
    GLint fragment_shader = init_fragment_shader();
    glAttachShader(program_id, vertex_shader);
    glAttachShader(program_id, fragment_shader);
    glLinkProgram(program_id);
    glGetProgramiv(program_id, GL_LINK_STATUS, &link_success);
    if(!link_success)
    {
        GLchar message[256];
        glGetProgramInfoLog(program_id, sizeof(GLchar) * 256, NULL, message);
        printf("Link program error: %s\n", message);
        exit(EXIT_FAILURE);
    }
    return program_id;
}

void init_raster(Raster * raster_data, GLint program_id, GDALImage *image)
{
    // some shader magic
    raster_data->position_slot = glGetAttribLocation(program_id, "Position1");
    raster_data->color_slot = glGetAttribLocation(program_id, "SourceColor1");
    raster_data->size = image->num_blocks.x * image->num_blocks.y; // TODO we won't always have block bounds, so this will need to be updated
    glEnableVertexAttribArray(raster_data->position_slot);
    glEnableVertexAttribArray(raster_data->color_slot);
    //TODO stuff with raster objects
}

void render_raster(Raster *raster)
{
    int i;
    glUniform1i(raster->color_slot, 0);
    for(i = 0; i < raster->size; i++)
    {
        //TODO glBindBuffer
        //TODO stuff with raster objects
    }
}
