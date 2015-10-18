#define GLFW_INCLUDE_ES2 1
#define GLFW_DLL 1

#include <GLES2/gl2.h>
#include <GLFW/glfw3.h>

#include <stdlib.h>
#include <stdio.h>

#include <ui_images.c>

GLFWwindow* window;

typedef struct {
	int Width, Height; // Image dimensions
	int Format; // OpenGL pixel format
	int BytesPerPixel; // Number of bytes per pixel
	unsigned char *Data; // Pointer to pixel data
} SVImage;

typedef struct {
  float position[3];
  float color[4];
} Vertex;

typedef struct {
  float position[3];
} rasterVertex;

const Vertex Vertices[] = {
  {{1, -1, 0}, {1, 0, 0, 1}},
  {{1, 1, 0}, {0, 1, 0, 1}},
  {{-1, 1, 0}, {0, 0, 1, 1}},
  {{-1, -1, 0}, {0, 0, 0, 1}}
};

const rasterVertex RasterVertices[] = {
  {1, -1, 0},
  {1, 1, 0},
  {-1, 1, 0},
  {-1, -1, 0}
};

const GLubyte Indices[] = {
  0, 1, 2,
  2, 3, 0
};

static const GLfloat squareTexCoords[] = {
        1, 1,
        1, 0,
        0, 0,
        0, 1
};


typedef struct {
  char* name;
  GLuint vertexBuffer;
  Vertex vertices[4];
} vectorObject;

typedef struct {
  char* name;
  GLint program_id;
  GLint positionSlot;
  GLint colorSlot;
  int size;
  vectorObject* vectorObjects;
} vectorGroup;


typedef struct {
  char* name;
  GLuint vertexBuffer;
  //GLuint texture;
  GLuint textureBuffer;
  GLuint textureCoordsBuffer;
  const GLfloat* textureCoords;
  rasterVertex vertices[4];
  SVImage* textureImage;
} rasterObject;

typedef struct {
  char* name;
  GLint program_id;
  GLint positionSlot;
  GLint colorSlot;
  int size;
  rasterObject* rasterObjects;
} rasterGroup;

char* vertex_shader_src =
  "attribute vec4 Position;\n"
  "attribute vec4 SourceColor;\n"
  "\n"
  "varying vec4 DestinationColor;\n"
  "\n"
  "void main(void) {\n"
  "    DestinationColor = SourceColor;\n"
  "    gl_Position = Position;\n"
  "}\n";


char* fragment_shader_src =
  "varying lowp vec4 DestinationColor;\n"
  "\n"
  "void main(void) {\n"
  "    gl_FragColor = DestinationColor;\n"
  "}\n";

char* raster_vertex_shader_src =
  "attribute vec4 Position1;\n"
  "attribute vec2 SourceColor1;\n"
  "\n"
  "varying vec2 DestinationColor1;\n"
  "\n"
  "void main(void) {\n"
  "    gl_Position = Position1;\n"
  "    DestinationColor1 = SourceColor1 ;\n" 
  "}\n";

char* raster_fragment_shader_src =
  "precision highp float;\n"
  "uniform sampler2D texture;\n"
  "varying lowp vec2 DestinationColor1;\n"
  "\n"
  "void main(void) {\n"
  "    gl_FragColor = texture2D(texture, DestinationColor1);\n"
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

void GLInitVectorGroup(vectorGroup* group){
	glUseProgram((group->program_id));
	printf("Initializing: %s\n",group->name);
	int i;
	for(i=0; i<(group->size);i++){
		printf(" - Initializing: %s\n", group->vectorObjects[i].name);
		// Create Buffer
		glGenBuffers(1, &(group->vectorObjects[i].vertexBuffer));
		// Map GL_ARRAY_BUFFER to this buffer
		glBindBuffer(GL_ARRAY_BUFFER, (group->vectorObjects[i].vertexBuffer));
		// Send the data
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), group->vectorObjects[i].vertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(group->positionSlot);
		glEnableVertexAttribArray(group->colorSlot);
	}
}

