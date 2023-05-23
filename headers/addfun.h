#ifndef ADDFUN_H
#define ADDFUN_H

typedef struct vec4 {
	float x;
	float y;
	float z;
	float a;
} vec4;

typedef struct metrics {
	double MSE;		// Mean Square Error
	double MAE;		// Mean Absolute Error
	double MaAE;	// Max Absolute Error
	double MaAEC;	// Max Absolute Error
	double HR;		// Height Reduction
	double HRC;		// Height Reduction Color
} metrics;

float get_max(float a, float b, float c, float d, float e);

float get_min(float a, float b, float c, float d, float e);

int power_of_2(int n);

void print_matrix(float *m, int len_row, int len_column);

void print_map_info(map_object *map);

#endif
