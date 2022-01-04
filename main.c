#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "base.h"
#include "saving.h"
#include "loading.h"
#include "editing.h"

// loads image from file using user input
img load_input(img pic)
{
	FILE *img_file;
	char file_name[50];
	scanf("%s", file_name);

	if (pic.m_word[0] != -1)
		free_img(pic);

	img_file = fopen(file_name, "r+b");
	if (!img_file) {
		printf("Failed to load %s\n", file_name);
		pic.m_word[0] = -1;
		return pic;
	}

	pic = load_img(img_file);
	printf("Loaded %s\n", file_name);
	fclose(img_file);
	return pic;
}

//saves image into file using user input
void save_input(img pic)
{
	if (pic.m_word[0] == -1)
		printf("No image loaded");
	char aux;
	char file_name[50];
	char option[6] = {'\0'};
	scanf("%s", file_name);
	scanf("%c", &aux);
	if (aux == ' ')
		scanf("%s", option);

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
img select_input(img pic, img select_pic)
{
	int x1, y1, x2, y2; // coordinates
	char aux[4];
	scanf("%s", aux);
	if (strcmp(aux, "ALL") == 0) {
		if (pic.m_word[0] == -1) {
			printf("No image loaded\n");
			return select_pic;
		}
		x1 = 0; y1 = 0;
		x2 = pic.w; y2 = pic.h;
		if (select_pic.m_word[0] != -1)
			free_img(select_pic);
		select_pic = new_image(pic, 0, 0, pic.w, pic.h);
		printf("Selected ALL\n");
		return select_pic;
	}
	x1 = atoi(aux);
	scanf("%d %d %d", &y1, &x2, &y2);

	if (pic.h < y1 || pic.h < y2 || pic.w < x1 || pic.w < x2) {
		printf("Invalid set of coordinates\n");
		return select_pic;
	}

	if (pic.m_word[0] == -1) {
		printf("No image loaded\n");
		return select_pic;
	}

	if (select_pic.m_word[0] != -1)
		free_img(select_pic);
	select_pic = new_image(pic, x1, y1, x2, y2);
	printf("Selected %d %d %d %d\n", x1, y1, x2, y2);
	return select_pic;
}

// rotates image by an right angle using user input
img rotate_input(img pic)
{
	int angle;
	scanf("%d", &angle);
	if (pic.m_word[0] == -1) {
		printf("No image loaded\n");
		return pic;
	}
	if (angle % 90 != 0) {
		printf("Unsupported rotation angle\n");
		return pic;
	}
	if (pic.h != pic.w) {
		printf("The selection must be square\n");
		return pic;
	}
	printf("Rotated %d\n", angle);
	angle = angle / 90;
	angle = angle % 4;
	if (angle < 0)
		angle = angle + 4;
	for (int i = 0; i < angle; i++)
		pic = rotate_right_img(pic);
	return pic;
}

// lets user chose what effect to apply
// kernel for effects are stored in text files with the same name
img effects_input(img pic)
{
	char effect[20];
	scanf("%s", effect);

	if (pic.m_word[0] == -1) {
		printf("No image loaded\n");
		return pic;
	}
	if (pic.m_word[1] != '3' && pic.m_word[1] != '6') {
		printf("Easy, Charlie Chaplin\n");
		return pic;
	}
	double k_mat[3][3]; // kernel matrix
	FILE *kernel_file = fopen(effect, "r");
	if (!kernel_file) {
		printf("APPLY parameter invalid\n");
		return pic;
	}
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			fscanf(kernel_file, "%lf", &k_mat[i][j]);
	fclose(kernel_file);

	color **map = apply_effect(pic, k_mat);
	printf("APPLY %s done\n", effect);
	pic.map = map;
	return pic;
}

int main(void)
{
	img pic; // loaded image
	img select_pic; // selected image
	pic.m_word[0] = -1; // is -1 when an image is not loaded
	select_pic.m_word[0] = -1;
	char input[50];

	while (1 == 1) {
		scanf("%s", input);
		if (strcmp(input, "LOAD") == 0) {
			pic = load_input(pic);
			if (pic.m_word[0] == -1)
				continue;
			if (select_pic.m_word[0] != -1)
				free_img(select_pic);
			select_pic = new_image(pic, 0, 0, pic.w, pic.h);
			continue;
		}
		if (strcmp(input, "SELECT") == 0) {
			select_pic = select_input(pic, select_pic);
			continue;
		}
		if (strcmp(input, "CROP") == 0) {
			if (pic.m_word[0] == -1) {
				printf("No image loaded\n");
				continue;
			}
			free_img(pic);
			pic = select_pic;
			select_pic = new_image(pic, 0, 0, pic.w, pic.h);
			printf("Image cropped\n");
			continue;
		}
		if (strcmp(input, "ROTATE") == 0) {
			select_pic = rotate_input(select_pic);
			continue;
		}
		if (strcmp(input, "APPLY") == 0) {
			select_pic = effects_input(select_pic);
			continue;
		}
		if (strcmp(input, "SAVE") == 0) {
			save_input(pic);
			continue;
		}
		if (strcmp(input, "EXIT") == 0) {
			if (pic.m_word[0] == -1) {
				printf("No image loaded\n");
				continue;
			}
			free_img(pic);
			if (select_pic.m_word[0] != -1)
				free_img(select_pic);
			break;
		}
		printf("Invalid command\n");
	}
	return 0;
}