void GLInitRasterGroup(rasterGroup* group){
	glUseProgram((group->program_id));
	

	
	printf("Initializing: %s\n",group->name);
	int i;
	for(i=0; i<(group->size);i++){
		printf(" - Initializing: %s\n", group->rasterObjects[i].name);
			
		// Create Buffer
		glGenBuffers(1, &(group->rasterObjects[i].vertexBuffer));
		// Map GL_ARRAY_BUFFER to this buffer
		glBindBuffer(GL_ARRAY_BUFFER, (group->rasterObjects[i].vertexBuffer));
		// Send the data
		glBufferData(GL_ARRAY_BUFFER, sizeof(RasterVertices), group->rasterObjects[i].vertices, GL_STATIC_DRAW);
		
		glEnableVertexAttribArray(group->positionSlot);
		


		//Texture Coords
		glGenBuffers(1, &(group->rasterObjects[i].textureCoordsBuffer));
		glBindBuffer(GL_ARRAY_BUFFER, group->rasterObjects[i].textureCoordsBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords), (group->rasterObjects[i].textureCoords), GL_STATIC_DRAW);

		//Generate Texture		
		glGenTextures(1,&(group->rasterObjects[i].textureBuffer)); // Good
		glBindTexture(GL_TEXTURE_2D, (group->rasterObjects[i].textureBuffer));// Good
		glUniform1i((group->colorSlot), 0); // Good

		glTexImage2D(
			GL_TEXTURE_2D,         // target
			0,                     // level, 0 = base, no mipmap,
			GL_RGBA,
			group->rasterObjects[i].textureImage->Width,     // width
			group->rasterObjects[i].textureImage->Height,    // height

			0,                     // border, always 0 in OpenGL ES
			GL_RGBA,
			GL_UNSIGNED_BYTE,      // type
			(group->rasterObjects[i].textureImage->Data)
		);

		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
		
		glEnableVertexAttribArray(group->colorSlot);
	}
}


void GLRenderVectorGroup(vectorGroup* group){
	glUseProgram((group->program_id));
	printf("Rendering: %s\n",group->name);
	int i;
	for(i=0; i<(group->size);i++){
		//Bind Buffer
		glBindBuffer(GL_ARRAY_BUFFER, (group->vectorObjects[i].vertexBuffer));
		glVertexAttribPointer((group->positionSlot),
							  3,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(Vertex),
							  0);

		glVertexAttribPointer((group->colorSlot),
							  4,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(Vertex),
							  (GLvoid*) (sizeof(float) * 3));

		glDrawElements(GL_TRIANGLES,
					   sizeof(Indices) / sizeof(GLubyte),
					   GL_UNSIGNED_BYTE, 0);
	}
}

void GLRenderRasterGroup(rasterGroup* group){
	glUseProgram((group->program_id));
	printf("Rendering: %s\n",group->name);
	int i;
	for(i=0; i<(group->size);i++){
		glBindBuffer(GL_ARRAY_BUFFER, (group->rasterObjects[i].vertexBuffer)); //ok
		

		glVertexAttribPointer((group->positionSlot),
							  3,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(rasterVertex),
							  (void*)0);
		
		glBindBuffer(GL_ARRAY_BUFFER, group->rasterObjects[i].textureCoordsBuffer);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(squareTexCoords), group->rasterObjects[i].textureCoords, GL_STATIC_DRAW);
		

		glVertexAttribPointer(group->colorSlot,
							  2,
							  GL_FLOAT,
							  GL_FALSE,
							  sizeof(GL_FLOAT)*2,
							  (void*)0);

								
				//Texture
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, group->rasterObjects[i].textureBuffer);
		glUniform1i(group->colorSlot, 0);
		
		glDrawElements(GL_TRIANGLES,
					   sizeof(Indices) / sizeof(GLubyte),
					   GL_UNSIGNED_BYTE, 0);
	}
}

