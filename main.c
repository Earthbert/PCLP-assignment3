#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char uchar;

typedef struct {
	uchar r, g, b;
} color;

// structure for holding info about image
typedef struct {
	char m_word[3]; // magic word
	int h, w; // height, width
	uchar max; // maximum grey value, if black-white it is 1
	void *mat; // pointer to the begining of the pixel matrix
} img;

// allocs a matrix of h row and w colons with a specific size
void **alloc_img_mat (const long unsigned int size, int h, int w)
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

// reads plain non-color pixel values from file
void read_nc_img(img pic, FILE *input)
{
	char **mat = pic.mat;
	int aux;
	for (int i = 0; i < pic.h; i++){
		for (int j = 0; j < pic.w; j++){
			fscanf(input, "%d", &aux);
			mat[i][j] = aux;
		}
	}
}

// reads plain color pixel values from file
void read_c_img(img pic, FILE *input)
{
	color **mat = pic.mat;
	int aux[3];
	for (int i = 0; i < pic.h; i++){
		for (int j = 0; j < pic.w; j++){
			fscanf(input, "%d %d %d", &aux[0], &aux[1], &aux[2]);
			mat[i][j].r = aux[0];
			mat[i][j].g = aux[1];
			mat[i][j].b = aux[2];
		}
	}
}

// loads file
FILE *load_file(const char *file_name)
{
	FILE *in = fopen(file_name, "r+b");
	if (in == NULL) {
		printf("Failed to load %s\n", file_name);
		return NULL;
	}
	fseek(in, 0, SEEK_SET);
	return in;
}

// loads image from input file
img load_img(FILE *input)
{
	img pic;
	int aux;
	fscanf(input, "%s", pic.m_word);
	pic.m_word[2] = '\0';
	fscanf(input, "%d %d", &pic.w, &pic.h);
	switch (pic.m_word[1]){
		case '1':
		pic.max = 1;
		pic.mat = alloc_img_mat(sizeof(char), pic.h, pic.w);
		read_nc_img(pic, input);
		break;
		case '2':
		fscanf(input, "%d", &aux);
		pic.max = aux;
		pic.mat = alloc_img_mat(sizeof(char), pic.h, pic.w);
		read_nc_img(pic, input);
		break;
		case '3':
		fscanf(input, "%d", &aux);
		pic.max = aux;
		pic.mat = alloc_img_mat(sizeof(color), pic.h, pic.w);
		read_c_img(pic, input);
		break;
	}
	return pic;
}

int main(void)
{	
	FILE *input = NULL;
	img img;
	input = load_file("test2");
	img = load_img(input);
	printf("%s\n", img.m_word);
	uchar **mat = img.mat;
	printf("%d %d\n", img.w, img.h);
	for (int i = 0; i < img.h; i++){
		for (int j = 0; j < img.w; j++){
			printf("%d ", mat[i][j]);
		}
		printf("\n");
	}

	return 0;
}