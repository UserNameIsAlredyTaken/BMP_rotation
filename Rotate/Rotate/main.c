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
	const int padding = (4 - (bmp_h->biWidth * sizeof(struct pixel)) % 4) % 4;
	int i;
	for (i = 0; i < read->height;i++){
		fread(read->data+i*read->width,sizeof(struct pixel), read->width,in);
		fseek(in,padding,SEEK_CUR);
	}
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
	const int padding = (4 - (bmp_h->biWidth * sizeof(struct pixel)) % 4) % 4;
	struct bmp_header new_header = { 0x4D42,sizeof(struct bmp_header)+(img->width+padding)*img->height,0,54,40,img->width,img->height,1,24,0,(img->width + padding)*img->height,0,0,0,0 };
	fwrite(&new_header, sizeof(struct bmp_header), 1, out);
	//fwrite(bmp_h, sizeof(struct bmp_header), 1, out);		
	int i;
	for(i=0;i<img->height;i++){
		fwrite(img->data + i*img->width, sizeof(struct pixel), img->width, out);
		fseek(out, padding, SEEK_CUR);
	}
	return WRITE_OK;
}

struct image* rotate_image_left(struct image const * source) {
	struct image* new_img = (struct image*)malloc(sizeof(struct image));
	new_img->width = source->height;
	new_img->height = source->width;
	new_img->data = (struct pixel*)malloc(sizeof(struct pixel)*new_img->width*new_img->height);
	int i, j;
	for(i=0;i<new_img->height;i++){
		for (j = 0; j<new_img->width; j++) {
			*(new_img->data+i*new_img->width+j) = *(source->data+(new_img->width-j-1)*new_img->height+i);
		}
	}

	return new_img;
}
struct image* rotate_image_rigth(struct image const * source) {
	struct image* new_img = (struct image*)malloc(sizeof(struct image));
	new_img->width = source->height;
	new_img->height = source->width;
	new_img->data = (struct pixel*)malloc(sizeof(struct pixel)*new_img->width*new_img->height);
	int i, j;
	for (i = 0; i<new_img->height; i++) {
		for (j = 0; j<new_img->width; j++) {
			*(new_img->data + i*new_img->width + j) = *(source->data + j*new_img->height + i);
		}
	}

	return new_img;
}
struct bmp_header* rotate_header(struct bmp_header const * old_header){
	uint32_t new_width = old_header->biHeight;
	uint32_t new_height = old_header->biWidth;
	const int padding = (4 - (new_width * sizeof(struct pixel)) % 4) % 4;
	uint32_t new_fileSize = sizeof(struct bmp_header) + (new_width + padding)*new_height;
	uint32_t new_sizeImage = (new_width + padding)*new_height;
	struct bmp_header* new_header = (struct bmp_header*)malloc(sizeof(struct bmp_header));
	new_header->biHeight = new_height;
	new_header->biWidth = new_width;
	new_header->bfileSize = new_fileSize;
	new_header->biSizeImage = new_sizeImage;
	return new_header;
}

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