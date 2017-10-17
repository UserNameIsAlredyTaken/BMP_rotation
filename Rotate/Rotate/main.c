#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#pragma pack(push,2)
struct bmp_header {
	uint16_t bfType;
	uint32_t bfileSize;
	uint32_t bfReserved;
	uint32_t bOffBits;
	uint32_t biSize;
	uint32_t biWidth;
	uint32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	uint32_t biXPelsPerMeter;
	uint32_t biYPelsPerMeter;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
};
#pragma pack(pop)

#pragma pack(push,2)
struct pixel {
	uint8_t b;
	uint8_t g;
	uint8_t r;
};
#pragma pack(pop)
struct image {
	uint64_t width, height;
	struct pixel* data;
};

enum read_status {
	READ_OK = 0,
	READ_COULDNT_OPEN,
	READ_INVALID_BIT_COUNT,

};

enum read_status from_bmp(FILE* in, struct bmp_header* const bmp_h, struct image* const read) {
	if (in == NULL) {
		return READ_COULDNT_OPEN;
	}
	fread(bmp_h, sizeof(struct bmp_header), 1, in);
	if (bmp_h->biSizeImage==0) {
		bmp_h->biSizeImage = bmp_h->bfileSize - bmp_h->bOffBits;
	}
	if (bmp_h->biBitCount != 24) {
		return READ_INVALID_BIT_COUNT;
	}
	read->width = bmp_h->biWidth;
	read->height = abs(bmp_h->biHeight);
	read->data = (struct pixel*)malloc(bmp_h->biSizeImage);
	fread(read->data, 1, bmp_h->biSizeImage, in);	
	return READ_OK;
}

enum write_status {
	WRITE_OK = 0,
	WRITE_COULDNT_CRETE
};
enum write_status to_bmp(FILE* out, struct bmp_header* const bmp_h, struct image* const img) {
	if (out == NULL) {
		return WRITE_COULDNT_CRETE;
	}
	fwrite(bmp_h, sizeof(struct bmp_header), 1, out);	
	fwrite(img->data, 1, bmp_h->biSizeImage, out);
	/*img->data = (struct pixel*)malloc(bmp_h->biSizeImage);
	struct pixel* offset = img->data;
	for (int i = 0; i < abs(bmp_h->biHeight); i++) {
		for(int j = 0; j < bmp_h->biWidth; j++){
			fwrite(offset, sizeof(struct pixel), 1, out);
			offset ++;
		}
		/*fwrite(offset, sizeof(struct pixel), img_width, out);
		offset += sizeof(struct pixel)*img_width;*/
		/*const int padding = (4 - (bmp_h->biWidth * sizeof(struct pixel)) %4)% 4;
		//offset += padding * sizeof(struct pixel);
		for (int k = 0; k < padding; k++) {
			offset++;
			fputc(0x00, out);
		}
	}*/
	return WRITE_OK;
}

struct image* rotate(struct image const * source) {
	/*const uint64_t new_height = old_height;
	const uint64_t new_width = old_width;
	struct pixel new_image[new_height][new_width];*/
	

	
}

int main(int argc, char *argv[]) {
	char *infile = argv[1];
	FILE *inptr = fopen(infile, "rb");
	char *outfile = argv[2];
	FILE *outptr = fopen(outfile, "wb");
	struct bmp_header* bh_ptr = (struct bmp_header*)malloc(sizeof(struct bmp_header));
	struct image* img_ptr = (struct image*)malloc(sizeof(struct image));
	enum read_status rs = from_bmp(inptr, bh_ptr, img_ptr);
	

	struct image* new_img_ptr = rotate(img_ptr);
	enum write_status ws = to_bmp(outptr, bh_ptr,/*img_ptr*/new_img_ptr);
	fclose(inptr);
	fclose(outptr);
}