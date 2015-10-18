#define GLFW_INCLUDE_ES2 1
#define GLFW_DLL 1

#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include <ui_images.c>

GLFWwindow* window;


typedef struct {
  float position[3];
  float color[4];
} Vertex;


const Vertex Vertices[] = {
  {{1, -1, 0}, {0, 1, 0, 1}},
  {{1, 1, 0}, {0, 1, 0, 1}},
  {{-1, 1, 0}, {0, 1, 0, 1}},
  {{-1, -1, 0}, {0, 1, 0, 1}}
};
const GLubyte Indices[] = {
  0, 1, 2,
  2, 3, 0
};

typedef struct {
  float position[3];
} uiVertex;

const GLubyte uiIndices[] = {
  0, 1, 2,
  2, 3, 0
};
 static const GLfloat squareTexCoords[] = {
        1, 1,
        1, 0,
        0, 0,
        0, 1
 };
 
 static const GLfloat squareTexCoords90ccw[] = {
        0, 1,
		1, 1,
        1, 0,
        0, 0
 };
  static const GLfloat squareTexCoords180[] = {
        0, 0,
		0, 1,
		1, 1,
        1, 0
 };
  static const GLfloat squareTexCoords90cw[] = {
        1, 0,
		0, 0,
		0, 1,
		1, 1
 };
 
char* vector_vertex_shader_src =
  "attribute vec4 Position;\n"
  "attribute vec4 SourceColor;\n"
  "\n"
  "varying vec4 DestinationColor;\n"
  "\n"
  "void main(void) {\n"
  "    DestinationColor = SourceColor;\n"
  "    gl_Position = Position;\n"
  "}\n";

char* vector_fragment_shader_src =
  "varying lowp vec4 DestinationColor;\n"
  "\n"
  "void main(void) {\n"
  "    gl_FragColor = DestinationColor;\n"
  "}\n";

  
  char* ui_vertex_shader_src =
  "attribute vec4 uiPosition;\n"
  "attribute vec2 uiSourceColor;\n"
  "\n"
  "varying vec2 uiDestinationColor;\n"
  "\n"
  "void main(void) {\n"
  "    gl_Position = uiPosition;\n"
  "    uiDestinationColor = uiSourceColor ;\n" 
  "}\n";

  char* ui_fragment_shader_src =
  "precision highp float;\n"
  "uniform sampler2D texture;\n"
  "varying lowp vec2 uiDestinationColor;\n"
  "\n"
  "void main(void) {\n"
  "    gl_FragColor = texture2D(texture, uiDestinationColor);\n"
  "}\n";

