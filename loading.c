#include <stdio.h>
#include "base.h"

// checks if lines start with # and skips them
void check_comments(FILE *file)
{
	char aux;
	fscanf(file, "%c", &aux);
	while (aux == '#' || aux == '\n') {
		while (aux != '\n')
			fscanf(file, "%c", &aux);
		fscanf(file, "%c", &aux);
	}
	fseek(file, -1, SEEK_CUR);
}

// reads plain (text) non-color pixel values from file into image
void read_nc_img(img pic, FILE *input)
{
	char **map = pic.map;
	int aux;
	for (int i = 0; i < pic.h; i++) {
		for (int j = 0; j < pic.w; j++) {
			fscanf(input, "%d", &aux);
			map[i][j] = aux;
		}
	}
}

// reads plain color pixel values from file into image
void read_c_img(img pic, FILE *input)
{
	color **map = pic.map;
	int aux[3];
	for (int i = 0; i < pic.h; i++) {
		for (int j = 0; j < pic.w; j++) {
			fscanf(input, "%d %d %d", &aux[0], &aux[1], &aux[2]);
			map[i][j].r = aux[0];
			map[i][j].g = aux[1];
			map[i][j].b = aux[2];
		}
	}
}

// reads raw greyscale pixel values from file into image
void read_b_g_img(img pic, FILE *input)
{
	//fseek(input, 1, SEEK_CUR);
	char **map = pic.map;
	int aux;
	for (int i = 0; i < pic.h; i++) {
		for (int j = 0; j < pic.w; j++) {
			fread(&aux, sizeof(char), 1, input);
			map[i][j] = aux;
		}
	}
}

// reads raw black-white pixel values from file
void read_b_bw_img(img pic, FILE *input)
{
	//fseek(input, 1, SEEK_CUR);
	char **map = pic.map;
	uchar aux;
	for (int i = 0; i < pic.h; i++) {
		for (int j = 0; j < pic.w; j = j + 8) {
			fread(&aux, sizeof(char), 1, input);
			for (int k = 0; (k < 8) && (j + k) < pic.w; k++) {
				if (aux >= (1 << (7 - k))) {
					map[i][j + k] = 1;
					aux = aux - (1 << (7 - k));
				} else {
					map[i][j + k] = 0;
				}
			}
		}
	}
}

// reads raw color pixel values from file
void read_b_c_img(img pic, FILE *input)
{
	color **map = pic.map;
	for (int i = 0; i < pic.h; i++) {
		for (int j = 0; j < pic.w; j++)
			fread(&map[i][j], sizeof(color), 1, input);
	}
}

// loads image from file
img load_img(FILE *file)
{
	img pic;
	int aux;

	check_comments(file);
	fscanf(file, "%s", pic.m_word);
	pic.m_word[2] = '\0';

	check_comments(file);
	fscanf(file, "%d %d", &pic.w, &pic.h);
	check_comments(file);

	switch (pic.m_word[1]) {
	case '1':
		pic.max = 1;
		pic.map = alloc_mat(sizeof(char), pic.h, pic.w);
		read_nc_img(pic, file);
		break;
	case '2':
		fscanf(file, "%d", &aux);
		check_comments(file);
		pic.max = aux;
		pic.map = alloc_mat(sizeof(char), pic.h, pic.w);
		read_nc_img(pic, file);
		break;
	case '3':
		fscanf(file, "%d", &aux);
		check_comments(file);
		pic.max = aux;
		pic.map = alloc_mat(sizeof(color), pic.h, pic.w);
		read_c_img(pic, file);
		break;
	case '4':
		pic.max = 1;
		pic.map = alloc_mat(sizeof(char), pic.h, pic.w);
		read_b_bw_img(pic, file);
		break;
	case '5':
		fscanf(file, "%d", &aux);
		check_comments(file);
		pic.max = aux;
		pic.map = alloc_mat(sizeof(char), pic.h, pic.w);
		read_b_g_img(pic, file);
		break;
	case '6':
		fscanf(file, "%d", &aux);
		check_comments(file);
		pic.max = aux;
		pic.map = alloc_mat(sizeof(color), pic.h, pic.w);
		read_b_c_img(pic, file);
		break;
	}
	return pic;
}
