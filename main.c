#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uchar;

typedef struct {
	uchar r, g, b;
} color;

void swap(int *x, int *y)
{
	int z = *x;
	*x = *y;
	*y = z;
}

// checks if lines start with # and skips them
void check_comments(FILE *file)
{
	char aux;
	fscanf(file, "%c", &aux);
	while (aux == '#' || aux == '\n') {
		while (aux != '\n') {
			fscanf(file, "%c", &aux);
		}
		fscanf(file, "%c", &aux);
	}
	fseek(file, -1, SEEK_CUR);
}

// structure for holding info about image
typedef struct {
	char m_word[3]; // magic word
	int h, w; // height, width
	uchar max; // maximum grey value, if black-white it is 1
	void *map; // pointer to the begining of the pixel map
} img;

// allocates memory for a matrix of h row and w colons with a specific size
void **alloc_mat (const long unsigned int size, const int h, const int w)
{
	void **mat;
	mat = malloc(h * sizeof(void *));
	if(!mat)
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
		for (int i = 0; i < pic.h; i++){
			free(map[i]);
		}
		free(map);
	} else {
		uchar **map = pic.map;
		for (int i = 0; i < pic.h; i++){
			free(map[i]);
		}
		free(map);
	}
}

// reads plain non-color pixel values from file
void read_nc_img(img pic, FILE *input)
{
	char **map = pic.map;
	int aux;
	for (int i = 0; i < pic.h; i++){
		for (int j = 0; j < pic.w; j++){
			fscanf(input, "%d", &aux);
			map[i][j] = aux;
		}
	}
}

// reads plain color pixel values from file
void read_c_img(img pic, FILE *input)
{
	color **map = pic.map;
	int aux[3];
	for (int i = 0; i < pic.h; i++){
		for (int j = 0; j < pic.w; j++){
			fscanf(input, "%d %d %d", &aux[0], &aux[1], &aux[2]);
			map[i][j].r = aux[0];
			map[i][j].g = aux[1];
			map[i][j].b = aux[2];
		}
	}
}

// reads raw greyscale pixel values from file
void read_b_g_img(img pic, FILE *input)
{
	fseek(input, 1, SEEK_CUR);
	char **map = pic.map;
	int aux;
	for (int i = 0; i < pic.h; i++){
		for (int j = 0; j < pic.w; j++){
			fread(&aux, sizeof(char), 1, input);
			map[i][j] = aux;
		}
	}
}

// reads raw black-white pixel values from file
void read_b_bw_img(img pic, FILE *input)
{
	fseek(input, 1, SEEK_CUR);
	char **map = pic.map;
	int aux;
	for (int i = 0; i < pic.h; i++){
		for (int j = 0; j < pic.w; j = j + 8){
			fread(&aux, sizeof(char), 1, input);
			uchar tmp = aux;
			for (int k = 0; (k < 8) && (j + k) < pic.w; k++) {
				if(tmp >= (1 << (7 - k))) {
					map[i][j + k] = 1;
					tmp = tmp - (1 << (7 - k));
				}
				else
					map[i][j + k] = 0;
			}
		}
	}
}

// reads raw color pixel values from file
void read_b_c_img(img pic, FILE *input)
{
	fseek(input, 1, SEEK_CUR);
	color **map = pic.map;
	int aux[3];
	for (int i = 0; i < pic.h; i++){
		for (int j = 0; j < pic.w; j++){
			fread(&aux[0], sizeof(char), 1, input);
			fread(&aux[1], sizeof(char), 1, input);
			fread(&aux[2], sizeof(char), 1, input);
			map[i][j].r = aux[0];
			map[i][j].g = aux[1];
			map[i][j].b = aux[2];
		}
	}
}