SVImage* wrapGImage(gimage* image){
	SVImage* svimagetemp = malloc(sizeof(SVImage));
	svimagetemp->Width = image->width;
	svimagetemp->Height = image->height;
	svimagetemp->Format = 6408; //GL_RGBA Constant
	svimagetemp->BytesPerPixel = image->bytes_per_pixel;
	svimagetemp->Data = (image->pixel_data);
	return svimagetemp;
}


static void error_callback(int error, const char* description) {
  fputs(description, stderr);
}


rasterObject* createTiles(int numXTiles, int numYTiles){
	printf("Test");
	float tileWidth = (2.0/numXTiles);
	float tileHeight = (2.0/numYTiles);
	int numTiles = (numXTiles * numYTiles);
	rasterObject* tiles = (rasterObject*)(malloc(sizeof(rasterObject) * numTiles));
	int i;
	int j;
	for (i = 0; i<numYTiles; i++){
		for(j = 0; j<numXTiles; j++){
			//sprintf(tiles[(i*numXTiles)+j].name, "Raster %i\0", ((i*numXTiles)+j));
			tiles[(i*numXTiles)+j].vertices[0] = (rasterVertex){ (((j*tileWidth)+tileWidth)-1) , (((i*tileHeight)+tileHeight)-1) , 0};	//bottom right
			tiles[(i*numXTiles)+j].vertices[1] = (rasterVertex){ (((j*tileWidth)+tileWidth)-1) , ((i*tileHeight)-1) , 0};	//top right
			tiles[(i*numXTiles)+j].vertices[2] = (rasterVertex){ ((j*tileWidth)-1) , ((i*tileHeight)-1) , 0};	//top left
			tiles[(i*numXTiles)+j].vertices[3] = (rasterVertex){ ((j*tileWidth)-1) , (((i*tileHeight) + tileHeight)-1) , 0};	//bottom left
			tiles[(i*numXTiles)+j].textureCoords = squareTexCoords;
			tiles[(i*numXTiles)+j].textureImage = (wrapGImage(&openfile_up_uiimage));
		}
		printf("Instantiated row %i\n",i);
	}
	return tiles;
}


