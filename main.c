#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"
#include "load-save.h"
#include "editing.h"

// loads image from file using user input
void load_input(img *pic, coord *c)
{
	FILE *img_file;
	char file_name[50];
	scanf("%s", file_name);

	if (pic->m_word[0] != -1)
		free_img(pic);

	img_file = fopen(file_name, "r+b");
	if (!img_file) {
		printf("Failed to load %s\n", file_name);
		if (pic->m_word[0] != -1)
			free_img(pic);
		return;
	}

	*pic = load_img(img_file);

	c->x1 = 0; c->y1 = 0;
	c->x2 = pic->w; c->y2 = pic->h;
	printf("Loaded %s\n", file_name);
	fclose(img_file);
}

//saves image into file using user input
void save_input(img pic)
{
	char file_name[50];
	char option[20];
	char option_line[50];
	scanf("%s", file_name);
	fgets(option_line, 50, stdin);

	read_word(option_line, option);

	if (pic.m_word[0] == -1) {
		printf("No image loaded\n");
		return;
	}

	if (strcmp(option, "ascii") == 0) {
		switch (pic.m_word[1]) {
		case '4':
			pic.m_word[1] = '1';
			break;
		case '5':
			pic.m_word[1] = '2';
			break;
		case '6':
			pic.m_word[1] = '3';
			break;
		}
	} else {
		switch (pic.m_word[1]) {
		case '1':
			pic.m_word[1] = '4';
			break;
		case '2':
			pic.m_word[1] = '5';
			break;
		case '3':
			pic.m_word[1] = '6';
			break;
		}
	}
	save_image(pic, file_name);
	printf("Saved %s\n", file_name);
}

// selects coordinates of pixel map using user input
void select_input(img pic, coord *c)
{
	coord b; // backup, in case of invalid coordinates
	b.x1 = c->x1; b.y1 = c->y1;
	b.x2 = c->x2; b.y2 = c->y2;
	char line[50], b_line[50];

	fgets(line, 50, stdin);
	memcpy(b_line, line, 50);
	char *word;
	int word_c = word_count(line);

	if (pic.m_word[0] == -1) {
		printf("No image loaded\n");
		return;
	}

	if (word_c != 1 && word_c != 4) {
		printf("Invalid command\n");
		return;
	}

	word = strtok(line, "\n ");

	if (strcmp(word, "ALL") == 0) {
		c->x1 = 0; c->y1 = 0;
		c->x2 = pic.w; c->y2 = pic.h;
		printf("Selected ALL\n");
		return;
	}

	if (check_num(b_line) == 0) {
		printf("Invalid command\n");
		return;
	}

	c->x1 = atoi(word);
	word = strtok(NULL, " ");
	c->y1 = atoi(word);
	word = strtok(NULL, " ");
	c->x2 = atoi(word);
	word = strtok(NULL, " ");
	c->y2 = atoi(word);

	if (c->x1 > c->x2)
		swap(&c->x1, &c->x2);
	if (c->y1 > c->y2)
		swap(&c->y1, &c->y2);

	if (pic.h < c->y1 || pic.h < c->y2 || pic.w < c->x1 || pic.w < c->x2 ||
		c->x1 == c->x2 || c->y1 == c->y2 ||
		c->x1 < 0 || c->y1 < 0 || c->x2 < 0 || c->y2 < 0) {
		c->x1 = b.x1; c->y1 = b.y1;
		c->x2 = b.x2; c->y2 = b.y2;
		printf("Invalid set of coordinates\n");
		return;
	}

	printf("Selected %d %d %d %d\n", c->x1, c->y1, c->x2, c->y2);
}

// rotates image by an right angle using user input
void rotate_input(img *pic, coord *c)
{
	int angle;
	scanf("%d", &angle);

	if (pic->m_word[0] == -1) {
		printf("No image loaded\n");
		return;
	}

	if (angle % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return;
	}

	if (c->y2 - c->y1 != pic->h && c->x2 - c->x1 != pic->w) {
		if (c->y2 - c->y1 != c->x2 - c->x1) {
			printf("The selection must be square\n");
			return;
		}
	}
	printf("Rotated %d\n", angle);
	angle = angle / 90;
	angle = angle % 4;

	if (angle < 0)
		angle = angle + 4;

	img aux_pic = new_image(*pic, *c);
	for (int i = 0; i < angle; i++)
		aux_pic = rotate_right_img(aux_pic);

	// when the selection was the whole image
	if (c->x1 == 0 && c->y1 == 0 && c->x2 == pic->w && c->y2 == pic->h) {
		c->x2 = aux_pic .w; c->y2 = aux_pic .h;
		free_img(pic);
		*pic = new_image(aux_pic, *c);
	} else {
		paste_img(aux_pic, *pic, *c);
	}

	free_img(&aux_pic);
}

// lets user chose what effect to apply
void effects_input(img pic, coord c)
{
	char effect_line[50], effect[20];
	fgets(effect_line, 50, stdin);

	if (pic.m_word[0] == -1) {
		printf("No image loaded\n");
		return;
	}

	if (word_count(effect_line) == 0) {
		printf("Invalid command\n");
		return;
	}

	read_word(effect_line, effect);

	if (pic.m_word[1] != '3' && pic.m_word[1] != '6') {
		printf("Easy, Charlie Chaplin\n");
		return;
	}

	double k_mat[3][3]; // kernel matrix

	FILE *kernel_file = fopen(effect, "r");
	if (!kernel_file) {
		printf("APPLY parameter invalid\n");
		return;
	}
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			fscanf(kernel_file, "%lf", &k_mat[i][j]);
	fclose(kernel_file);

	img aux_pic = apply_effect(pic, k_mat, c);
	paste_img(aux_pic, pic, c);
	free_img(&aux_pic);

	printf("APPLY %s done\n", effect);
}

int main(void)
{
	img pic; // loaded image
	coord c; // coordinates of selected area
	c.x1 = 0; c.y1 = 0; c.x2 = 0; c.y2 = 0;
	pic.m_word[0] = -1; // is -1 when an image is not loaded
	char input[50];

	while (1 == 1) {
		scanf("%s", input);
		if (strcmp(input, "LOAD") == 0) {
			load_input(&pic, &c);
			continue;
		}
		if (strcmp(input, "SELECT") == 0) {
			select_input(pic, &c);
			continue;
		}
		if (strcmp(input, "CROP") == 0) {
			if (pic.m_word[0] == -1) {
				printf("No image loaded\n");
				continue;
			}
			img aux_pic = pic;
			pic = new_image(aux_pic, c);
			c.x1 = 0; c.y1 = 0;
			c.x2 = pic.w; c.y2 = pic.h;
			free_img(&aux_pic);
			printf("Image cropped\n");
			continue;
		}
		if (strcmp(input, "ROTATE") == 0) {
			rotate_input(&pic, &c);
			continue;
		}
		if (strcmp(input, "APPLY") == 0) {
			effects_input(pic, c);
			continue;
		}
		if (strcmp(input, "SAVE") == 0) {
			save_input(pic);
			continue;
		}
		if (strcmp(input, "EXIT") == 0) {
			if (pic.m_word[0] == -1)
				printf("No image loaded\n");
			if (pic.m_word[0] != -1)
				free_img(&pic);
			break;
		}
		fgets(input, 50, stdin);
		printf("Invalid command\n");
	}
	return 0;
}
