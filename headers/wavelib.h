#ifndef WAVELIB_H
#define WAVELIB_H

typedef struct wave_object {
	char wname[10];
	int filtlength; // When all filters are of the same length. [Matlab uses zero-padding to make all filters of the same length]
	int lpd_len; // Default filtlength = lpd_len = lpr_len = hpd_len = hpr_len
	int hpd_len;
	int lpr_len;
	int hpr_len;
	float *lpd;
	float *hpd;
	float *lpr;
	float *hpr;
	float params[0];
}wave_object;

typedef struct map_object {
	wave_object *wavelet;	
	float *matrix;
	int max_level;
	int cur_level;
	int len_row;
	int len_column;
	int max_h;
	int min_h;
}map_object;

void haar_2D_decomposition(map_object *data, int level);


void haar_2D_reconstruction(map_object *data, int level);

#endif
