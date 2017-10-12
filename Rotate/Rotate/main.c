#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#pragma pack(push,2)
struct bmp_header{
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
struct pixel{
	uint8_t b, g, r;
};
#pragma pack(pop)
struct image{
	uint64_t width, height;
	struct pixel* data;
};

enum read_status{
	READ_OK = 0,
	READ_COULDNT_OPEN,
	READ_INVALID_BIT_COUNT,
	
};

struct pixel* offset;

enum read_status from_bmp(FILE* in, struct bmp_header* const bmp_h,struct image* const read){
	if(in==NULL){
		return READ_COULDNT_OPEN;
	}
	struct bmp_header bh;
	fread(&bh, sizeof(bh), 1, in);
	if(bh.biBitCount!=24){
		return READ_INVALID_BIT_COUNT;
	}
	*bmp_h = bh;
	read->width = bh.biWidth;
	read->height = abs(bh.biHeight);
	int i;
	int j;
	const int img_height = abs(bh.biWidth);
	const int img_width = bh.biWidth;
	for (i = 0; i < img_height; i++) {
		for (j = 0; j < img_width;j++) {
			offset = read->data;
			fread(&(offset), sizeof(struct pixel), 1,in);
			offset += sizeof(struct pixel);
		}
		const int padding = 4 - (bh.biWidth * sizeof(struct pixel)) % 4;
		fseek(in, padding, SEEK_CUR);
		int k;
		for(k = 0; k< padding; k++){
			const struct pixel null_pix = {0,0,0};
			*offset = null_pix;
		}
	}
	return READ_OK;
}

enum write_status {
	WRITE_OK = 0,
	WRITE_COULDNT_CRETE
};
enum write_status to_bmp(FILE* out, struct bmp_header* const bmp_h, struct image* const img){
	if(out == NULL){
		return WRITE_COULDNT_CRETE;
	}
	fwrite(&bmp_h, sizeof(bmp_h), 1, out);
	int i;
	int j;
	const int img_height = abs(bmp_h->biWidth);
	const int img_width = bmp_h->biWidth;
	for(i = 0; i < img_height; i++){
		for(j = 0; j < img_width; j++){
			offset = img->data;
			fwrite(&(offset), sizeof(struct pixel), 1, out);
			offset += sizeof(struct pixel);
		}
		const int padding = 4 - (bmp_h->biWidth * sizeof(struct pixel)) % 4;
		offset += padding*sizeof(struct pixel);
		int k;
		for(k = 0; k< padding; k++){
			fputc(0x00, out);
		}
	}
	return WRITE_OK;
}

struct image* rotate(struct image const* source){
	
}

int main(int argc, char *argv[]){
	char *infile = argv[1];
	FILE *inptr = fopen(infile, "r");
	char *outfile = argv[2];
	FILE *outptr = fopen(outfile, "w");
	struct bmp_header* bh_ptr = (struct bmp_header*)malloc(sizeof(struct bmp_header));
	struct image* img_ptr = (struct image*)malloc(sizeof(struct image));
	enum read_status rs = from_bmp(inptr, bh_ptr,img_ptr);
	//struct image* new_img_ptr = rotate(img_ptr);
	enum write_status ws = to_bmp(outptr, bh_ptr,img_ptr/*new_img_ptr*/);
	fclose(inptr);
	fclose(outptr);
}