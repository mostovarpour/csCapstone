#include "raster.h"

//TODO move these shaders into GLSL files and load them in that way
const char *vertex_shader = 
"#version 150\n" // declare version 1.5
"in vec2 texCoord;"
"in vec2 position;" // input is a 2d vector (X,Y)
"out vec2 TexCoord;"
"void main()"
"{"
    "TexCoord = texCoord;"
    "gl_Position = vec4(position, 0.0, 1.0);" // convert vec2 to vec4 and set it gl_position
"}";

const char *fragment_shader =
"#version 150\n"
"uniform sampler2D tex[3];"
"in vec2 TexCoord;"
"out vec4 outColor;" // output a vec4 (r,g,b,a)
"void main()"
"{"
"   vec4 tmpr = texture2D(tex[0], TexCoord);"
"   vec4 tmpg = texture2D(tex[1], TexCoord);"
"   vec4 tmpb = texture2D(tex[2], TexCoord);"
"   outColor = vec4(tmpr.r, tmpg.r, tmpb.r, 1.0f);"
"}"
;

// pass in pointers so we can free the memory in the gpu later
void setup_polygons(GLuint *vertex_attribute_obj, GLuint *element_buffer, GLuint *vertex_buffer, GLuint *v_shader, GLuint *f_shader, GLuint *shader_program) 
{
    glewInit();
    // create vertices simple triangle
    float vertices[] = {
        //position     //texture coordinates
        -1.0f,  1.0f,  0.0f, 0.0f, // top left 
         1.0f,  1.0f,  1.0f, 0.0f, // top right
         1.0f, -1.0f,  1.0f, 1.0f, // bottom-right
        -1.0f, -1.0f,  0.0f, 1.0f  // bottom-left
    };

    // create vertex array object for storing shader and attribute data
    glGenVertexArrays(1, vertex_attribute_obj);
    glBindVertexArray(*vertex_attribute_obj);


    // create vertex buffer
    glGenBuffers(1, vertex_buffer); // generate one buffer
    // make this buffer the "active" buffer
    glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);
    // put vertices into the buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // create element buffer
    GLuint elements[] = {
        0, 1, 2,
        2, 3, 0
    };
    glGenBuffers(1, element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements, GL_STATIC_DRAW);
    // create vertex shader
    *v_shader = glCreateShader(GL_VERTEX_SHADER);
    // load shader source into GPU
    glShaderSource(*v_shader, 1, &vertex_shader, NULL);
    // compile the shader
    glCompileShader(*v_shader);
    // check if shader compiled correctly
    GLint status;
    glGetShaderiv(*v_shader, GL_COMPILE_STATUS, &status);
    if(status != GL_TRUE)
    {
        puts("Error compiling vertex shader");
        // get error log from shader
        char buffer[512];
        glGetShaderInfoLog(*v_shader, 512, NULL, buffer);
        printf("%s\n", buffer); 
    }
    // same thing for fragment shader
    *f_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(*f_shader, 1, &fragment_shader, NULL);
    glCompileShader(*f_shader);
    glGetShaderiv(*f_shader, GL_COMPILE_STATUS, &status);

    if(status != GL_TRUE)
    {
        puts("Error compiling fragment shader");
        // get error log from shader
        char buffer[512];
        glGetShaderInfoLog(*f_shader, 512, NULL, buffer);
        printf("%s\n", buffer); 
    }
    // combine shaders into a program
    *shader_program = glCreateProgram();
    glAttachShader(*shader_program, *v_shader);
    glAttachShader(*shader_program, *f_shader);
    /*glBindFragDataLocation(*shader_program, 0, "outColor");*/
    // link the program
    glLinkProgram(*shader_program);
    // tell opengl to use this program
    glUseProgram(*shader_program);

    // get position of the input position in vertex_shader
    GLint posAttribute = glGetAttribLocation(*shader_program, "position"); // always 0 since it's the first and only arg
    // (arg_position, number of values for that arg, type for each component, 
    // should they be normalized?, distance (in bytes) between each position attribute,
    // offset from start of array for first attribute value)
    glVertexAttribPointer(posAttribute, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), 0);
    // enable attribute array
    glEnableVertexAttribArray(posAttribute);
    GLint texAttrib = glGetAttribLocation(*shader_program, "texCoord");
    glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(texAttrib);
}

void setup_texture(GLFWwindow *window, GDALImage *image, GLuint *tex_buffer, GLuint shader)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    sample(image, width, height);
    // if the image isn't ready then there's no point
    if(!image->ready_to_upload)
        return;
    /*******************
     *TEXTURE STUFF YAY*
     *******************/
    // delete old textures
    glDeleteTextures(image->band_count, tex_buffer);
    // generate new textures, one for each band
    glGenTextures(image->band_count, tex_buffer);

    // loop through textures and apply them to the shader
    int i;
    for (i = 0; i < image->band_count; i++)
    {
        glBindTexture(GL_TEXTURE_2D, tex_buffer[i]); // set this texture as the current texture
        // load in texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, image->bands[i]);
        // generate mip map
        glGenerateMipmap(GL_TEXTURE_2D);
        // set gpu sampling parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    }
    char shader_location[10];
    // This needs to come after loading the textures, I don't fully understand why...
    for (i = 0; i < image->band_count; i++)
    {
        sprintf_s(shader_location, sizeof(shader_location), "tex[%d]", i);
        GLint tex_loc;
        tex_loc = glGetUniformLocation(shader, shader_location);
        glUniform1i(tex_loc, i);
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, tex_buffer[i]);
    }
    image->ready_to_upload = false;
}

void check_texture(GDALImage *image, GLFWwindow *window, GLuint *tex)
{
    // check if we're still sampling
    /*
     *int i;
     *for(i = 0; i < image->band_count; i++)
     *{
     *    if(image->is_sampling[i])
     *        return;
     *}
     */
    // no sampling, time to read it again
}

