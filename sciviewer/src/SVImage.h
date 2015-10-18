typedef struct {
	int Width, Height; // Image dimensions
	int Format; // OpenGL pixel format
	int BytesPerPixel; // Number of bytes per pixel
	unsigned char *Data; // Pointer to pixel data
} SVImage;