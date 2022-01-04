#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"

// rotates square image 90 degrees to the right
img rotate_right_img(img pic)
{
	if (pic.h != pic.w) {
		printf("Cannot rotate");
		return pic;
	}
	if (pic.m_word[1] == '3' || pic.m_word[1] == '6') {
		color **map = pic.map;
		color **new_map = (color **)alloc_mat(sizeof(color), pic.h, pic.w);
		for (int i = 0; i < pic.h; i++)
			for (int j = 0; j < pic.w; j++)
				new_map[i][j] = map[pic.h - j - 1][i];
		for (int i = 0; i < pic.h; i++)
			free(map[i]);
		free(map);
		pic.map = new_map;
	} else {
		uchar **map = pic.map;
		uchar **new_map = (uchar **)alloc_mat(sizeof(char), pic.h, pic.w);
		for (int i = 0; i < pic.h; i++)
			for (int j = 0; j < pic.w; j++)
				new_map[i][j] = map[pic.h - j - 1][i];
		for (int i = 0; i < pic.h; i++)
			free(map[i]);
		free(map);
		pic.map = new_map;
	}
	return pic;
}

// applies effect given by kernal
color **apply_effect(img pic, double k_mat[3][3])
{
	color **new_map = (color **)alloc_mat(sizeof(color), pic.h, pic.w);
	color **map = pic.map;

	// it keeps the same values for pixels without enough neighbors
	for (int i = 0; i < pic.w; i++)
		new_map[0][i] = map[0][i];
	for (int i = 0; i < pic.h; i++)
		new_map[i][0] = map[i][0];
	for (int i = 0; i < pic.w; i++)
		new_map[pic.h - 1][i] = map[pic.h - 1][i];
	for (int i = 0; i < pic.h; i++)
		new_map[i][pic.w - 1] = map[i][pic.w - 1];

	for (int i = 1; i < pic.h - 1; i++)
		for (int j = 1; j < pic.w - 1; j++) {
			double r = 0;
			double g = 0;
			double b = 0;
			for (int m = -1; m < 2; m++)
				for (int n = -1; n < 2; n++) {
					r = r + (double)map[i + n][j + m].r * k_mat[m + 1][n + 1];
					g = g + (double)map[i + n][j + m].g * k_mat[m + 1][n + 1];
					b = b + (double)map[i + n][j + m].b * k_mat[m + 1][n + 1];
				}
			if (r < 0)
				r = 0;
			if (r > 255)
				r = 255;
			if (g < 0)
				g = 0;
			if (g > 255)
				g = 255;
			if (b < 0)
				b = 0;
			if (b > 255)
				b = 255;
			new_map[i][j].r = r;
			new_map[i][j].g = g;
			new_map[i][j].b = b;
		}
	for (int i = 0; i < pic.h; i++)
		free(map[i]);
	free(map);
	return new_map;
}
