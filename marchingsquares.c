//Streche Andrei-Claudiu, 312CA
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define RS_F 4 // resize_factor
#define S_VAL 4 // sampling value
#define PRAG 200 // prag grid

typedef struct {
	unsigned char r;
	unsigned char g;
	unsigned char b;
} imag_t;

int read_matrix(int n, int m, int max_val, imag_t **imag);

void free_im_matrix(int n, imag_t **imag);
void free_char_matrix(int n, char **a);

void write(int n, int m, imag_t **imag);

void resize(int n, int m, imag_t **copy, imag_t **imag);
int copy_matrix(int n, int m, imag_t **copy, imag_t ***imag);

int grid_make(int n, int m, imag_t **imag, char ***grid, int *grid_size);
int return_average(int i, int j, imag_t **imag);

imag_t **alloc_im_matrix(int n, int m);
void realloc_im_matrix(int oldn, int n, int m, imag_t ***a);
char **alloc_char_matrix(int n, int m);

void init_contour(imag_t contours[16][4][4]);
void fill_contour(int k, int i, int j, imag_t countours[16][4][4], int value);

void print_contour(imag_t contours[16][4][4]);
void print_grid(int n, int m, char **grid);

void march(int n, int m, imag_t **imag, char **grid, imag_t contours[16][4][4]);

int base_convertor(int nr, int base);

int main(void)
{
	int n = 0, m = 0, max_val, grid_size = 0;
	char command[20], tip[20], endline;
	imag_t contours[16][4][4], **imag = NULL;

	init_contour(contours);

	while (scanf("%s", command)) {
		if (strcmp(command, "READ") == 0) {

			scanf("%c%s", &endline, tip);
			int oldn = n;

			scanf("%d%d%d", &n, &m, &max_val);

			if (tip[0] == 'P' && tip[1] == '3') {
				realloc_im_matrix(oldn, n, m, &imag);

				if (!imag) {
					fprintf(stderr, "malloc failled 1");
					return 1;
				}

				if (read_matrix(n, m, max_val, imag) != 1) {
					printf("Imagine citita cu succes [%d-%d]\n", n, m);
				} else {
					free_im_matrix(n, imag);
					break;
				}
			} else {
				printf("Eroare: trebuie sa fie P3\n");
				break;
			}
		} else if (command[0] == 'W') {
			write(n, m, imag);
		} else if (command[0] == 'E') {
			free_im_matrix(n, imag);
			printf("Gigel a terminat");
			scanf("%c", &endline);
			break;
		} else if (strcmp(command, "RESIZE") == 0) {
			imag_t **copy = alloc_im_matrix(n * RS_F, m * RS_F);

			if (!copy) {
				fprintf(stderr, "malloc failled 2");
				return 1;
			}

			resize(n, m, copy, imag);
			n *= RS_F;
			m *= RS_F;
			if (copy_matrix(n, m, copy, &imag) != 0) {
				free_im_matrix(n / RS_F, imag);
				free_im_matrix(n, copy);
				break;
			}
		} else if (command[0] == 'G' || command[0] == 'M') {
			char **grid = alloc_char_matrix(n / S_VAL + 1, m / S_VAL + 1);
			if (!grid) {
				fprintf(stderr, "malloc failled 3");
				return 1;
			}
			grid_size = n / S_VAL;
			if (grid_make(n / S_VAL, m / S_VAL, imag, &grid, &grid_size)) {
				fprintf(stderr, "malloc failled 4");
				return 1;
			}

			if (command[0] == 'M') {
				march(n / S_VAL, m / S_VAL, imag, grid, contours);
			} else {
				print_grid(n / S_VAL, m / S_VAL, grid);
			}
			free_char_matrix(grid_size + 1, grid);

		} else if (command[0] == 'I') {
			print_contour(contours);
		}
	}
	return 0;
}

// used to resize imag in a copy
void resize(int n, int m, imag_t **copy, imag_t **imag)
{
	for (int i = n * RS_F - 1; i >= 0; i--) {
		for (int j = m * RS_F - 1; j >= 0; j--) {
			copy[i][j] = imag[i / RS_F][j / RS_F];
		}
	}
	printf("Imagine redimensionata cu succes [%d-%d]\n", n * RS_F, m * RS_F);
	// prints confirmation message
}

