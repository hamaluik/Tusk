#include <stdio.h>
#include <stdlib.h>
#include "lodepng.h"

int main() {
	const char filename[] = "brick.png";

	unsigned error;
	unsigned char* image;
	unsigned width, height;

	error = lodepng_decode32_file(&image, &width, &height, filename);
	if(error) printf("error %u: %s\n", error, lodepng_error_text(error));

	/*use image here*/

	free(image);
	
	printf("ok\n");

	return 0;
}