#ifndef _base_H
#define _base_H

typedef unsigned char uchar;

typedef struct {
	uchar r, g, b;
} color;

// structure for holding info about image
typedef struct {
	char m_word[3]; // magic word
	int h, w; // height, width
	uchar max; // maximum grey value, if black-white it is 1
	void *map; // pointer to the begining of the pixel map
} img;

// allocates memory for a matrix of h row and w colons with a specific size
void **alloc_mat(const unsigned long size, const int h, const int w)
{
	void **mat;
	mat = malloc(h * sizeof(void *));
	if (!mat)
		fprintf(stderr, "Allocation error");
	for (int i = 0; i < h; i++) {
		mat[i] = malloc(w * size);
		if (!mat[i]) {
			fprintf(stderr, "Allocation error");
			for (int j = 0; j < i; j++)
				free(mat[j]);
			free(mat);
			return NULL;
		}
	}
	return mat;
}

// frees allocated memory of a image
void free_img (img pic)
{
	if (pic.m_word[1] == '3' || pic.m_word[1] == '6') {
		color **map = pic.map;
		for (int i = 0; i < pic.h; i++)
			free(map[i]);
		free(map);
	} else {
		uchar **map = pic.map;
		for (int i = 0; i < pic.h; i++)
			free(map[i]);
		free(map);
	}
	pic.m_word[0] = -1;
}

// creates a new image from received coordinates relative to input image
img new_image(img input_pic, int x1, int y1, int x2, int y2)
{
	img pic;
	memcpy(pic.m_word, input_pic.m_word, 3);
	pic.max = input_pic.max;

	if (input_pic.h < y1 || input_pic.h < y2)
		printf("Invalid set of coordinates | This shouldn't happen");
	if (input_pic.w < x1 || input_pic.w < x2)
		printf("Invalid set of coordinates | This shouldn't happen");

	if (x1 > x2) {
		int a;
		a = x1;
		x1 = x2;
		x2 = a;
	}
	if (y1 > y2) {
		int a;
		a = y1;
		y1 = y2;
		y2 = a;
	}
	pic.h = y2 - y1;
	pic.w = x2 - x1;

	if (pic.m_word[1] == '3' || pic.m_word[1] == '6') {
		pic.map = alloc_mat(sizeof(color), pic.h, pic.w);
		color **map = pic.map;
		color **input_map = input_pic.map;
		for (int i = 0; i < pic.h; i++)
			for (int j = 0; j < pic.w; j++)
				map[i][j] = input_map[i + y1][j + x1];
	} else {
		pic.map = alloc_mat(sizeof(char), pic.h, pic.w);
		uchar **map = pic.map;
		uchar **input_map = input_pic.map;
		for (int i = 0; i < pic.h; i++)
			for (int j = 0; j < pic.w; j++)
				map[i][j] = input_map[i + y1][j + x1];
	}
	return pic;
}

#endif // _base_H