// average function
int return_average(int i, int j, imag_t **imag)
{
	int sum = imag[i][j].r;
	sum += imag[i][j].g;
	sum += imag[i][j].b;
	return sum / 3;
}

// function to create the grid
int grid_make(int n, int m, imag_t **imag, char ***grid, int *grid_size)
{
	if (*grid_size != n) {
		// deallocating old grid
		free_char_matrix(*grid_size + 1, *grid);
		char **grid_cpy = alloc_char_matrix(n + 1, m + 1);
		if (!grid_cpy) {
			fprintf(stderr, "malloc failled 5");
			return 1;
		}

		*grid = grid_cpy;
		*grid_size = n;
	}

	for (int i = 0; i <= n; i++) {
		for (int j = 0; j <= m; j++) {
			int adjacent[5] = {0, 0, 0, 0, 0};
			// array used to see what nodes are in proximity with grid node
			if (i > 0 && i < n - 1 && j > 0 && j < m - 1) {
				adjacent[0] = 1;
				adjacent[1] = 1;
				adjacent[2] = 1;
				adjacent[3] = 1;
				adjacent[4] = 1;
			} else {
				if (i != n && j != m) {
					adjacent[2] = 1; // vecin centru
				}
				if (i < n && j < m) {
					adjacent[4] = 1; // vecin dr jos
				}
				if (i < n && j > 0) {
					adjacent[3] = 1; // vecin st jos
				}
				if (i > 0 && j > 0) {
					adjacent[0] = 1; // vecin st sus
				}
				if (i > 0 && j < m) {
					adjacent[1] = 1; // vecin dr sus
				}
			}
			int sum = 0, k = 0;
			// prin sum vom face media aritmetica
			if (adjacent[0] == 1) {
				sum += return_average(S_VAL * i - 1, S_VAL * j - 1, imag);
				k++;
			}
			if (adjacent[1] == 1) {
				sum += return_average(S_VAL * i - 1, S_VAL * j + 1, imag);
				k++;
			}
			if (adjacent[2] == 1) {
				sum += return_average(S_VAL * i, S_VAL * j, imag);
				k++;
			}
			if (adjacent[3] == 1) {
				sum += return_average(S_VAL * i + 1, S_VAL * j - 1, imag);
				k++;
			}
			if (adjacent[4] == 1) {
				sum += return_average(S_VAL * i + 1, S_VAL * j + 1, imag);
				k++;
			}
			sum /= k;
			if (sum > PRAG) {
				(*grid)[i][j] = 0;
			} else {
				(*grid)[i][j] = 1;
			}
		}
	}
	return 0;
}

// making a copy of a imag_t matrix
int copy_matrix(int n, int m, imag_t **copy, imag_t ***imag)
{
	free_im_matrix(n / RS_F, *imag); // free matricei vechi
	imag_t **tmp = alloc_im_matrix(n, m);
	if (!tmp) {
		fprintf(stderr, "malloc failled 6");
		return 1;
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			tmp[i][j].r = copy[i][j].r;
			tmp[i][j].g = copy[i][j].g;
			tmp[i][j].b = copy[i][j].b;
		}
	}
	*imag = tmp;
	// deallocating copy after we're done with it
	if (copy) {
		free_im_matrix(n, copy);
	}
	return 0;
}

// alloc function for a imag_t matrix
imag_t **alloc_im_matrix(int n, int m)
{
	imag_t **a = (imag_t **)malloc(n * sizeof(imag_t *));

	if (!a) {
		fprintf(stderr, "malloc failed 7");
		return NULL;
	}

	for (int i = 0; i < n; i++) {
		a[i] = (imag_t *)malloc(m * sizeof(imag_t));

		if (!a[i]) {
			fprintf(stderr, "malloc failed 8");

			for (int j = i - 1; j >= 0; j--) {
				free(a[j]);
			}
			free(a);
			return NULL;
		}

	}
	return a;
}