int main(void) {

  GLint program_id, position_slot, color_slot;
  GLuint vertex_buffer;
  GLuint index_buffer;

  glfwSetErrorCallback(error_callback);

  // Initialize GLFW library
  if (!glfwInit())
    return -1;

  glfwDefaultWindowHints();
  glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  // Create and open a window
  window = glfwCreateWindow(640,
                            480,
                            "Hello World",
                            NULL,
                            NULL);

  if (!window) {
    glfwTerminate();
    printf("glfwCreateWindow Error\n");
    exit(1);
  }

  glfwMakeContextCurrent(window);




  //------------------------------------------------------------------------------------
  //Vector Objects
  vectorObject firstVector;  
  firstVector.name = "First Vector\0";
  firstVector.vertices[0] =(Vertex){{.5, -.5, 1}, {1, 0, 0, 1}};
  firstVector.vertices[1] =(Vertex){{.5, .5, 1}, {1, 0, 0, 1}};
  firstVector.vertices[2] =(Vertex){{-.5, .5, 1}, {1, 0, 0, 1}};
  firstVector.vertices[3] =(Vertex){{-.5, -.5, 1}, {1, 0, 0, 1}};

  vectorObject secondVector;
  secondVector.name = "Second Vector\0";
  secondVector.vertices[0] =(Vertex){{.25, -.25, 1}, {0, 1, 0, 1}};
  secondVector.vertices[1] =(Vertex){{.25, .25, 1}, {0, 1, 0, 1}};
  secondVector.vertices[2] =(Vertex){{-.25, .25, 1}, {0, 1, 0, 1}};
  secondVector.vertices[3] =(Vertex){{-.25, -.25, 1}, {0, 1, 0, 1}};

  vectorObject thirdVector;
  thirdVector.name = "Third Vector\0";
  thirdVector.vertices[0] =(Vertex){{.15, -.15, 1}, {0, 0, 1, 1}};
  thirdVector.vertices[1] =(Vertex){{.15, .15, 1}, {0, 0, 1, 1}};
  thirdVector.vertices[2] =(Vertex){{-.15, .15, 1}, {0, 0, 1, 1}};
  thirdVector.vertices[3] =(Vertex){{-.15, -.15, 1}, {0, 0, 1, 1}};  
  
  //Vector Group
  vectorGroup firstVectorGroup;
  firstVectorGroup.name = "First Vector Group\0";
  firstVectorGroup.size = 3;
  firstVectorGroup.program_id = simple_program(vertex_shader_src, fragment_shader_src);
	glUseProgram(firstVectorGroup.program_id);
  
  firstVectorGroup.positionSlot = glGetAttribLocation(firstVectorGroup.program_id, "Position");
  firstVectorGroup.colorSlot = glGetAttribLocation(firstVectorGroup.program_id, "SourceColor");

	
	vectorObject* vobjectarray = malloc(firstVectorGroup.size * sizeof(vectorObject));
	vobjectarray[0] = firstVector;
	vobjectarray[1] = secondVector;
	vobjectarray[2] = thirdVector;  
  
  firstVectorGroup.vectorObjects = vobjectarray;
  
  //Init Vector Group (Create Buffers Bind Buffers Send Buffer Data)
  //GLInitVectorGroup(&firstVectorGroup);
  
  //---------------------------------------------------------------------------------------
  //RASTER STUFF
  //  
  
/*
  //Raster Objects
  rasterObject firstRaster;  
  firstRaster.name = "First Raster\0";
  firstRaster.vertices[0] =(rasterVertex){1, -1, 0};			//bottom right
  firstRaster.vertices[1] =(rasterVertex){1, 1, 0};				//top right
  firstRaster.vertices[2] =(rasterVertex){-1, 1, 0};			//top left
  firstRaster.vertices[3] =(rasterVertex){-1, -1, 0};			//bottom left
  firstRaster.textureCoords = squareTexCoords;
  firstRaster.textureImage = (wrapGImage(&openfile_up_uiimage));
*/

  //Raster Group
  rasterGroup firstRasterGroup;
  firstRasterGroup.name = "First Raster Group\0";
  firstRasterGroup.size = 24*12;
  firstRasterGroup.program_id = simple_program(raster_vertex_shader_src, raster_fragment_shader_src);
  firstRasterGroup.positionSlot = glGetAttribLocation(firstRasterGroup.program_id, "Position1");
  firstRasterGroup.colorSlot = glGetAttribLocation(firstRasterGroup.program_id, "SourceColor1");
  glEnableVertexAttribArray(firstRasterGroup.positionSlot); //GO BACK?
  glEnableVertexAttribArray(firstRasterGroup.colorSlot); //Go BACK?
  
    //rasterObject* robjectarray = malloc(firstRasterGroup.size * sizeof(rasterObject));
	//robjectarray[0] = firstRaster;
  
  firstRasterGroup.rasterObjects = (createTiles(24,12));
	
  GLInitRasterGroup(&firstRasterGroup);	
  //---------------------------------------------------------------------------------------
  //Todo Create Function to init index buffer
  glGenBuffers(1, &index_buffer);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);
  //---------------------------------------------------------------------------------------
  
  
  // Repeat
  while (!glfwWindowShouldClose(window)) {

	glClearColor(0, 3, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
	
	glEnable( GL_BLEND );
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA); 

    glViewport(0, 0, 640, 480);

	//GLRenderVectorGroup(&firstVectorGroup);
	GLRenderRasterGroup(&firstRasterGroup);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  exit(EXIT_SUCCESS);
}