// loads file
FILE *load_file(const char *file_name)
{
	FILE *in = fopen(file_name, "r+b");
	if (in == NULL) {
		return NULL;
	}
	fseek(in, 0, SEEK_SET);
	return in;
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

	switch (pic.m_word[1]){
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

// prints pixel map of non-color image into plain file
void print_nc_img(img pic, FILE *file)
{
	uchar **map = pic.map;
	for (int i = 0; i < pic.h; i++){
		for (int j = 0; j < pic.w; j++){
			fprintf(file, "%d", map[i][j]);
		}
	fprintf(file,"\n");
	}
}

// prints pixel map of non-color image into plain file
void print_c_img(img pic, FILE *file)
{
	color **map = pic.map;
	for (int i = 0; i < pic.h; i++){
		for (int j = 0; j < pic.w; j++){
			fprintf(file, "%d %d %d ", map[i][j].r, map[i][j].g, map[i][j].b);
		}
	fprintf(file,"\n");
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
	for (int i = 0; i < pic.h; i++)
		for (int j = 0; j < pic.w; i = i + 8) {
			uchar aux = 0;
			for (int k = 0; k < 8 && j + k < pic.w; k++)
				if(map[i][j + k] == 1)
					aux = aux + (1 << (7 - k));
			fwrite(&map[i][j], sizeof(char), 1, file);
		}
}

// save image into a file
void save_image(img pic, const char *file_name)
{
	FILE *file;
	file = fopen(file_name, "w+b");
	fprintf(file, "%s\n", pic.m_word);
	fprintf(file, "%d %d\n", pic.w, pic.h);

	if (pic.m_word[1] != '1' || pic.m_word[1] != '4')
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

// creates a new image from received coordinates relative to input image
img new_image(img input_pic, int x[2], int y[2])
{
	img pic;
	memcpy(pic.m_word, input_pic.m_word, 3);
	pic.max = input_pic.max;

	if(input_pic.h < y[0] || input_pic.h < y[1])
		printf("Invalid set of coordinates");
	if(input_pic.w < x[0] || input_pic.w < x[1])
		printf("Invalid set of coordinates");

	if (x[0] > x[1]) swap(&x[0], &x[1]);
	if (y[0] > y[1]) swap(&y[0], &y[1]);
	pic.h = y[1] - y[0];
	pic.w = x[1] - x[0];

	if (pic.m_word[1] == '3' || pic.m_word[1] == '6') {
		pic.map = alloc_mat(sizeof(color), pic.h, pic.w);
		color **map = pic.map;
		color **input_map = input_pic.map;
		for (int i = 0; i < pic.h; i++)
			for (int j = 0; j < pic.w; j++)
				map[i][j] = input_map[i + y[0]][j + x[0]];
	} else {
		pic.map = alloc_mat(sizeof(char), pic.h, pic.w);
		uchar **map = pic.map;
		uchar **input_map = input_pic.map;
		for (int i = 0; i < pic.h; i++)
			for (int j = 0; j < pic.w; j++)
				map[i][j] = input_map[i + y[0]][j + x[0]];
	}
	return pic;
}

// loads image from file using user input
img load_input (img pic)
{
	FILE *img_file;
	char file_name[50];
	scanf("%s", file_name);

	if(pic.m_word[0] != -1) {
		free_img(pic);
	}

	img_file = load_file(file_name);
	if(img_file == NULL){
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
void save_input (img pic)
{
	if (pic.m_word[0] == -1) printf("No image loaded");
	char file_name[50], option[6];
	scanf("%s", file_name);
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
	}
	else {
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

int main(void)
{	
	img pic; // loaded image
	pic.m_word[0] = -1; // is -1 when an image is not loaded
	//int x[2], y[2]; // selection coordinates
	char input[50];

	while (1 == 1) {
		scanf("%s", input);
		if (strcmp(input, "LOAD") == 0) {
			pic = load_input(pic);
			// x[0] = 0; y[0] = 0;
			// x[1] = pic.w; y[1] = pic.h;
			continue;
		}
		if (strcmp(input, "SAVE") == 0) {
			save_input(pic);
			continue;
		}
		if (strcmp(input, "EXIT") == 0) {
			if(pic.m_word[0] == -1){
				printf("No image loaded");
				break;
			}
			free_img(pic);
			break;
		}
		printf("Invalid command\n");
	}
	return 0;
}