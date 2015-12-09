#include "headers/rastershader.h"

char* raster_vertex_shader_src =
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

char* raster_fragment_shader_src =
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
        glGetShaderInfoLog(shader_id, sizeof(message), 0, &message[0]);
        printf("glCompileShader Error: %s\n", message);
        exit(1);
    }

    return shader_id;
}

