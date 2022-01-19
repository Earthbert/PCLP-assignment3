#ifndef _base_H
#define _base_H

// shortcuts
typedef unsigned char uchar;

typedef struct {
	uchar r, g, b;
} color;

// structure for coordinates
typedef struct {
	int x1, y1, x2, y2;
} coord;

// structure for holding info about image
typedef struct {
	char m_word[3]; // magic word
	int h, w; // height, width
	uchar max; // maximum grey value, if black-white it is 1
	void *map; // pointer to the begining of the pixel map
} img;

void swap(int *x, int *y);
int check_num(char *line);
int word_count(char *line);
void read_word(char *line, char *word);
void **alloc_mat(const unsigned long size, const int h, const int w);
img new_image(img input_pic, coord c);
void free_img (img *pic);
void paste_img(img pic1, img pic2, coord c);

#endif // _base_H
