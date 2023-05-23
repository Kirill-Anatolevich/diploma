#include <stdio.h>
#include "../headers/wavelib.h"

#ifndef ADDFUN_C
#define ADDFUN_C

int power_of_2(int n) {
	if ((n & (n - 1)) != 0) return -1;
	int r = 0;
	while (n > 1) {
		++r;
		n /= 2;
	}
	return r;
}

float get_max(float a,float b,float c,float d,float e) {
	float max_h = a;
	float arr[4] = {b, c, d, e};	
	for (int i = 0; i < 4; ++i)
		if (arr[i] > max_h)
			max_h = arr[i];
	return max_h;
}


float get_min(float a,float b,float c,float d,float e) {
	float min_h = a;
	float arr[4] = {b, c, d, e};	
	for (int i = 0; i < 4; ++i)
		if (arr[i] < min_h)
			min_h = arr[i];
	return min_h;
}

void print_matrix(float *m, int len_row, int len_column) {
	printf("\n-------\n");
	for (int i = 0; i < len_column; ++i) {
		for (int j = 0; j < len_row; ++j) {
			printf("%f ", m[i * len_row + j]);
		}
		printf("\n");
	}
	printf("\n-------\n");
}

void print_map_info(map_object *map) {
	printf("Len row = %d\n", map->len_row);
	printf("Len column = %d\n", map->len_column);
	print_matrix(map->matrix, map->len_row, map->len_column);
	printf("Max height = %d\n", map->max_h);
	printf("Min height = %d\n", map->min_h);
	printf("Max level decomposition = %d\n", map->max_level);
	printf("Curent level decomposition = %d\n", map->cur_level);
}

#endif
