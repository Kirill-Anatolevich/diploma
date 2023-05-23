#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "../headers/wavelib.h"
#include "../headers/addfun.h"

void haar_2D_decomposition(map_object *data, int level) {
	int len_row, len_column, cur_len_row, cur_len_column;
	len_row = data->len_row;
	len_column = data->len_column;
	float *res_data = (float *)malloc(len_row * len_column * sizeof(float));
	memcpy(res_data, data->matrix, len_row * len_column * sizeof(float));
	float max_h = -10000;
	float min_h = 10000;
	for (int l = 0; l < level; ++l) {
		cur_len_row = len_row / (1 << (data->max_level - data->cur_level));
		cur_len_column = len_column / (1 << (data->max_level - data->cur_level));
		// printf("cur_len_row = %d; cur_len_column = %d\n", cur_len_row, cur_len_column);
		for (int i = 0; i < cur_len_column; i += 2) {
			for (int j = 0; j < cur_len_row; j += 2) {
				res_data[(i / 2) * len_row + j / 2] = data->matrix[i * len_row + j] + 
					data->matrix[i * len_row + j + 1] + data->matrix[(i + 1) * len_row + j] + data->matrix[(i + 1) * len_row + j + 1];
				res_data[(i / 2) * len_row + cur_len_row / 2 + j / 2] = data->matrix[i * len_row + j] -
					data->matrix[i * len_row + j + 1] + data->matrix[(i + 1) * len_row + j] - data->matrix[(i + 1) * len_row + j + 1];
				res_data[(i / 2 + cur_len_column / 2) * len_row + j / 2] = data->matrix[i * len_row + j] +
					data->matrix[i * len_row + j + 1] - data->matrix[(i + 1) * len_row + j] - data->matrix[(i + 1) * len_row + j + 1];
				res_data[(i / 2 + cur_len_column / 2) * len_row + cur_len_row / 2 + j / 2] = data->matrix[i * len_row + j] -
					data->matrix[i * len_row + j + 1] - data->matrix[(i + 1) * len_row + j] + data->matrix[(i + 1) * len_row + j + 1];
				res_data[(i / 2) * len_row + j / 2] /= 4;
				res_data[(i / 2) * len_row + cur_len_row / 2 + j / 2] /= 4;
				res_data[(i / 2 + cur_len_column / 2) * len_row + j / 2] /= 4;
				res_data[(i / 2 + cur_len_column / 2) * len_row + cur_len_row / 2 + j / 2] /= 4;
				// printf("LL res = %f\n", res_data[(i / 2) * len_row + j / 2]);
				max_h = fmax(res_data[(i / 2) * len_row + j / 2], max_h);
				min_h = fmin(res_data[(i / 2) * len_row + j / 2], min_h);

			}
		}
		--data->cur_level;
		float *tmp = data->matrix;
		data->matrix = res_data;
		res_data = tmp;
		memcpy(res_data, data->matrix, len_row * len_column * sizeof(float));
		data->max_h = max_h;
		data->min_h = min_h;
	}
}


void haar_2D_reconstruction(map_object *data, int level) {
	int len_row, len_column, cur_len_row, cur_len_column;
	len_row = data->len_row;
	len_column = data->len_column;
	float *res_data = (float *)malloc(len_row * len_column * sizeof(float));
	memcpy(res_data, data->matrix, len_row * len_column * sizeof(float));
	float *matrix;
	float max_h = -10000;
	float min_h = 10000;
	for (int l = 0; l < level; ++l) {
		matrix = data->matrix;
		cur_len_row = len_row / (1 << (data->max_level - data->cur_level));
		cur_len_column = len_column / (1 << (data->max_level - data->cur_level));
		// printf("cur_len_row = %d; cur_len_column = %d\n", cur_len_row, cur_len_column);
		for (int i = 0; i < cur_len_column; ++i) {
			for (int j = 0; j < cur_len_row; ++j) {
				// printf("a[%d] = %f\n", i * len_row  + j, matrix[i * len_row  + j]);
				// printf("a[%d] = %f\n", (i + cur_len_column) * len_row + j, matrix[(i + cur_len_column) * len_row + j]);
				// printf("a[%d] = %f\n", i * len_row + cur_len_row + j, matrix[i * len_row + cur_len_row + j]); 
				// printf("a[%d] = %f\n", (i + cur_len_column) * len_row + cur_len_column + j, matrix[(i + cur_len_column) * len_row + cur_len_column + j]);
				res_data[2 * i * len_row + 2 * j] = matrix[i * len_row + j] + matrix[(i + cur_len_column) * len_row + j] +
					matrix[i * len_row + cur_len_row + j] + matrix[(i + cur_len_column) * len_row + cur_len_row + j];
				res_data[2 * i * len_row + 2 * j + 1] = matrix[i * len_row  + j] + matrix[(i + cur_len_column) * len_row + j]
					- (matrix[i * len_row + cur_len_row + j] + matrix[(i + cur_len_column) * len_row + cur_len_row + j]);
				res_data[(2 * i + 1) * len_row + 2 * j] = (matrix[i * len_row  + j] - matrix[(i + cur_len_column) * len_row + j])
					+ (matrix[i * len_row + cur_len_row + j] - matrix[(i + cur_len_column) * len_row + cur_len_row + j]);
				res_data[(2 * i + 1) * len_row + 2 * j + 1] = (matrix[i * len_row  + j] - matrix[(i + cur_len_column) * len_row + j])
					- (matrix[i * len_row + cur_len_row + j] - matrix[(i + cur_len_column) * len_row + cur_len_row + j]);
				res_data[2 * i * len_row + 2 * j] /= 1;
				res_data[2 * i * len_row + 2 * j + 1] /= 1;
				res_data[(2 * i + 1) * len_row + 2 * j] /= 1;
				res_data[(2 * i + 1) * len_row + 2 * j + 1] /= 1;
				max_h = get_max(res_data[2 * i * len_row + 2 * j], res_data[2 * i * len_row + 2 * j + 1], 
						res_data[(2 * i + 1) * len_row + 2 * j], res_data[(2 * i + 1) * len_row + 2 * j + 1], max_h);
				min_h = get_min(res_data[2 * i * len_row + 2 * j], res_data[2 * i * len_row + 2 * j + 1], 
						res_data[(2 * i + 1) * len_row + 2 * j], res_data[(2 * i + 1) * len_row + 2 * j + 1], min_h);

			}
		}
		data->cur_level += 1;
		float *tmp = data->matrix;
		data->matrix = res_data;
		res_data = tmp;
		memcpy(res_data, data->matrix, len_row * len_column * sizeof(float));
		data->max_h = max_h;
		data->min_h = min_h;
	}
}
