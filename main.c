


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <png.h>
#include <setjmp.h>



int main(int argc, char* argv[]) {
	
	
	
	FILE* fi, *fo;
	png_byte sig[8];
	
	
	char* infile = argv[1];
	char* outfile = argv[2];
	
	
	fi = fopen(infile, "rb");
	if(!fi) {
		fprintf(stderr, "Could not open \"%s\".\n", infile);
		return 1;
	}
	
		// file stuff
	fo = fopen(outfile, "wb");
	if(!fo) {
		fprintf(stderr, "Could not open \"%s\".\n", argv[2]);
		return 1;
	}
	
	
	
	
	
	fread(sig, 1, 8, fi);
	
	if(png_sig_cmp(sig, 0, 8)) {
		fprintf(stderr, "\"%s\" is not a valid PNG file.\n", infile);
		return 1;
	}
	
	png_structp readStruct = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!readStruct) {
		fprintf(stderr, "Failed to load \"%s\". Error 1.\n", infile);
		return 1;
	}
	
	png_structp writeStruct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!writeStruct) {
		return 1;
	}
	
	png_infop infoStruct = png_create_info_struct(readStruct);
	if (!infoStruct) {
		fprintf(stderr, "Failed to load \"%s\". Error 2.\n", infile);
		return 1;
	};
	
	
	
	
	// exceptions are evil. the alternative with libPNG is a bit worse. ever heard of return codes libPNG devs?
	if (setjmp(png_jmpbuf(readStruct)) || setjmp(png_jmpbuf(writeStruct))) {
		fprintf(stderr, "Failed to load \"%s\". Error 3.\n", infile);
		return 1;
	}
	
	
	png_init_io(readStruct, fi);
	png_init_io(writeStruct, fo);
	png_set_sig_bytes(readStruct, 8);

	png_read_info(readStruct, infoStruct);

	int width = png_get_image_width(readStruct, infoStruct);
	int height = png_get_image_height(readStruct, infoStruct);
	
	png_read_update_info(readStruct, infoStruct);

	png_write_info(writeStruct, infoStruct);
	
	int rowbytes = png_get_rowbytes(readStruct, infoStruct);
	png_bytep line = malloc(rowbytes);
	
	while(height--) {
		int i;
		unsigned char* p;
		png_read_row(readStruct, line, NULL);
		
		p = line;
		for(i = 0; i < width; i++) {
			if(p[3] == 0) {
				p[0] = 0;
				p[1] = 0;
				p[2] = 0;
			}
			p += 4;
		} 
		
		png_write_row(writeStruct, line);
	}
	
	
	
	png_write_end(writeStruct, NULL);
	
	png_destroy_read_struct(&readStruct, NULL, NULL);
	png_destroy_write_struct(&writeStruct, NULL);
	
	fclose(fi);
	fclose(fo);
	
	printf("Finished \"%s\".\n", argv[2]);
	
	return 0;
}