// alloc function for an already declared imag_t matrix
void realloc_im_matrix(int oldn, int n, int m, imag_t ***a)
{
	if (*a  && oldn != 0) {
		for (int i = 0; i < oldn; i++) {
			if ((*a)[i] != NULL) {
				free((*a)[i]);
			}
		}
		free(*a);
		*a = NULL;
	}
	// deallocating old matrix

	int ok = 0;
	imag_t **temporary_m = (imag_t **)malloc(n * sizeof(imag_t *));

	if (!temporary_m) {
		fprintf(stderr, "malloc failed 9");
		*a = NULL;
		ok = 1;
	}

	for (int i = 0; i < n; i++) {
		temporary_m[i] = (imag_t *)malloc(m * sizeof(imag_t));

		if (!temporary_m[i]) {
			fprintf(stderr, "malloc failed 10");
			*a = NULL;

			for (int j = i - 1; j >= 0; j--) {
				free(temporary_m[j]);
			}
			free(temporary_m);
			ok = 1;
		}
	}
	// if everything went well
	if (ok == 0) {
		*a = temporary_m;
	}
}

// alloc function for a char matrix
char **alloc_char_matrix(int n, int m)
{
	char **a = (char **)malloc(n * sizeof(char *));

	if (!a) {
		fprintf(stderr, "malloc failed 11");
		return NULL;
	}

	for (int i = 0; i < n; i++) {
		a[i] = (char *)malloc(m * sizeof(char));

		if (!a[i]) {
			fprintf(stderr, "malloc failed 12");

			for (int j = i - 1; j >= 0; j--) {
				free(a[j]);
			}
			free(a);
			return NULL;
		}

	}
	return a;
}

// read function for an image
int read_matrix(int n, int m, int max_val, imag_t **imag)
{
	int col1, col2, col3, index = 0, i = 0, j = 0;
	while (scanf("%d%d%d", &col1, &col2, &col3) == 3) {
		index++;
		if (col1 >= 0 && col2 >= 0 && col3 >= 0) {
			if (col1 <= max_val && col2 <= max_val && col3 <= max_val) {
				imag[i][j].r = col1;
				imag[i][j].g = col2;
				imag[i][j].b = col3;
			} else {
				printf("Eroare: valoare pixel necorespunzatoare\n");
				return 1;
				// required error
			}
		} else {
			printf("Eroare: valoare pixel necorespunzatoare\n");
			return 1;
			// requiered error
		}
		j++;
		if (j > m - 1) {
			j = 0;
			i++;
		}
	}
	if (index != n * m) {
		printf("Eroare: eroare citire pixeli\n");
		return 1;
		// requiered error
	}
	return 0;
}

// dealloc function for an imag_t matrix
void free_im_matrix(int n, imag_t **imag)
{
	for (int i = n - 1; i >= 0; i--) {
		if (imag[i]) {
			free(imag[i]);
		}
	}
	if (imag) {
		free(imag);
	}
}

// dealloc function for an char matrix
void free_char_matrix(int n, char **a)
{
	for (int i = n - 1; i >= 0; i--) {
		if (a[i]) {
			free(a[i]);
		}
	}
	if (a) {
		free(a);
	}
}

// function for filling a contour value
void fill_contour(int k, int i, int j, imag_t contours[16][4][4], int value)
{
	contours[k][i][j].r = value;
	contours[k][i][j].g = value;
	contours[k][i][j].b = value;
}

