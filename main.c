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
	pic.m_word[0] = -1;
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
	//fseek(input, 1, SEEK_CUR);
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
	//fseek(input, 1, SEEK_CUR);
	char **map = pic.map;
	uchar aux;
	for (int i = 0; i < pic.h; i++){
		for (int j = 0; j < pic.w; j = j + 8){
			fread(&aux, sizeof(char), 1, input);
			for (int k = 0; (k < 8) && (j + k) < pic.w; k++) {
				if(aux >= (1 << (7 - k))) {
					map[i][j + k] = 1;
					aux = aux - (1 << (7 - k));
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
	color **map = pic.map;
	for (int i = 0; i < pic.h; i++){
		for (int j = 0; j < pic.w; j++)
			fread(&map[i][j], sizeof(color), 1, input);
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
			fprintf(file, "%d ", map[i][j]);
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
	uchar aux = 0;
	for (int i = 0; i < pic.h; i++){
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

// rotates square image 90 degrees to the right
img rotate_right_img(img pic)
{
	if (pic.h != pic.w){
		printf("Cannot rotate");
		return pic;
	}
	if (pic.m_word[1] == '3' || pic.m_word[1] == '6') {
		color **map = pic.map;
		color **new_map = (color **)alloc_mat(sizeof(color), pic.h, pic.w);
		for (int i = 0; i < pic.h; i++)
			for (int j = 0; j < pic.w; j++) {
				new_map[i][j] = map[pic.h - j - 1][i];
		}
		for (int i = 0; i < pic.h; i++)
			free(map[i]);
		free(map);
		pic.map = new_map;
	}
	else {
		uchar **map = pic.map;
		uchar **new_map = (uchar **)alloc_mat(sizeof(char), pic.h, pic.w);
		for (int i = 0; i < pic.h; i++)
			for (int j = 0; j < pic.w; j++) {
				new_map[i][j] = map[pic.h - j - 1][i];
		}
		for (int i = 0; i < pic.h; i++)
			free(map[i]);
		free(map);
		pic.map = new_map;
	}
	return pic;
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

// creates a new image from received coordinates relative to input image
img new_image(img input_pic, int x1, int y1, int x2, int y2)
{
	img pic;
	memcpy(pic.m_word, input_pic.m_word, 3);
	pic.max = input_pic.max;

	if(input_pic.h < y1 || input_pic.h < y2)
		printf("Invalid set of coordinates | This shouldn't happen");
	if(input_pic.w < x1 || input_pic.w < x2)
		printf("Invalid set of coordinates | This shouldn't happen");

	if (x1 > x2) swap(&x1, &x2);
	if (y1 > y2) swap(&y1, &y2);
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
	char aux;
	char file_name[50], option[6] = {'s'};
	scanf("%s", file_name);
	scanf("%c", &aux);
	if (aux == ' ') {
		scanf("%s", option);
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

// selects coordinates of pixel map using user input
img select_input (img pic, img select_pic)
{
	int x1, y1, x2, y2; // coordinates
	char aux[4];
	scanf("%s", aux);
	if (strcmp(aux, "ALL") == 0) {
		if (pic.m_word[0] == -1){
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

	if (pic.m_word[0] == -1){
		printf("No image loaded\n");
		return select_pic;
	}

	if (select_pic.m_word[0] != -1)
		free_img(select_pic);
	select_pic = new_image(pic, x1, y1, x2, y2);
	printf("Selected %d %d %d %d \n", x1, y1, x2, y2);
	return select_pic;
}

// rotates image by an right angle using user input
img rotate_input(img pic)
{
	int angle;
	scanf("%d", &angle);
	if(pic.m_word[0] == -1){
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
	if(angle < 0) angle = angle + 4;
	for (int i = 0; i < angle; i++)
		pic = rotate_right_img(pic);
	return pic;
}

// applies effect given by kernal
color **apply_effect(img pic, double kernel_mat[3][3], double div)
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
		for (int j = 1; j < pic.w - 1; j++){
			double r = 0;
			double g = 0;
			double b = 0;
			for (int m = -1; m < 2; m++)
				for (int n = -1; n < 2; n++){
					r = r + map[i+n][j+m].r * kernel_mat[m+1][n + 1];
					g = g + map[i+n][j+m].g * kernel_mat[m+1][n + 1];
					b = b + map[i+n][j+m].b * kernel_mat[m+1][n + 1];
				}
			r = r / div;
			if (r < 0) r = 0;
			if (r > 255) r = 255;
			g = g / div;
			if (g < 0) g = 0;
			if (g > 255) g = 255;
			b = b / div;
			if (b < 0) b = 0;
			if (b > 255) b = 255;
			new_map[i][j].r = r;
			new_map[i][j].g = g;
			new_map[i][j].b = b;
		}
	for (int i = 0; i < pic.h; i++){
		free(map[i]);
	}
	free(map);
	return new_map;
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
	double div;
	double kernel_mat[3][3];
	FILE *kernel_file = fopen(effect, "r");
	if (kernel_file == NULL) {
		printf("APPLY parameter invalid\n");
		return pic;
	}
	fscanf(kernel_file, "%lf", &div);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			fscanf(kernel_file, "%lf", &kernel_mat[i][j]);
	fclose(kernel_file);

	// printf("%d ", div);
	// for (int i = 0; i < 3; i++)
	// 	for ( int j = 0; j < 3; j++)
	// 		printf("%d ", kernel_mat[i][j]);

	color **map = apply_effect(pic, kernel_mat, div);
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
			if(pic.m_word[0] == -1){
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
			if(pic.m_word[0] == -1){
				printf("No image loaded\n");
				continue;
			}
			free_img(pic);
			if(select_pic.m_word[0] != -1)
				free_img(select_pic);
			break;
		}
		printf("Invalid command\n");
	}
	return 0;
}