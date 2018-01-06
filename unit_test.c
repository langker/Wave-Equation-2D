#include <stdio.h>
#include "unit_test.h"

int main(int argc, char *argv[]) {
	
	double *rightdata= process_withoutomp();
	double *testdata1= process_withomp1();
	double *testdata= process_withomp();
	
	for(int i = 0; i < GRID_SZ; i++){
		for(int j = 0; j < GRID_SZ; j++){
			if (rightdata[i*GRID_SZ+j]!=testdata1[i*GRID_SZ+j]) {
				printf("wrong1");
				return 0;
			}
		}
	}
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
// WITHOMP PART
double *process_withomp1() {
		double start = omp_get_wtime();
		
		int i, j;
		double dt = 0.04, C = 16, K = 0.1, h = 6;
		double *data, *olddata, *newdata, *tmp;
		double x[PEAK_SZ][PEAK_SZ], linspace[PEAK_SZ], delta = 2.0/(PEAK_SZ-1.0);
		data = (double*)malloc(sizeof(double)*ARR_SZ);
		olddata = (double*)malloc(sizeof(double)*ARR_SZ);
		newdata = (double*)malloc(sizeof(double)*ARR_SZ);
		
		#pragma omp parallel for private(i) schedule(auto)
		for(i = 0; i < ARR_SZ; i++){
				data[i] = 1.0;
		}
		
		#pragma omp parallel for private(i,j) schedule(auto)
		for(i = 0; i < PEAK_SZ; i++){
			linspace[i] = -1.0 + delta * i;
			for(j = 0; j < PEAK_SZ; j++) {
				x[i][j] = linspace[i];
			}
		}
		
		#pragma omp barrier
		
		#pragma omp parallel for private(i,j) schedule(auto)
		for(i = 0; i < PEAK_SZ; i++){
			for(j = 0; j < PEAK_SZ; j++){
				data[(i+20)*GRID_SZ+j+20] += h * exp( -5 * (pow(x[i][j], 2 ) + pow(x[j][i], 2 )));
			}
		}
		
		#pragma omp barrier
		
		#pragma omp parallel for private(i) schedule(auto)
		for(i = 0; i < ARR_SZ; i++){
			olddata[i] = data[i];
		}
		
		#pragma omp barrier
		
		for(i = 0; i < 20; i++) {
			
				sequential_update_withomp1( data, olddata, newdata, C, K, dt);
				tmp = olddata;
				olddata = data;
				data = newdata;
				newdata = tmp;
			
		}
		
		
		double end = omp_get_wtime();
		printf("with omp spend: %f\n",end-start);
		return data;
}
void sequential_update_withomp1(double *data, double *olddata, double *newdata, double C, double K, double dt ) {
		int i, j;
		double pot;
		for( i = 0; i < GRID_SZ; i++) {
			for( j = 0; j < GRID_SZ; j++) {
				pot = data[(i+1 >= GRID_SZ ? i : i+1)*GRID_SZ+j]+
						data[(i-1 < 0 ? 0 : i-1)*GRID_SZ+j]+
						data[(j+1 >= GRID_SZ ? j : j+1)+i*GRID_SZ]+
						data[(j-1 < 0 ? 0 : j-1)+i*GRID_SZ]
						-4*data[i*GRID_SZ+j];
				newdata[i * GRID_SZ + j] = 
								(pow(C * dt, 2) * pot * 2 + 4 * data[i * GRID_SZ + j] - olddata[i * GRID_SZ + j] 
								* (2 - K * dt)) 
								/ (2 + K * dt);
				
			}
		}
}
// WITHOMP PART END

// WITHOMP PART
double *process_withomp() {
		double start = omp_get_wtime();
		
		int i, j;
		double dt = 0.04, C = 16, K = 0.1, h = 6;
		double *data, *olddata, *newdata, *tmp;
		double x[PEAK_SZ][PEAK_SZ], linspace[PEAK_SZ], delta = 2.0/(PEAK_SZ-1.0);
		data = (double*)malloc(sizeof(double)*ARR_SZ);
		olddata = (double*)malloc(sizeof(double)*ARR_SZ);
		newdata = (double*)malloc(sizeof(double)*ARR_SZ);
		
		#pragma omp parallel for private(i) schedule(auto)
		for(i = 0; i < ARR_SZ; i++){
				data[i] = 1.0;
		}
		
		#pragma omp parallel for private(i,j) schedule(auto)
		for(i = 0; i < PEAK_SZ; i++){
			linspace[i] = -1.0 + delta * i;
			for(j = 0; j < PEAK_SZ; j++) {
				x[i][j] = linspace[i];
			}
		}
		
		#pragma omp barrier
		
		#pragma omp parallel for private(i,j) schedule(auto)
		for(i = 0; i < PEAK_SZ; i++){
			for(j = 0; j < PEAK_SZ; j++){
				data[(i+20)*GRID_SZ+j+20] += h * exp( -5 * (pow(x[i][j], 2 ) + pow(x[j][i], 2 )));
			}
		}
		
		#pragma omp barrier
		
		#pragma omp parallel for private(i) schedule(auto)
		for(i = 0; i < ARR_SZ; i++){
			olddata[i] = data[i];
		}
		
		#pragma omp barrier
			 
		for(i = 0; i < 20; i++) {
			sequential_update_withomp( data, olddata, newdata, C, K, dt);
			tmp = olddata;
			olddata = data;
			data = newdata;
			newdata = tmp;
		}
		
		
		double end = omp_get_wtime();
		printf("with omp spend: %f\n",end-start);
		return data;
}
void sequential_update_withomp(double *data, double *olddata, double *newdata, double C, double K, double dt ) {
		int i, j;
		double pot;
		#pragma omp parallel for private(i,j,pot) schedule(auto)
		for( i = 0; i < GRID_SZ; i++) {
			for( j = 0; j < GRID_SZ; j++) {
				pot = data[(i+1 >= GRID_SZ ? i : i+1)*GRID_SZ+j]+
					data[(i-1 < 0 ? 0 : i-1)*GRID_SZ+j]+
					data[(j+1 >= GRID_SZ ? j : j+1)+i*GRID_SZ]+
					data[(j-1 < 0 ? 0 : j-1)+i*GRID_SZ]
					-4*data[i*GRID_SZ+j];
				newdata[i * GRID_SZ + j] = 
							(pow(C * dt, 2) * pot * 2 + 4 * data[i * GRID_SZ + j] - olddata[i * GRID_SZ + j] 
							* (2 - K * dt)) 
							/ (2 + K * dt);
			}
		}
}
// WITHOMP PART END

// WITHOUTOMP PART
double *process_withoutomp() {
		double start = omp_get_wtime();
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
		double end = omp_get_wtime();
		printf("without omp spend: %f\n",end-start);
		
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
// WITHOUTOMP PART END
