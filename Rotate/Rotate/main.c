#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "structs_h.h"
#include "functions_h.h"


int main(int argc, char *argv[]) {
	char *infile = argv[1];
	FILE *inptr = fopen(infile, "rb");
	char *outfile = argv[2];
	FILE *outptr = fopen(outfile, "wb");
	struct bmp_header* bh_ptr = (struct bmp_header*)malloc(sizeof(struct bmp_header));
	struct image* img_ptr = (struct image*)malloc(sizeof(struct image));
	enum read_status rs = from_bmp(inptr, bh_ptr, img_ptr);
	

	//struct image* new_img_ptr = rotate_image_left(img_ptr);
	struct image* new_img_ptr = rotate_image_rigth(img_ptr);
	struct bmp_header* new_bh_ptr = rotate_header(bh_ptr);
	enum write_status ws = to_bmp(outptr, /*bh_ptr*/new_bh_ptr,/*img_ptr*/new_img_ptr);
	fclose(inptr);
	fclose(outptr);
}