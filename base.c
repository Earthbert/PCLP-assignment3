#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"

void swap(int *x, int *y)
{
	int z = *x;
	*x = *y;
	*y = z;
}

// checks if a string contains only digits
int check_num(char *line)
{
	for (int i = 0; i < (int)strlen(line); i++) {
		if (line[i] >= 65 && 122 >= line[i])
			return 0;
	}
	return 1;
}

// counts words in a string
int word_count(char *line)
{
	unsigned int count = 0;

	for (int i = 0; i < (int)strlen(line); i++) {
		if (line[i] != ' ' && line[i] != '\n') {
			while (i < (int)strlen(line) && line[i] != ' ' && line[i] != '\n')
				i++;
			count++;
		}
	}
	return count;
}

// writes the first word from line into word
void read_word(char *line, char *word)
{
	int pos1 = 0;
	int pos2 = 0;

	for (int i = 0; i < (int)strlen(line); i++)
		if ((line[i] >= 'a' && line[i] <= 'z') ||
			(line[i] >= 'A' && line[i] <= 'Z')) {
			pos1 = i;
			break;
		}

	for (int i = (int)strlen(line); i > 0; i--)
		if ((line[i] >= 'a' && line[i] <= 'z') ||
			(line[i] >= 'A' && line[i] <= 'Z')) {
			pos2 = i;
			break;
		}

	int j = 0;
	for (int i = pos1; i <= pos2; i++) {
		word[j] = line[i];
		j++;
	}
	word[j] = '\0';
}

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
void free_img (img *pic)
{
	if (pic->m_word[1] == '3' || pic->m_word[1] == '6') {
		color **map = pic->map;
		for (int i = 0; i < pic->h; i++)
			free(map[i]);
		free(map);
	} else {
		uchar **map = pic->map;
		for (int i = 0; i < pic->h; i++)
			free(map[i]);
		free(map);
	}
	pic->m_word[0] = -1;
}

// creates a new image from received coordinates relative to input image
img new_image(img input_pic, coord c)
{
	img pic;
	memcpy(pic.m_word, input_pic.m_word, 3);
	pic.max = input_pic.max;

	if (input_pic.h < c.y1 || input_pic.h < c.y2)
		printf("Invalid set of coordinates | This shouldn't happen");
	if (input_pic.w < c.x1 || input_pic.w < c.x2)
		printf("Invalid set of coordinates | This shouldn't happen");

	if (c.x1 > c.x2)
		swap(&c.x1, &c.x2);
	if (c.y1 > c.y2)
		swap(&c.y1, &c.y2);

	pic.h = c.y2 - c.y1;
	pic.w = c.x2 - c.x1;

	if (pic.m_word[1] == '3' || pic.m_word[1] == '6') {
		pic.map = alloc_mat(sizeof(color), pic.h, pic.w);
		color **map = pic.map;
		color **input_map = input_pic.map;
		for (int i = 0; i < pic.h; i++)
			for (int j = 0; j < pic.w; j++)
				map[i][j] = input_map[i + c.y1][j + c.x1];
	} else {
		pic.map = alloc_mat(sizeof(char), pic.h, pic.w);
		uchar **map = pic.map;
		uchar **input_map = input_pic.map;
		for (int i = 0; i < pic.h; i++)
			for (int j = 0; j < pic.w; j++)
				map[i][j] = input_map[i + c.y1][j + c.x1];
	}
	return pic;
}

// puts image 1 over image 2 based on coordinates
void paste_img(img pic1, img pic2, coord c)
{
	if (pic1.m_word[1] == '3' || pic1.m_word[1] == '6') {
		color **map1 = pic1.map;
		color **map2 = pic2.map;
		for (int i = 0; i < pic1.h; i++)
			for (int j = 0; j < pic1.w; j++) {
				map2[c.y1 + i][c.x1 + j].r = map1[i][j].r;
				map2[c.y1 + i][c.x1 + j].g = map1[i][j].g;
				map2[c.y1 + i][c.x1 + j].b = map1[i][j].b;
			}
	} else {
		uchar **map1 = pic1.map;
		uchar **map2 = pic2.map;
		for (int i = 0; i < pic1.h; i++)
			for (int j = 0; j < pic1.w; j++)
				map2[c.y1 + i][c.x1 + j] = map1[i][j];
	}
}
