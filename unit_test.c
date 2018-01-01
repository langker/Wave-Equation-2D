#include <stdio.h>
#include "wave_2d.h"

int main(int argc, char *argv[]) {
	
	double *rightdata= process_withoutomp();
	double *testdata= process_withomp();
	
	for(int i = 0; i < GRID_SZ; i++){
		for(int j = 0; j < GRID_SZ; j++){
			if (rightdata[i*GRID_SZ+j]!=testdata[i*GRID_SZ+j]) {
				printf("wrong");
				return 0;
			}
		}
	}
	printf("no wrong\n");
}

double *process_withomp() {
		clock_t start = clock();
		
		int i, j;
		double dt = 0.04, C = 16, K = 0.1, h = 6;
		double *data, *olddata, *newdata, *tmp;
		double x[PEAK_SZ][PEAK_SZ], linspace[PEAK_SZ], delta = 2.0/(PEAK_SZ-1.0);
		data = (double*)malloc(sizeof(double)*ARR_SZ);
		olddata = (double*)malloc(sizeof(double)*ARR_SZ);
		newdata = (double*)malloc(sizeof(double)*ARR_SZ);
		
		#pragma omp parallel for
		for(i = 0; i < ARR_SZ; i++){
				data[i] = 1.0;
		}
		#pragma omp parallel for
		for(i = 0; i < PEAK_SZ; i++){
				linspace[i] = -1.0 + delta * i;
		}
		#pragma omp parallel for
		for(i = 0; i < PEAK_SZ; i++){
				for(j = 0; j < PEAK_SZ; j++){
						x[i][j] = linspace[i];
				}
		}
		#pragma omp parallel for
		for(i = 0; i < PEAK_SZ; i++){
				for(j = 0; j < PEAK_SZ; j++){
						data[(i+20)*GRID_SZ+j+20] += h * exp( -5 * (pow(x[i][j], 2 ) + pow(x[j][i], 2 )));
				}
		}
		#pragma omp parallel for
		for(i = 0; i < ARR_SZ; i++){
				olddata[i] = data[i];
		}
		
		#pragma omp barrier
		 
	 
		for(i = 0; i < 20; i++){
				
				sequential_update_withomp( data, olddata, newdata, C, K, dt);
				tmp = olddata;
				olddata = data;
				data = newdata;
				newdata = tmp;
		}
		
		
		clock_t end = clock();
		printf("with omp spend: %lu\n",end-start);
		return data;
}
void sequential_update_withomp(double *data, double *olddata, double *newdata, double C, double K, double dt ) {
		int i, j, add_i, sub_i, add_j, sub_j;
		double pot;
		for( i = 0; i < GRID_SZ; i++){
				for( j = 0; j < GRID_SZ; j++){
						add_i = i+1 >= GRID_SZ ? i : i+1;
						add_j = j+1 >= GRID_SZ ? j : j+1;
						sub_i = i-1 < 0 ? 0 : i-1;
						sub_j = j-1 < 0 ? 0 : j-1;
						pot = data[add_i*GRID_SZ+j]+
									data[sub_i*GRID_SZ+j]+
									data[add_j+i*GRID_SZ]+
									data[sub_j+i*GRID_SZ]-
									4*data[i*GRID_SZ+j];
						newdata[i * GRID_SZ + j] = 
								( pow(C * dt, 2) * pot * 2 + 4 * data[i * GRID_SZ + j] - olddata[i * GRID_SZ + j] *(2 - K * dt) ) / (2 + K * dt);
				}
		}
}

double *process_withoutomp() {
		clock_t start = clock();
		int i, j;
		double dt = 0.04, C = 16, K = 0.1, h = 6;
		double *data, *olddata, *newdata, *tmp;
		double x[PEAK_SZ][PEAK_SZ], linspace[PEAK_SZ], delta = 2.0/(PEAK_SZ-1.0);
		data = (double*)malloc(sizeof(double)*ARR_SZ);
		olddata = (double*)malloc(sizeof(double)*ARR_SZ);
		newdata = (double*)malloc(sizeof(double)*ARR_SZ);
	
		for(i = 0; i < ARR_SZ; i++){
				data[i] = 1.0;
		}
	 
		for(i = 0; i < PEAK_SZ; i++){
				linspace[i] = -1.0 + delta * i;
		}
	 
		for(i = 0; i < PEAK_SZ; i++){
				for(j = 0; j < PEAK_SZ; j++){
						x[i][j] = linspace[i];
				}
		}
		
		for(i = 0; i < PEAK_SZ; i++){
				for(j = 0; j < PEAK_SZ; j++){
						data[(i+20)*GRID_SZ+j+20] += h * exp( -5 * (pow(x[i][j], 2 ) + pow(x[j][i], 2 )));
				}
		}
		
		for(i = 0; i < ARR_SZ; i++){
				olddata[i] = data[i];
		}

		for(i = 0; i < 20; i++){
				sequential_update_withoutomp( data, olddata, newdata, C, K, dt);
				tmp = olddata;
				olddata = data;
				data = newdata;
				newdata = tmp;
		}
		clock_t end = clock();
		printf("without omp spend: %lu\n",end-start);
		
		return data;
}
void sequential_update_withoutomp(double *data, double *olddata, double *newdata, double C, double K, double dt ){
		int i, j, add_i, sub_i, add_j, sub_j;
		double pot;
		for( i = 0; i < GRID_SZ; i++){
				for( j = 0; j < GRID_SZ; j++){
						add_i = i+1 >= GRID_SZ ? i : i+1;
						add_j = j+1 >= GRID_SZ ? j : j+1;
						sub_i = i-1 < 0 ? 0 : i-1;
						sub_j = j-1 < 0 ? 0 : j-1;
						pot = data[add_i*GRID_SZ+j]+
									data[sub_i*GRID_SZ+j]+
									data[add_j+i*GRID_SZ]+
									data[sub_j+i*GRID_SZ]-
									4*data[i*GRID_SZ+j];
						newdata[i * GRID_SZ + j] = 
								( pow(C * dt, 2) * pot * 2 + 4 * data[i * GRID_SZ + j] - olddata[i * GRID_SZ + j] *(2 - K * dt) ) / (2 + K * dt);
				}
		}
}