GLint simple_shader(GLint shader_type, char* shader_src) {

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


int simple_program(char* vertex_shader_src, char* fragment_shader_src) {

  GLint link_success = 0;

  GLint program_id = glCreateProgram();
  GLint vertex_shader = simple_shader(GL_VERTEX_SHADER, vertex_shader_src);
  GLint fragment_shader = simple_shader(GL_FRAGMENT_SHADER, fragment_shader_src);

  glAttachShader(program_id, vertex_shader);
  glAttachShader(program_id, fragment_shader);

  glLinkProgram(program_id);

  glGetProgramiv(program_id, GL_LINK_STATUS, &link_success);

  if (link_success == GL_FALSE) {
    GLchar message[256];
    glGetProgramInfoLog(program_id, sizeof(message), 0, &message[0]);
    printf("glLinkProgram Error: %s\n", message);
    exit(1);
  }

  return program_id;
}


static void error_callback(int error, const char* description) {
  fputs(description, stderr);
}

void build_texture(GLuint texture_buffer, gimage texture_image, GLint color_slot){
	//glGenTextures(1,&ui_Texture_panarrow_hover);
	glGenTextures(1, &texture_buffer);
	//glBindTexture(GL_TEXTURE_2D, ui_Texture_panarrow_hover);
	glBindTexture(GL_TEXTURE_2D, texture_buffer);
	glUniform1i(color_slot, 0);
	glTexImage2D(
		GL_TEXTURE_2D,         // target
		0,                     // level, 0 = base, no mipmap,
		GL_RGBA,
		//panarrow_hover_uiimage.width,     // width
		texture_image.width,
		//panarrow_hover_uiimage.height,    // height
		texture_image.height,
		0,                     // border, always 0 in OpenGL ES
		GL_RGBA,
		GL_UNSIGNED_BYTE,      // type
		//panarrow_hover_uiimage.pixel_data
		texture_image.pixel_data
	);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
}

int main(void) {
  //GL vector variables
  GLint program_id, position_slot, color_slot;
  GLuint vertex_buffer;
  GLuint index_buffer;

  //-----------------------------------------------------
  //GL UI VARIABLES
  //-----------------------------------------------------
  GLint ui_program_id, ui_position_slot, ui_color_slot;
  //Pan UI Variables
  GLuint ui_vertex_buffer_up;
  GLuint ui_vertex_buffer_right; 
  GLuint ui_vertex_buffer_down;
  GLuint ui_vertex_buffer_left;

  GLuint ui_Texture_panarrow_up;
  GLuint ui_Texture_panarrow_hover;
  GLuint ui_Texture_panarrow_down;

  //zoomin UI Variables
  GLuint ui_vertex_buffer_zoomin;
  
  GLuint ui_Texture_zoomin_up;
  GLuint ui_Texture_zoomin_hover;
  GLuint ui_Texture_zoomin_down;

  //zoomin UI Variables  
  GLuint ui_vertex_buffer_zoomout;
  
  GLuint ui_Texture_zoomout_up;
  GLuint ui_Texture_zoomout_hover;
  GLuint ui_Texture_zoomout_down;

  //openfile UI Variables 
  GLuint ui_vertex_buffer_openfile;
  
  GLuint ui_Texture_openfile_up;
  GLuint ui_Texture_openfile_hover;
  GLuint ui_Texture_openfile_down;
  
  GLuint ui_index_buffer;
  //-----------------------------------------------------
  glfwSetErrorCallback(error_callback);

  // Initialize GLFW library
  if (!glfwInit())
    return -1;

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
  window = glfwCreateWindow(mode->width, mode->height,"SciViewer",NULL,NULL);
  
  int* winwidth = malloc (sizeof(int));
  int* winheight = malloc (sizeof(int));
  glfwGetWindowSize(window,winwidth,winheight);
 
  if (!window) {
    glfwTerminate();
    printf("glfwCreateWindow Error\n");
    exit(1);
  }

  glfwMakeContextCurrent(window);

  //MAIN
  //----------------------------------------------------------
 
  program_id = simple_program(vector_vertex_shader_src, vector_fragment_shader_src);
  glUseProgram(program_id);

  position_slot = glGetAttribLocation(program_id, "Position");
  color_slot = glGetAttribLocation(program_id, "SourceColor");

  glEnableVertexAttribArray(position_slot);
  glEnableVertexAttribArray(color_slot);

  // Create Buffer
  glGenBuffers(1, &vertex_buffer);

  // Map GL_ARRAY_BUFFER to this buffer
  glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

  // Send the data
  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);

  //INDEX
  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
  
  //----------------------------------------------------------
  //UI
  ui_program_id = simple_program(ui_vertex_shader_src, ui_fragment_shader_src);
  glUseProgram(ui_program_id);

  ui_position_slot = glGetAttribLocation(ui_program_id, "uiPosition");
  ui_color_slot = glGetAttribLocation(ui_program_id, "uiSourceColor"); //texture
  glEnableVertexAttribArray(ui_position_slot);
  glEnableVertexAttribArray(ui_color_slot);
  
  //Up
  float up_tlx=0.0;
  float up_tly=0.0;
  float up_brx=0.0;
  float up_bry=0.0;	
  uiVertex UIVertices_up[4];
  
  //Right
  float right_tlx=0.0;
  float right_tly=0.0;
  float right_brx=0.0;
  float right_bry=0.0;	
  uiVertex UIVertices_right[4];
  
  //Down
  float down_tlx=0.0;
  float down_tly=0.0;
  float down_brx=0.0;
  float down_bry=0.0;	
  uiVertex UIVertices_down[4];
  
  //Left
  float left_tlx=0.0;
  float left_tly=0.0;
  float left_brx=0.0;
  float left_bry=0.0;	
  uiVertex UIVertices_left[4];
  
  //Zoom In
  float zoomin_tlx=0.0;
  float zoomin_tly=0.0;
  float zoomin_brx=0.0;
  float zoomin_bry=0.0;	
  uiVertex UIVertices_zoomin[4];
  
  //Zoom Out
  float zoomout_tlx=0.0;
  float zoomout_tly=0.0;
  float zoomout_brx=0.0;
  float zoomout_bry=0.0;	
  uiVertex UIVertices_zoomout[4];
  
  //Open File
  float openfile_tlx=0.0;
  float openfile_tly=0.0;
  float openfile_brx=0.0;
  float openfile_bry=0.0;	
  uiVertex UIVertices_openfile[4];
  
  //UP
  glGenBuffers(1, &ui_vertex_buffer_up);
  glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_up);
  glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_up), UIVertices_up, GL_STATIC_DRAW);

  //RIGHT
  glGenBuffers(1, &ui_vertex_buffer_right);
  glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_right);
  glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_right), UIVertices_right, GL_STATIC_DRAW);

  //DOWN
  glGenBuffers(1, &ui_vertex_buffer_down);
  glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_down);
  glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_down), UIVertices_down, GL_STATIC_DRAW);
  
  //LEFT
  glGenBuffers(1, &ui_vertex_buffer_left);
  glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_left);
  glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_left), UIVertices_left, GL_STATIC_DRAW);
  
  //ZOOM IN
  glGenBuffers(1, &ui_vertex_buffer_zoomin);
  glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_zoomin);
  glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_zoomin), UIVertices_zoomin, GL_STATIC_DRAW);
  
  //ZOOM OUT
  glGenBuffers(1, &ui_vertex_buffer_zoomout);
  glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_zoomout);
  glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_zoomout), UIVertices_zoomout, GL_STATIC_DRAW);
  
  //OPEN FILE
  glGenBuffers(1, &ui_vertex_buffer_openfile);
  glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_openfile);
  glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_openfile), UIVertices_openfile, GL_STATIC_DRAW);

  //UI Index
  glGenBuffers(1, &ui_index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ui_index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uiIndices), uiIndices, GL_STATIC_DRAW);
  
  //normal
  glBindBuffer(GL_ARRAY_BUFFER, squareTexCoords);
  glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords), squareTexCoords, GL_STATIC_DRAW);

  //90 clockwise
  glBindBuffer(GL_ARRAY_BUFFER, squareTexCoords90cw);
  glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords90cw), squareTexCoords90cw, GL_STATIC_DRAW);

  //180
  glBindBuffer(GL_ARRAY_BUFFER, squareTexCoords180);
  glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords180), squareTexCoords180, GL_STATIC_DRAW);  

  //90 counter clockwise
  glBindBuffer(GL_ARRAY_BUFFER, squareTexCoords90ccw);
  glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords90ccw), squareTexCoords90ccw, GL_STATIC_DRAW);  

  //----------------------------------------------------------
  //textures
  //----------------------------------------------------------
  
  //PAN ARROW UP
  build_texture(ui_Texture_panarrow_hover, panarrow_hover_uiimage, ui_color_slot);
  /*glGenTextures(1,&ui_Texture_panarrow_up);
  glBindTexture(GL_TEXTURE_2D, ui_Texture_panarrow_up);
  glUniform1i(ui_color_slot, 0);
	
  glTexImage2D(
    GL_TEXTURE_2D,         // target
    0,                     // level, 0 = base, no mipmap,
    GL_RGBA,
    panarrow_up_uiimage.width,     // width
    panarrow_up_uiimage.height,    // height

    0,                     // border, always 0 in OpenGL ES
	GL_RGBA,
    GL_UNSIGNED_BYTE,      // type
	panarrow_up_uiimage.pixel_data
  );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	//PAN ARROW HOVER
   glGenTextures(1,&ui_Texture_panarrow_hover);
  glBindTexture(GL_TEXTURE_2D, ui_Texture_panarrow_hover);
  glUniform1i(ui_color_slot, 0);
	
  glTexImage2D(
    GL_TEXTURE_2D,         // target
    0,                     // level, 0 = base, no mipmap,
    GL_RGBA,
    panarrow_hover_uiimage.width,     // width
    panarrow_hover_uiimage.height,    // height

    0,                     // border, always 0 in OpenGL ES
	GL_RGBA,
    GL_UNSIGNED_BYTE,      // type
	panarrow_hover_uiimage.pixel_data
  );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

	//PAN ARROW DOWN
   glGenTextures(1,&ui_Texture_panarrow_down);
  glBindTexture(GL_TEXTURE_2D, ui_Texture_panarrow_down);
  glUniform1i(ui_color_slot, 0);
	
  glTexImage2D(
    GL_TEXTURE_2D,         // target
    0,                     // level, 0 = base, no mipmap,
    GL_RGBA,
    panarrow_down_uiimage.width,     // width
    panarrow_down_uiimage.height,    // height

    0,                     // border, always 0 in OpenGL ES
	GL_RGBA,
    GL_UNSIGNED_BYTE,      // type
	panarrow_down_uiimage.pixel_data
  );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	//ZOOM IN UP 
	
   glGenTextures(1,&ui_Texture_zoomin_up);
  glBindTexture(GL_TEXTURE_2D, ui_Texture_zoomin_up);
  glUniform1i(ui_color_slot, 0);
	
  glTexImage2D(
    GL_TEXTURE_2D,         // target
    0,                     // level, 0 = base, no mipmap,
    GL_RGBA,
    zoomin_up_uiimage.width,     // width
    zoomin_up_uiimage.height,    // height

    0,                     // border, always 0 in OpenGL ES
	GL_RGBA,
    GL_UNSIGNED_BYTE,      // type
	zoomin_up_uiimage.pixel_data
  );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  	//ZOOM IN HOVER
	
   glGenTextures(1,&ui_Texture_zoomin_hover);
  glBindTexture(GL_TEXTURE_2D, ui_Texture_zoomin_hover);
  glUniform1i(ui_color_slot, 0);
	
  glTexImage2D(
    GL_TEXTURE_2D,         // target
    0,                     // level, 0 = base, no mipmap,
    GL_RGBA,
    zoomin_hover_uiimage.width,     // width
    zoomin_hover_uiimage.height,    // height

    0,                     // border, always 0 in OpenGL ES
	GL_RGBA,
    GL_UNSIGNED_BYTE,      // type
	zoomin_hover_uiimage.pixel_data
  );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	//ZOOM IN DOWN 
	
   glGenTextures(1,&ui_Texture_zoomin_down);
  glBindTexture(GL_TEXTURE_2D, ui_Texture_zoomin_down);
  glUniform1i(ui_color_slot, 0);
	
  glTexImage2D(
    GL_TEXTURE_2D,         // target
    0,                     // level, 0 = base, no mipmap,
    GL_RGBA,
    zoomin_down_uiimage.width,     // width
    zoomin_down_uiimage.height,    // height

    0,                     // border, always 0 in OpenGL ES
	GL_RGBA,
    GL_UNSIGNED_BYTE,      // type
	zoomin_down_uiimage.pixel_data
  );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	//ZOOM OUT UP 
  glGenTextures(1,&ui_Texture_zoomout_up);
  glBindTexture(GL_TEXTURE_2D, ui_Texture_zoomout_up);
  glUniform1i(ui_color_slot, 0);
	
  glTexImage2D(
    GL_TEXTURE_2D,         // target
    0,                     // level, 0 = base, no mipmap,
    GL_RGBA,
    zoomout_up_uiimage.width,     // width
    zoomout_up_uiimage.height,    // height

    0,                     // border, always 0 in OpenGL ES
	GL_RGBA,
    GL_UNSIGNED_BYTE,      // type
	zoomout_up_uiimage.pixel_data
  );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
  	//ZOOM OUT HOVER
  glGenTextures(1,&ui_Texture_zoomout_hover);
  glBindTexture(GL_TEXTURE_2D, ui_Texture_zoomout_hover);
  glUniform1i(ui_color_slot, 0);
	
  glTexImage2D(
    GL_TEXTURE_2D,         // target
    0,                     // level, 0 = base, no mipmap,
    GL_RGBA,
    zoomout_hover_uiimage.width,     // width
    zoomout_hover_uiimage.height,    // height

    0,                     // border, always 0 in OpenGL ES
	GL_RGBA,
    GL_UNSIGNED_BYTE,      // type
	zoomout_hover_uiimage.pixel_data
  );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	//ZOOM OUT DOWN 	
  glGenTextures(1,&ui_Texture_zoomout_down);
  glBindTexture(GL_TEXTURE_2D, ui_Texture_zoomout_down);
  glUniform1i(ui_color_slot, 0);
	
  glTexImage2D(
    GL_TEXTURE_2D,         // target
    0,                     // level, 0 = base, no mipmap,
    GL_RGBA,
    zoomout_down_uiimage.width,     // width
    zoomout_down_uiimage.height,    // height

    0,                     // border, always 0 in OpenGL ES
	GL_RGBA,
    GL_UNSIGNED_BYTE,      // type
	zoomout_down_uiimage.pixel_data
  );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	//OPEN FILE UP 
	  glGenTextures(1,&ui_Texture_openfile_up);
  glBindTexture(GL_TEXTURE_2D, ui_Texture_openfile_up);
  glUniform1i(ui_color_slot, 0);
	
  glTexImage2D(
    GL_TEXTURE_2D,         // target
    0,                     // level, 0 = base, no mipmap,
    GL_RGBA,
    openfile_up_uiimage.width,     // width
    openfile_up_uiimage.height,    // height

    0,                     // border, always 0 in OpenGL ES
	GL_RGBA,
    GL_UNSIGNED_BYTE,      // type
	openfile_up_uiimage.pixel_data
  );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
  	//OPEN FILE HOVER
  glGenTextures(1,&ui_Texture_openfile_hover);
  glBindTexture(GL_TEXTURE_2D, ui_Texture_openfile_hover);
  glUniform1i(ui_color_slot, 0);
	
  glTexImage2D(
    GL_TEXTURE_2D,         // target
    0,                     // level, 0 = base, no mipmap,
    GL_RGBA,
    openfile_hover_uiimage.width,     // width
    openfile_hover_uiimage.height,    // height

    0,                     // border, always 0 in OpenGL ES
	GL_RGBA,
    GL_UNSIGNED_BYTE,      // type
	openfile_hover_uiimage.pixel_data
  );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	//OPEN FILE DOWN 	
	 glGenTextures(1,&ui_Texture_openfile_down);
  glBindTexture(GL_TEXTURE_2D, ui_Texture_openfile_down);
  glUniform1i(ui_color_slot, 0);
	
  glTexImage2D(
    GL_TEXTURE_2D,         // target
    0,                     // level, 0 = base, no mipmap,
    GL_RGBA,
    openfile_down_uiimage.width,     // width
    openfile_down_uiimage.height,    // height

    0,                     // border, always 0 in OpenGL ES
	GL_RGBA,
    GL_UNSIGNED_BYTE,      // type
	openfile_down_uiimage.pixel_data
  );

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	*/
	
   // Repeat
  while (!glfwWindowShouldClose(window)) {
	//Get current window size

    //glClearColor(0, 104.0/255.0, 55.0/255.0, 1.0);
	glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
	
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 
	
	glfwGetWindowSize(window,winwidth,winheight);
    glViewport(0, 0, *winwidth, *winheight);
	//glViewport(0, 0, glfwi->Width, glfwi->Height);

	//===========================================================================
	glUseProgram(program_id);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    glVertexAttribPointer(position_slot,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          0);

    glVertexAttribPointer(color_slot,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(Vertex),
                          (GLvoid*) (sizeof(float) * 3));
						  
    glDrawElements(GL_TRIANGLES,
                   sizeof(Indices) / sizeof(GLubyte),
                   GL_UNSIGNED_BYTE, 0);
				   
  
  //------------------------------------------------------------------------------------  
  //UI
  //------------------------------------------------------------------------------------
  glUseProgram(ui_program_id);

  //-------------------
  //Up Arrow
  //-------------------
    up_tlx=(((*winwidth/2.0)-(30+30+15))/(*winwidth/2.0));
    up_tly=(((*winheight/2.0)-(15))/(*winheight/2.0));
    up_brx=(((*winwidth/2.0)-(30+15))/(*winwidth/2.0));
    up_bry=(((*winheight/2.0)-(30+15))/(*winheight/2.0));	
  
	uiVertex UIVertices_up[] = {
	  {up_brx, up_bry, 1}, //Botom right
	  {up_brx, up_tly, 1},    //Top right
	  {up_tlx, up_tly, 1}, //Top Left
	  {up_tlx, up_bry, 1} //Bottom Left
	};	

	glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_up);
    glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_up), UIVertices_up, GL_STATIC_DRAW);

    glEnableVertexAttribArray(ui_position_slot);
    glVertexAttribPointer(ui_position_slot,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(uiVertex),
                          (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, squareTexCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords), squareTexCoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(ui_color_slot);
    glVertexAttribPointer(ui_color_slot,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(GL_FLOAT)*2,
                          (void*)0);

			//Texture
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ui_Texture_panarrow_up);
	glUniform1i(ui_color_slot, 0);

    glDrawElements(GL_TRIANGLES,
                   sizeof(uiIndices) / sizeof(GLubyte),
                   GL_UNSIGNED_BYTE, 0);
				   
  //-------------------
  //Right Arrow
  //-------------------

	  right_tlx=(((*winwidth/2.0)-(30+15))/(*winwidth/2.0));
	  right_tly=(((*winheight/2.0)-(30+15))/(*winheight/2.0));
	  right_brx=(((*winwidth/2.0)-(15))/(*winwidth/2.0));
	  right_bry=(((*winheight/2.0)-(30+30+15))/(*winheight/2.0));	

	  uiVertex UIVertices_right[] = {
	  {right_brx, right_bry, 1}, //Botom right
	  {right_brx, right_tly, 1},    //Top right
	  {right_tlx, right_tly, 1}, //Top Left
	  {right_tlx, right_bry, 1} //Bottom Left
	  };
	
      glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_right);
      glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_right), UIVertices_right, GL_STATIC_DRAW);

	glEnableVertexAttribArray(ui_position_slot);
    glVertexAttribPointer(ui_position_slot,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(uiVertex),
                          (void*)0);


	glBindBuffer(GL_ARRAY_BUFFER, squareTexCoords90cw);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords90cw), squareTexCoords90cw, GL_STATIC_DRAW);

	glEnableVertexAttribArray(ui_color_slot);
    glVertexAttribPointer(ui_color_slot,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(GL_FLOAT)*2,
                          (void*)0);

			//Texture
	glActiveTexture(GL_TEXTURE0);
    //glBindTexture(GL_TEXTURE_2D, ui_Texture_panarrow_up);
	glBindTexture(GL_TEXTURE_2D, ui_Texture_panarrow_hover);
	glUniform1i(ui_color_slot, 0);


       glDrawElements(GL_TRIANGLES,
                   sizeof(uiIndices) / sizeof(GLubyte),
                   GL_UNSIGNED_BYTE, 0);
	   //-------------------

  //-------------------
  //Down Arrow
  //-------------------	   	
	  down_tlx=(((*winwidth/2.0)-(30+30+15))/(*winwidth/2.0));
	  down_tly=(((*winheight/2.0)-(30+30+15))/(*winheight/2.0));
	  down_brx=(((*winwidth/2.0)-(30+15))/(*winwidth/2.0));
	  down_bry=(((*winheight/2.0)-(30+30+30+15))/(*winheight/2.0));	

	  uiVertex UIVertices_down[] = {
	  {down_brx, down_bry, 1}, //Botom right
	  {down_brx, down_tly, 1},    //Top right
	  {down_tlx, down_tly, 1}, //Top Left
	  {down_tlx, down_bry, 1} //Bottom Left
	  };
		
	
      glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_down);
      glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_down), UIVertices_down, GL_STATIC_DRAW);

	glEnableVertexAttribArray(ui_position_slot);
    glVertexAttribPointer(ui_position_slot,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(uiVertex),
                          (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, squareTexCoords180);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords180), squareTexCoords180, GL_STATIC_DRAW);

	glEnableVertexAttribArray(ui_color_slot);
    glVertexAttribPointer(ui_color_slot,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(GL_FLOAT)*2,
                          (void*)0);

			//Texture
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ui_Texture_panarrow_down);
	glUniform1i(ui_color_slot, 0);


       glDrawElements(GL_TRIANGLES,
                   sizeof(Indices) / sizeof(GLubyte),
                   GL_UNSIGNED_BYTE, 0);
  //-------------------

  //-------------------
  //Left Arrow
  //-------------------	   

	  left_tlx=(((*winwidth/2.0)-(30+30+30+15))/(*winwidth/2.0));
	  left_tly=(((*winheight/2.0)-(30+15))/(*winheight/2.0));
	  left_brx=(((*winwidth/2.0)-(30+30+15))/(*winwidth/2.0));
	  left_bry=(((*winheight/2.0)-(30+30+15))/(*winheight/2.0));	

	  uiVertex UIVertices_left[] = {
	  {left_brx, left_bry, 1}, //Botom right
	  {left_brx, left_tly, 1},    //Top right
	  {left_tlx, left_tly, 1}, //Top Left
	  {left_tlx, left_bry, 1} //Bottom Left
	  };
	  
		
	
      glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_left);
      glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_left), UIVertices_left, GL_STATIC_DRAW);

	glEnableVertexAttribArray(ui_position_slot);
    glVertexAttribPointer(ui_position_slot,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(uiVertex),
                          (void*)0);
	
	glBindBuffer(GL_ARRAY_BUFFER, squareTexCoords90ccw);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords90ccw), squareTexCoords90ccw, GL_STATIC_DRAW);

	glEnableVertexAttribArray(ui_color_slot);
    glVertexAttribPointer(ui_color_slot,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(GL_FLOAT)*2,
                          (void*)0);

			//Texture
	glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, ui_Texture_panarrow_up);
	glUniform1i(ui_color_slot, 0);

       glDrawElements(GL_TRIANGLES,
                   sizeof(Indices) / sizeof(GLubyte),
                   GL_UNSIGNED_BYTE, 0);
  //-------------------


  //-------------------
  //Zoom In
  //-------------------	 
	  zoomin_tlx=(((*winwidth/2.0)-(30+30+15))/(*winwidth/2.0));
	  zoomin_tly=(((*winheight/2.0)-(30+30+30+15+15))/(*winheight/2.0));
	  zoomin_brx=(((*winwidth/2.0)-(30+15))/(*winwidth/2.0));
	  zoomin_bry=(((*winheight/2.0)-(30+30+30+30+15+15))/(*winheight/2.0));	

	  uiVertex UIVertices_zoomin[] = {
	  {zoomin_brx, zoomin_bry, 1}, //Botom right
	  {zoomin_brx, zoomin_tly, 1},    //Top right
	  {zoomin_tlx, zoomin_tly, 1}, //Top Left
	  {zoomin_tlx, zoomin_bry, 1} //Bottom Left
	  };
	
      glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_zoomin);
      glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_zoomin), UIVertices_zoomin, GL_STATIC_DRAW);
	  
	glEnableVertexAttribArray(ui_position_slot);
    glVertexAttribPointer(ui_position_slot,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(uiVertex),
                          (void*)0);


	glBindBuffer(GL_ARRAY_BUFFER, squareTexCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords), squareTexCoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(ui_color_slot);
    glVertexAttribPointer(ui_color_slot,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(GL_FLOAT)*2,
                          (void*)0);

			//Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ui_Texture_zoomin_down);
	
	glUniform1i(ui_color_slot, 0);
       glDrawElements(GL_TRIANGLES,
                   sizeof(Indices) / sizeof(GLubyte),
                   GL_UNSIGNED_BYTE, 0);
  //-------------------

  //-------------------
  //Zoom out
  //-------------------	 
	  zoomout_tlx=(((*winwidth/2.0)-(30+30+15))/(*winwidth/2.0));
	  zoomout_tly=(((*winheight/2.0)-(30+30+30+30+15+15+5))/(*winheight/2.0));
	  zoomout_brx=(((*winwidth/2.0)-(30+15))/(*winwidth/2.0));
	  zoomout_bry=(((*winheight/2.0)-(30+30+30+30+30+15+15+5))/(*winheight/2.0));	

	  uiVertex UIVertices_zoomout[] = {
	  {zoomout_brx, zoomout_bry, 1}, //Botom right
	  {zoomout_brx, zoomout_tly, 1},    //Top right
	  {zoomout_tlx, zoomout_tly, 1}, //Top Left
	  {zoomout_tlx, zoomout_bry, 1} //Bottom Left
	  };
	
      glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_zoomout);
      glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_zoomout), UIVertices_zoomout, GL_STATIC_DRAW);
	  
	glEnableVertexAttribArray(ui_position_slot);
    glVertexAttribPointer(ui_position_slot,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(uiVertex),
                          (void*)0);


	glBindBuffer(GL_ARRAY_BUFFER, squareTexCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords), squareTexCoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(ui_color_slot);
    glVertexAttribPointer(ui_color_slot,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(GL_FLOAT)*2,
                          (void*)0);

			//Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ui_Texture_zoomout_hover);
	
       glDrawElements(GL_TRIANGLES,
                   sizeof(Indices) / sizeof(GLubyte),
                   GL_UNSIGNED_BYTE, 0);
  //-------------------

  //-------------------
  //Open file
  //-------------------	 
	  openfile_tlx=(((*winwidth/2.0)-(30+30+15))/(*winwidth/2.0));
	  openfile_tly=(((*winheight/2.0)-(30+30+30+30+30+15+15+5+15))/(*winheight/2.0));
	  openfile_brx=(((*winwidth/2.0)-(30+15))/(*winwidth/2.0));
	  openfile_bry=(((*winheight/2.0)-(30+30+30+30+30+30+15+15+5+15))/(*winheight/2.0));	

	  uiVertex UIVertices_openfile[] = {
	  {openfile_brx, openfile_bry, 1}, //Botom right
	  {openfile_brx, openfile_tly, 1},    //Top right
	  {openfile_tlx, openfile_tly, 1}, //Top Left
	  {openfile_tlx, openfile_bry, 1} //Bottom Left
	  };
	
      glBindBuffer(GL_ARRAY_BUFFER, ui_vertex_buffer_openfile);
      glBufferData(GL_ARRAY_BUFFER, sizeof(UIVertices_openfile), UIVertices_openfile, GL_STATIC_DRAW);
	  
	glEnableVertexAttribArray(ui_position_slot);
    glVertexAttribPointer(ui_position_slot,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(uiVertex),
                          (void*)0);


	glBindBuffer(GL_ARRAY_BUFFER, squareTexCoords);
    glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords), squareTexCoords, GL_STATIC_DRAW);

	glEnableVertexAttribArray(ui_color_slot);
    glVertexAttribPointer(ui_color_slot,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          sizeof(GL_FLOAT)*2,
                          (void*)0);

			//Texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, ui_Texture_openfile_hover);
	

       glDrawElements(GL_TRIANGLES,
                   sizeof(Indices) / sizeof(GLubyte),
                   GL_UNSIGNED_BYTE, 0);
  //-------------------

  
      glfwSwapBuffers(window);
      glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}