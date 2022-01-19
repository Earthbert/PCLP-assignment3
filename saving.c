#include <stdio.h>
#include "base.h"

// prints pixel map of non-color image into plain file
void print_nc_img(img pic, FILE *file)
{
	uchar **map = pic.map;
	for (int i = 0; i < pic.h; i++) {
		for (int j = 0; j < pic.w; j++)
			fprintf(file, "%d ", map[i][j]);
	fprintf(file, "\n");
	}
}

// prints pixel map of non-color image into plain file
void print_c_img(img pic, FILE *file)
{
	color **map = pic.map;
	for (int i = 0; i < pic.h; i++) {
		for (int j = 0; j < pic.w; j++)
			fprintf(file, "%d %d %d ", map[i][j].r, map[i][j].g, map[i][j].b);
	fprintf(file, "\n");
	}
}

// prints pixel map of greyscale image into raw file
void print_b_gs_img(img pic, FILE *file)
{
	uchar **map = pic.map;
	for (int i = 0; i < pic.h; i++)
		for (int j = 0; j < pic.w; j++)
			fwrite(&map[i][j], sizeof(char), 1, file);
}

// prints pixel map of color image into raw file
void print_b_c_img(img pic, FILE *file)
{
	color **map = pic.map;
	for (int i = 0; i < pic.h; i++)
		for (int j = 0; j < pic.w; j++)
			fwrite(&map[i][j], sizeof(color), 1, file);
}

// prints pixel map of black-white image into raw file
void print_b_bw_img(img pic, FILE *file)
{
	uchar **map = pic.map;
	uchar aux = 0;
	for (int i = 0; i < pic.h; i++) {
		for (int j = 0; j < pic.w; j = j + 8) {
			aux = 0;
			for (int k = 0; (k < 8) && ((j + k) < pic.w); k++) {
				if (map[i][j + k] == 1)
					aux = aux + (1 << (7 - k));
			}

			fwrite(&aux, sizeof(char), 1, file);
		}
	}
}

// save image into a file
void save_image(img pic, const char *file_name)
{
	FILE *file;
	file = fopen(file_name, "w+b");
	fprintf(file, "%s\n", pic.m_word);
	fprintf(file, "%d %d\n", pic.w, pic.h);

	if (pic.m_word[1] != '1' && pic.m_word[1] != '4')
		fprintf(file, "%d\n", pic.max);

	switch (pic.m_word[1]) {
	case '1':
		print_nc_img(pic, file);
		break;
	case '2':
		print_nc_img(pic, file);
		break;
	case '3':
		print_c_img(pic, file);
		break;
	case '4':
		print_b_bw_img(pic, file);
		break;
	case '5':
		print_b_gs_img(pic, file);
		break;
	case '6':
		print_b_c_img(pic, file);
		break;
	}
	fclose(file);
}