// function for creating the contours
void init_contour(imag_t contours[16][4][4])
{
	for (int k = 0; k < 16; k++) {
		int x = base_convertor(k, 10);
		int cornr4 = x % 10, cornr3 = (x / 10) % 10;
		int cornr2 = (x / 100) % 10, cornr1 = (x / 1000) % 10;

		int sum = cornr1 + cornr2 + cornr3 + cornr4;
		int c1 = 0, c2 = 0, fill = 0, exterior = 0;
		// we find the 4 corners, and set the fill value, which is 0 as default

		if (sum == 3) {
			fill = 180;
		} else if (sum == 4) {
			fill = 255;
		}
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				fill_contour(k, i, j, contours, fill);
			}
		}

		if (sum == 1) {
			exterior = 180;
		} else if (sum == 3) {
			exterior = 0;
		} else if (sum == 2) {
			exterior = 180;
			if (cornr1 == 1 && cornr3 == 1) {
				c1 = 1;
			} else if (cornr2 == 1 && cornr4 == 1) {
				c2 = 1;
			}
		}

		if (sum == 1 || sum == 3 || c1 + c2 != 0) {
			if ((cornr1 == 1 && sum == 1) || (cornr1 == 0 && sum == 3) || c1) {
				fill_contour(k, 0, 1, contours, 255);
				fill_contour(k, 1, 0, contours, 255);
				fill_contour(k, 0, 0, contours, exterior);
			}
			if ((cornr2 == 1 && sum == 1) || (cornr2 == 0 && sum == 3) || c2) {
				fill_contour(k, 0, 2, contours, 255);
				fill_contour(k, 1, 3, contours, 255);
				fill_contour(k, 0, 3, contours, exterior);
			}
			if ((cornr3 == 1 && sum == 1) || (cornr3 == 0 && sum == 3) || c1) {
				fill_contour(k, 2, 3, contours, 255);
				fill_contour(k, 3, 2, contours, 255);
				fill_contour(k, 3, 3, contours, exterior);
			}
			if ((cornr4 == 1 && sum == 1) || (cornr4 == 0 && sum == 3) || c2) {
				fill_contour(k, 2, 0, contours, 255);
				fill_contour(k, 3, 1, contours, 255);
				fill_contour(k, 3, 0, contours, exterior);
			}
		}
		if (sum == 2 && c1 + c2 == 0) {
			if (cornr1 == 1 && cornr2 == 1) {
				for (int j = 0; j < 4; j++) {
					fill_contour(k, 0, j, contours, 180);
					fill_contour(k, 1, j, contours, 255);
				}
			} else if (cornr3 == 1 && cornr4 == 1) {
				for (int j = 0; j < 4; j++) {
					fill_contour(k, 3, j, contours, 180);
					fill_contour(k, 2, j, contours, 255);
				}
			} else if (cornr1 == 1 && cornr4 == 1) {
				for (int i = 0; i < 4; i++) {
					fill_contour(k, i, 0, contours, 180);
					fill_contour(k, i, 1, contours, 255);
				}
			} else if (cornr2 == 1 && cornr3 == 1) {
				for (int i = 0; i < 4; i++) {
					fill_contour(k, i, 3, contours, 180);
					fill_contour(k, i, 2, contours, 255);
				}
			}
		}
	}
}

// march function
void march(int n, int m, imag_t **imag, char **grid, imag_t contours[16][4][4])
{
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			int code = 0;

			code = ((code + grid[i][j]) * 10 + grid[i][j + 1]) * 10;
			code = (code + grid[i + 1][j + 1]) * 10 + grid[i + 1][j];

			int k = base_convertor(code, 2);

			for (int t = 0; t < 4; t++) {
				for (int r = 0; r < 4; r++) {
					imag[i * 4 + t][j * 4 + r].r = contours[k][t][r].r;
					imag[i * 4 + t][j * 4 + r].g = contours[k][t][r].g;
					imag[i * 4 + t][j * 4 + r].b = contours[k][t][r].b;
				}
			}
		}
	}
	printf("Marching Squares aplicat cu succes [%d-%d]\n", n * 4, m * 4);
}

// write function, for printing the image data
void write(int n, int m, imag_t **imag)
{
	printf("P3\n%d %d\n255\n", n, m);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			printf("%d %d %d\n", imag[i][j].r, imag[i][j].g, imag[i][j].b);
		}
	}
}

// function for contour printing
void print_contour(imag_t contours[16][4][4])
{
	for (int k = 0; k < 16; k++) {
		printf("P3\n4 4\n255\n");

		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				printf("%d %d ", contours[k][i][j].r, contours[k][i][j].g);
				printf("%d\n", contours[k][i][j].b);
			}
		}
	}
}

// function for grid printing
void print_grid(int n, int m, char **grid)
{
	printf("Grid calculat cu succes [%d-%d]\n", n, m);

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			printf("%d ", grid[i][j]);
		}
		printf("\n");
	}
}

// number base convertor
int base_convertor(int nr, int base)
{
	if (base == 10) { // from tenth base
		int x = 0, p = 1;

		while (nr > 0) {
			x += (nr % 2) * p;
			p *= 10;
			nr /= 2;
		}
		return x;

	} else if (base == 2) { // from second base
		int x = 0, b = 1;

		while (nr > 0) {
			x += (nr % 10) * b;
			b *= 2;
			nr /= 10;
		}
		return x;

	}
	return 0;
}
