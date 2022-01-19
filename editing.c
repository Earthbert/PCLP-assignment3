#include <math.h>
#include <stdlib.h>
#include "base.h"

// rotates image 90 degrees to the right
img rotate_right_img(img pic)
{
	if (pic.m_word[1] == '3' || pic.m_word[1] == '6') {
		color **map = pic.map;
		color **new_map = (color **)alloc_mat(sizeof(color), pic.w, pic.h);
		for (int i = 0; i < pic.w; i++)
			for (int j = 0; j < pic.h; j++)
				new_map[i][j] = map[pic.h - j - 1][i];
		for (int i = 0; i < pic.h; i++)
			free(map[i]);
		free(map);
		pic.map = new_map;
	} else {
		uchar **map = pic.map;
		uchar **new_map = (uchar **)alloc_mat(sizeof(char), pic.w, pic.h);
		for (int i = 0; i < pic.w; i++)
			for (int j = 0; j < pic.h; j++)
				new_map[i][j] = map[pic.h - j - 1][i];
		for (int i = 0; i < pic.h; i++)
			free(map[i]);
		free(map);
		pic.map = new_map;
	}
	swap(&pic.h, &pic.w);
	return pic;
}

// applies effect on a selected area from image
img apply_effect(img pic, double k_mat[3][3], coord c)
{
	img aux_pic = new_image(pic, c);
	color **new_map = aux_pic .map;

	color **map = pic.map;

	for (int i = 0; i < aux_pic .h; i++)
		for (int j = 0; j < aux_pic .w; j++) {
			if (i + c.y1 == 0 || j + c.x1 == 0 ||
				i + c.y1 == pic.h - 1 || j + c.x1 == pic.w - 1) {
				new_map[i][j] = map[i + c.y1][j + c.x1];
				continue;
			}
			double r = 0;
			double g = 0;
			double b = 0;
			for (int m = -1; m < 2; m++)
				for (int n = -1; n < 2; n++) {
					r = r + (double)map[i + c.y1 + m][j + c.x1 + n].r
					* k_mat[m + 1][n + 1];
					g = g + (double)map[i + c.y1 + m][j + c.x1 + n].g
					* k_mat[m + 1][n + 1];
					b = b + (double)map[i + c.y1 + m][j + c.x1 + n].b
					* k_mat[m + 1][n + 1];
				}
			if (r < 0)
				r = 0;
			if (r > aux_pic.max)
				r = aux_pic.max;
			if (g < 0)
				g = 0;
			if (g > aux_pic.max)
				g = aux_pic.max;
			if (b < 0)
				b = 0;
			if (b > aux_pic.max)
				b = aux_pic.max;
			new_map[i][j].r = round(r);
			new_map[i][j].g = round(g);
			new_map[i][j].b = round(b);
		}

	return aux_pic;
}
