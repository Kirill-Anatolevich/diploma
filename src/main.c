#include <stdio.h>
#include <stdlib.h>

#include "../headers/wavelib.h"
#include "../headers/addfun.h"
#include "../headers/opengl.h"

void init_map(FILE *fp, map_object *map) {
	int n;
	fscanf(fp, "%d", &n);
	printf("%d\n", n);
	float *heights = (float *)malloc(sizeof(float) * n * n);
	float max_h = -10000;
	float min_h = 10000;
	short int tmp;
	for (int i = 0; i < n; ++i) {
		for (int j = 0; j < n; ++j) {
			fscanf(fp, "%f", &heights[i * n + j]);
			// printf("%f\n", heights[i * n + j]);
			if (heights[i * n + j] > max_h)
				max_h = heights[i * n + j];
			if (heights[i * n + j] < min_h)
				min_h = heights[i * n + j];
		}
	}
	map->matrix = heights;
	map->max_level = map->cur_level = power_of_2(n);
	map->len_row = n;
	map->len_column = n;
	map->max_h = max_h;
	map->min_h = min_h;
}

int main(int argc, char **argv) {
	// FILE *fp = fopen("SRTM-2023-02-06.bin", "r");
	char *filename = "test/t2.txt";
	FILE *fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("File wasn't found\n");
		exit(-1);
	}
	map_object map;
	init_map(fp, &map);
	run(argc, argv, &map);
	return 0;
}
