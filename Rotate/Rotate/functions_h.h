#pragma once
enum read_status;
enum read_status from_bmp(FILE* in, struct bmp_header* const bmp_h, struct image* const read);
enum write_status;
enum write_status to_bmp(FILE* out, struct bmp_header* const bmp_h, struct image* const img);
struct image* rotate_image_rigth(struct image const * source);
struct image* rotate_image_left(struct image const * source);
struct bmp_header* rotate_header(struct bmp_header const * old_header);