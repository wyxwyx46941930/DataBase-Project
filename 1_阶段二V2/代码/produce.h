#ifndef PRODUCE_H
#define PRODUCE_H
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "fileIO.h"
#include <math.h>
#include "try.h"
#define PI 3.1415926
#define M (10*784)


/*
*get a vector by id in all processed file
*except preprocess_data_mnist and origin_data_mnist and gaussian
*
*/
int* get_vector_by_id_in_all_fly_file(int id, const char* filePath) {
	int page_id, solt_size, solt_number;
	int* header_int = (int*)malloc(3*sizeof(int));
	FILE *fin = NULL;
	bool temp;
	char s[40];
	int num_page;
	int index;
	memset(s, 0, sizeof(s));
	if(filePath == "fly_WTA_data_mnist/") {
		int temp_id;
		bool* header_bool = (bool*)malloc(251*sizeof(bool));
		int *return_array = (int*)malloc(784*10 * sizeof(int));
		memset(return_array, 0, 7840 * sizeof(int));
		int *pair_array = (int*)malloc(64 * sizeof(int));
		num_page = id/251;
		index = id%251;
		int* before_data = (int*)malloc(65*(index-1)*sizeof(int));
		sprintf(s, "%s%d", filePath, num_page);
		FILE *fin = fopen(s, "rb");
		fread(header_int, sizeof(int), 3, fin);
		fread(header_bool, sizeof(bool), 251, fin);
		if(index == 0) {
			fread(&temp_id, sizeof(int), 1, fin);
			fread(pair_array, sizeof(int), 64, fin);
		} else {
			fread(before_data, sizeof(int), 65*(index -1), fin);
			fread(&temp_id, sizeof(int), 1, fin);
			fread(pair_array, sizeof(int), 64, fin);
		}

		int t1[32] = {0};
		int t2[32] = {0};
		int tag1 = 0, tag2 = 0;
		for(int i = 0; i < 64; i++) {
			if(i % 2 == 0) {
				t1[tag1++] = pair_array[i];
			} else {
				t2[tag2++] = pair_array[i];
			}
		}
		for(int i = 0; i < 32; i++) {
			return_array[t1[i]] = t2[i];
		}
		//printf("%d\n", temp_id);
		/*
		for(int i = 0; i < 100; i++) {
			printf("%d ", return_array[i]);
		}
		printf("\n");
		*/
		fclose(fin);
		return return_array;
	} else if( filePath == "fly_data_mnist/") {
		int temp_id;
		bool* header_bool = (bool*)malloc(2*sizeof(bool));
		int *return_array = (int*)malloc(784*10 * sizeof(int));
		num_page = id/2;
		index = id%2;
		int* before_data = (int*)malloc(7841*(index-1)*sizeof(int));
		sprintf(s, "%s%d", filePath, num_page);
		FILE *fin = fopen(s, "rb");
		int header_in;
		fread(header_int, sizeof(int), 3, fin);
		fread(header_bool, sizeof(bool), 2, fin);
		if(index == 0) {
			fread(&temp_id, sizeof(int), 1, fin);
			fread(return_array, sizeof(int), 7840, fin);
		} else {
			fread(before_data, sizeof(int), 7841*(index -1), fin);
			fread(&temp_id, sizeof(int), 1, fin);
			fread(return_array, sizeof(int), 7840, fin);
		}
		
		//printf("%d\n", temp_id);
		/*
		for(int i = 0; i < 10; i++) {
			printf("%d ", return_array[i]);
		}
		printf("\n");
		*/
		fclose(fin);
		return return_array;
	} else if(filePath == "fly_random_data_mnist/") {
		int temp_id;
		bool* header_bool = (bool*)malloc(492*sizeof(bool));
		int *return_array = (int*)malloc(32 * sizeof(int));
		num_page = id/492;
		index = id%492;
		int* before_data = (int*)malloc(33*(index-1)*sizeof(int));
		sprintf(s, "%s%d", filePath, num_page);
		FILE *fin = fopen(s, "rb");
		int header_in;
		fread(header_int, sizeof(int), 3, fin);
		fread(header_bool, sizeof(bool), 492, fin);
		if(index == 0) {
			fread(&temp_id, sizeof(int), 1, fin);
			fread(return_array, sizeof(int), 32, fin);
		} else {
			fread(before_data, sizeof(int), 33*(index -1), fin);
			fread(&temp_id, sizeof(int), 1, fin);
			fread(return_array, sizeof(int), 32, fin);
		}
		
		//printf("%d\n", temp_id);
		/*
		for(int i = 0; i < 32; i++) {
			printf("%d ", return_array[i]);
		}
		printf("\n");
		*/
		fclose(fin);
		return return_array;

	} else if(filePath == "fly_binary_data_mnist/") {
		int temp_id;
		bool* header_bool = (bool*)malloc(492*sizeof(bool));
		int *return_array = (int*)malloc(784*10 * sizeof(int));
		memset(return_array, 0, 7840 * sizeof(int));
		int *index_array = (int*)malloc(32 * sizeof(int));
		num_page = id/492;
		index = id%492;
		int* before_data = (int*)malloc(33*(index-1)*sizeof(int));
		sprintf(s, "%s%d", filePath, num_page);
		FILE *fin = fopen(s, "rb");
		fread(header_int, sizeof(int), 3, fin);
		fread(header_bool, sizeof(bool), 492, fin);
		if(index == 0) {
			fread(&temp_id, sizeof(int), 1, fin);
			fread(index_array, sizeof(int), 32, fin);
		} else {
			fread(before_data, sizeof(int), 33*(index -1), fin);
			fread(&temp_id, sizeof(int), 1, fin);
			fread(index_array, sizeof(int), 32, fin);
		}

		for(int i = 0; i < 32; i++) {
			return_array[index_array[i]] = 1;
		}
		
		//printf("%d\n", temp_id);
		/*
		for(int i = 0; i < 100; i++) {
			printf("%d ", return_array[i]);
		}
		printf("\n");
		*/
		fclose(fin);
		return return_array;
	} else {

	}
	
}

/*
*get a vector by id in gaussian file
*
*/
float* get_vector_by_id_in_gaussian_data_mnist(int id, const char* filePath) {
	int page_id, solt_size, solt_number;
	int* header_int = (int*)malloc(3*sizeof(int));
	FILE *fin = NULL;
	bool temp;
	char s[40];
	int num_page;
	int index;
	memset(s, 0, sizeof(s));
	if(filePath == "gaussian_data_mnist/") {
		int temp_id;
		bool* header_bool = (bool*)malloc(492*sizeof(bool));
		float *return_array = (float*)malloc(32 * sizeof(int));
		num_page = id/492;
		index = id%492;
		float* before_data = (float*)malloc(33*(index-1)*sizeof(float));
		sprintf(s, "%s%d", filePath, num_page);
		FILE *fin = fopen(s, "rb");
		int header_in;
		fread(header_int, sizeof(int), 3, fin);
		fread(header_bool, sizeof(bool), 492, fin);
		if(index == 0) {
			fread(&temp_id, sizeof(int), 1, fin);
			fread(return_array, sizeof(float), 32, fin);
		} else {
			fread(before_data, sizeof(float), 33*(index -1), fin);
			fread(&temp_id, sizeof(int), 1, fin);
			fread(return_array, sizeof(float), 32, fin);
		}
		/*
		printf("%d\n", temp_id);
		for(int i = 0; i < 32; i++) {
			printf("%lf ", return_array[i]);
		}
		printf("\n");
		*/
		fclose(fin);
		return return_array;
	}
}



/*
*get a vector by id in fly_random_data_mnist
*
*/

/*
*get a vector by id in fly_WTA_data_mnist
*
*/

/*
*storage the matrix  matrix/gaussian_matrix
*32*784
*/
void produce_gaussian_projection_matrix(int k, const char* filePath) {
	float matrix[k][784];
	char s[40];
	FILE *fout = NULL;
	/*
	srand((unsigned)time(NULL));
	double u1 = rand()/(RAND_MAX + 1.0);
	printf("%lf\n", u1);
	double u2 = rand()/(RAND_MAX + 1.0);
	printf("%lf\n", u2);
	*/
	int count;
	if(k == 2) {
		count = 300;
	} else if(k == 4) {
		count = 150;
	} else if(k == 8) {
		count = 80;
	} else if(k == 16) {
		count = 40;
	} else if(k == 64) {
		count = 20;
	}
	srand((unsigned)time(NULL));
	for(int i = 0; i < k; i++) {
		for(int j = 0; j < 784; j++) {
			float u1 = rand()/(RAND_MAX + 1.0);
			//printf("%lf\n", u1);
			float u2 = rand()/(RAND_MAX + 1.0);
			//printf("%lf\n", u2);
			if(u1 == 0||u2 == 0) {
				j--;
				continue;
			}
			float z = pow(((-2)*log(u1)), 0.5)*cos(2*PI*u2);		
			matrix[i][j] = z;
		}
	}
	for(int i = 0; i < k; i++) {
		if(i % count == 0) {
			if(fout != NULL) {
				fclose(fout);
			}
			memset(s, 0, sizeof(s));
			sprintf(s, "%s%d", filePath, gaussian_matrix_file_num);
			fout = fopen(s, "wb");
			gaussian_matrix_file_num++;
		}
		for(int j = 0; j < 784; j++) {
			fwrite(&matrix[i][j], sizeof(float), 1, fout);
		}
	}
	fclose(fout);
/*
	for(int i = 0; i < 784; i++) {
		printf("%lf ", matrix[20][i]);
	}
	printf("\n");
	*/
}

/*
*storage the index if the value is one
*7840*78
*/
void produce_fly_projection_matrix() {
	int matrix[M][78];
	for(int i = 0;  i < M; i++) {
		for(int m = 0; m < 0; m++) {
			matrix[i][m] = -1;
		}
	}
	FILE *fout = NULL;
	int num_of_one = (int)(0.1*784);
	int current_num = 0;
	int n = 0;
	int j = 0;
	char s[40];
	/*
	srand((unsigned)time(NULL));
	double u1 = rand()/(RAND_MAX + 1.0);
	printf("%lf\n", u1);
	double u2 = rand()/(RAND_MAX + 1.0);
	printf("%lf\n", u2);
	*/
	//rand index is not repeat
	srand((unsigned)time(NULL));
	for(int l = 0; l < M; l++) {
		for(int i = 0; i < num_of_one;) {
			int rand_index = rand()%(784);
			for(j = 0; j < i; j++) {
				if(matrix[l][j] == rand_index) {
					break;
				}
			}
			if(j == i) {
				matrix[l][i] = rand_index;
				i++;
			}
		}
	}
	//Sorting in  Ascending
	for(int i = 0; i < M; i++) {
		for(int p = 0; p < 77; p++) {
			for(int q = p+1; q < 78; q++) {
				if(matrix[i][p] > matrix[i][q]) {
					int temp = matrix[i][p];
					matrix[i][p] = matrix[i][q];
					matrix[i][q] = temp;
				}
			}
		}
	}

	for(int i = 0; i < M; i++) {
		if(i % 210 == 0) {
			if(fout != NULL) {
				fclose(fout);
			}
			memset(s, 0, sizeof(s));
			sprintf(s, "%s%d", "matrix/fly_matrix/", fly_matrix_file_num);
			fout = fopen(s, "wb");
			fly_matrix_file_num++;
		}
		for(int p = 0; p < 78; p++) {
			fwrite(&matrix[i][p], sizeof(int), 1, fout);
		}
	}
	if(fout)
		fclose(fout);
/*
	for(int i = 0; i < 78; i++) {
		printf("%d ", matrix[210][i]);
	}
	printf("\n");
	*/
}

/*
*get the gaussian projection matrix
*
*/
float** get_gaussian_projection_matrix(int k, const char* filePath) {
	int max_element;
	if(k == 2) {
		max_element = 300;
	} else if(k == 4) {
		max_element = 150;
	} else if(k == 8) {
		max_element = 80;
	} else if(k == 16) {
		max_element = 40;
	} else if(k == 32) {
		max_element = 20;
	}else if(k == 64) {
		max_element = 20;
	}
	FILE* fout = NULL;
	char s[40];
	gaussian_matrix_file_num = 0;
	float **matrix = new float*[k];
	for(int i = 0; i < k; i++) {
		matrix[i] = new float[784];
	}
	for(int i = 0; i < k; i++) {
		if(i % max_element == 0) {
			if(fout != NULL) {
				fclose(fout);
			}
			memset(s, 0, sizeof(s));
			sprintf(s, "%s%d", filePath, gaussian_matrix_file_num);
			fout = fopen(s, "rb");
			gaussian_matrix_file_num++;
		}
		for(int j = 0; j < 784; j++) {
			fread(&matrix[i][j], sizeof(float), 1, fout);
		}
	}
	//for(int i = 0; i < 784; i++) {
	//	printf("%lf ", matrix[0][i]);
	//}
	//printf("\n");
	if(fout)
		fclose(fout);
	return matrix;
}

/*
*get the fly projection matrix
*
*/
int** get_fly_projection_matrix() {
	FILE* fout = NULL;
	char s[40];
	fly_matrix_file_num = 0;
	int **matrix = new int*[10*784];
	for(int i = 0; i < 10*784; i++) {
		matrix[i] = new int[78];
	}
	for(int i = 0; i < 10*784; i++) {
		if(i % 210 == 0) {
			if(fout != NULL) {
				fclose(fout);
			}
			memset(s, 0, sizeof(s));
			sprintf(s, "%s%d", "matrix/fly_matrix/", fly_matrix_file_num);
			fout = fopen(s, "rb");
			fly_matrix_file_num++;
		}
		for(int j = 0; j < 78; j++) {
			fread(&matrix[i][j], sizeof(int), 1, fout);
		}
	}
	for(int i = 0; i < 78; i++) {
		printf("%d ", matrix[1][i]);
	}
	printf("\n");
	if(fout)
		fclose(fout);
	return matrix;
}


/*
*make guassian projection
*
*/

void produce_gaussian_projection(int k1, const char* filePath) {
	fileNum = 0;

	FILE* fout = NULL;
	short int temp = 0;
	char s[40];
	bool arrayToTagSlot = true;
	char s1[40];
	memset(s1, 0, sizeof(s));
	sprintf(s1, "%s%d%s", "matrix/gaussian_matrix_", k1, "/");
	
	float **matrix = new float*[k1];
	for(int i = 0; i < k1; i++) {
		matrix[i] = new float[784];
	}
	matrix = get_gaussian_projection_matrix(k1, s1);
	int max_element;
	if(k1 == 2) {
		max_element = 5000;
	} else if(k1 == 4) {
		max_element = 3000;
	} else if(k1 == 8) {
		max_element = 1700;
	} else if(k1 == 16) {
		max_element = 900;
	} else if(k1 == 32) {
		max_element = 492;
	}
	else if(k1 == 64) {
		max_element = 250;
	}
	

	// process every vector
	for (int i = 0; i < 60000; i++) {
		// Each page's header info
		if (i % max_element == 0) {
			if (fout != NULL)
				fclose(fout);
			memset(s, 0, sizeof(s));
			sprintf(s, "%s%d", filePath, fileNum);
			fout = fopen(s, "wb");
			// page id
			fwrite(&fileNum, sizeof(int), 1, fout);
			// slotSize = 32 + 32 * 32 =  = 1056bits = 132bytes
			int slotSize = 4*(k1+1);
			fwrite(&slotSize, sizeof(int), 1, fout);
			// number of slot
			int numOfSlot = max_element;
			fwrite(&numOfSlot, sizeof(int), 1, fout);

			// array to tag every slot
			// in case the last file is not full
			if (fileNum == (60000 / max_element)) {
				for (int k = 0; k < max_element; k++) {
					if (k < (60000 % max_element)) {
						fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
					}
					else {
						bool tagSlotNotUse = false;
						fwrite(&tagSlotNotUse, sizeof(bool), 1, fout);
					}
				}
			}
			else {
				for (int k = 0; k < max_element; k++) {
					fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
				}
			}
			//data
			fileNum++;
		}
		// write id
		fwrite(&i, sizeof(int), 1, fout);
		int* vector = get_vector_by_id(i, "preprocess_data_mnist/");
		
		// cout << endl;
		float arr[784] = {0};
		for (int j = 0; j < 784; ++j) {
			arr[j] = (float)(vector[j]);
			//printf("%lf ", arr[j]);
			//printf("\n");
		}
		float sum = 0;
		for(int p = 0; p < k1; p++) {
			sum = 0.0;
			for(int q = 0; q < 784; q++) {
				sum += (matrix[p][q]*arr[q]);
			}
			fwrite(&sum, sizeof(float), 1, fout);
		}
		// cout << endl;
		free(vector);
	}
	for(int i = 0; i < k1; i++) {
		delete[] matrix[i];
	}
	delete[] matrix;
}

/*
*make the fly projection
*
*/
void produce_fly_projection() {
	fileNum = 0;

	FILE* fout = NULL;
	short int temp = 0;
	char s[40];
	bool arrayToTagSlot = true;
	//get fly projection matrix
	int **matrix = new int*[10*784];
	for(int i = 0; i < 784*10; i++) {
		matrix[i] = new int[78];
	}
	matrix = get_fly_projection_matrix();
	for(int i = 0; i < 78; i++) {
		printf("%d ", matrix[0][i]);
	}
	printf("\n");
	for(int i = 0; i < 78; i++) {
		printf("%d ", matrix[1][i]);
	}
	printf("\n");
	for(int i = 0; i < 78; i++) {
		printf("%d ", matrix[2][i]);
	}
	printf("\n");

	// process every vector
	for (int i = 0; i < 60000; i++) {
		// Each page's header info
		if (i % 2 == 0) {
			if (fout != NULL)
				fclose(fout);
			memset(s, 0, sizeof(s));
			sprintf(s, "%s%d", "fly_data_mnist/", fileNum);
			fout = fopen(s, "wb");
			// page id
			fwrite(&fileNum, sizeof(int), 1, fout);
			// slotSize = 32 + 32 * 784 * 10 =  250912bits = 31364bytes
			int slotSize = 31364;
			fwrite(&slotSize, sizeof(int), 1, fout);
			// number of slot
			int numOfSlot = 2;
			fwrite(&numOfSlot, sizeof(int), 1, fout);

			// array to tag every slot
			// in case the last file is not full
			if (fileNum == (60000 / 2)) {
				for (int k = 0; k < 2; k++) {
					if (k < (60000 % 2)) {
						fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
					}
					else {
						bool tagSlotNotUse = false;
						fwrite(&tagSlotNotUse, sizeof(bool), 1, fout);
					}
				}
			}
			else {
				for (int k = 0; k < 2; k++) {
					fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
				}
			}
			//data
			fileNum++;
		}
		// write id
		fwrite(&i, sizeof(int), 1, fout);
		int* vector = get_vector_by_id(i, "preprocess_data_mnist/");
		
		
		int sum = 0;
		for(int p = 0; p < 784*10; p++) {
			sum = 0;
			for(int q = 0; q < 78; q++) {
				sum += vector[matrix[p][q]];
			}
			fwrite(&sum, sizeof(int), 1, fout);
		}
		// cout << endl;
		free(vector);
	}

	for(int i = 0; i < 10*784; i++) {
		delete[] matrix[i];
	}
	delete[] matrix;
	if(fout)
		fclose(fout);
}



/*
*random process for fly_data_mnist/
*
*/
void random_process(int k, const char* filepath) {
	//produce k random index k=32
	int *random_arr = (int*)malloc(k * sizeof(int));
	for(int i = 0; i < k; i++) {
		random_arr[i] = -1;
	}
	int j = 0;
	srand((unsigned)time(NULL));
	for(int i = 0; i < k;) {
		int rand_index = rand()%(784*10);
		for(j = 0; j < i; j++) {
			if(random_arr[j] == rand_index) {
				break;
			}
		}
		if(j == i) {
			random_arr[i] = rand_index;
			i++;
		}
	}

	fileNum = 0;
	FILE* fout = NULL;
	char s[40];
	bool arrayToTagSlot = true;
	int count;
	if(k == 2) {
		count = 5000;
	} else if(k == 4) {
		count = 3000;
	} else if(k == 8) {
		count = 1700;
	} else if(k == 16) {
		count = 900;
	} else if(k == 32) {
		count = 492;
	} else if(k == 64) {
		count = 250;
	}
	// process every vector
	for (int i = 0; i < 60000; i++) {
		// Each page's header info
		if (i % count == 0) {
			if (fout != NULL)
				fclose(fout);
			memset(s, 0, sizeof(s));
			sprintf(s, "%s%d", filepath, fileNum);
			fout = fopen(s, "wb");
			// page id
			fwrite(&fileNum, sizeof(int), 1, fout);
			// slotSize = 32 + 32 * 32 = 1056bits = 132bytes
			int slotSize = 4*(k+1);
			fwrite(&slotSize, sizeof(int), 1, fout);
			// number of slot
			int numOfSlot = count;
			fwrite(&numOfSlot, sizeof(int), 1, fout);

			// array to tag every slot
			// in case the last file is not full
			if (fileNum == (60000 / count)) {
				for (int k = 0; k < count; k++) {
					if (k < (60000 % count)) {
						fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
					}
					else {
						bool tagSlotNotUse = false;
						fwrite(&tagSlotNotUse, sizeof(bool), 1, fout);
					}
				}
			}
			else {
				for (int k = 0; k < count; k++) {
					fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
				}
			}
			//data
			fileNum++;
		}
		// write id
		fwrite(&i, sizeof(int), 1, fout);
		int* vector = get_vector_by_id_in_all_fly_file(i, "fly_data_mnist/");
		
		int sum = 0;
		for(int p = 0; p < k; p++) {
			fwrite(&vector[random_arr[p]], sizeof(int), 1, fout);
		}
		// cout << endl;
		free(vector);
	}

	if(fout)
		fclose(fout);
}

/*
*WTA process for fly_data_mnist/
*
*/
void WTA_process(int k, const char* filepath) {
	int *random_arr = (int*)malloc(k * sizeof(int));
	fileNum = 0;
	FILE* fout = NULL;
	char s[40];
	bool arrayToTagSlot = true;
	int count;
	if(k == 2) {
		count = 3000;
	} else if(k == 4) {
		count = 1700;
	} else if(k == 8) {
		count = 930;
	} else if(k == 16) {
		count = 480;
	} else if(k == 32) {
		count = 251;
	}else if(k == 64) {
		count = 120;
	}
	// process every vector
	for (int i = 0; i < 60000; i++) {
		// Each page's header info
		if (i % count == 0) {
			if (fout != NULL)
				fclose(fout);
			memset(s, 0, sizeof(s));
			sprintf(s, "%s%d", filepath, fileNum);
			fout = fopen(s, "wb");
			// page id
			fwrite(&fileNum, sizeof(int), 1, fout);
			// slotSize = 32 + 32 * 64 = 2080bits = 260bytes
			int slotSize = 4*(k+1);
			fwrite(&slotSize, sizeof(int), 1, fout);
			// number of slot
			int numOfSlot = count;
			fwrite(&numOfSlot, sizeof(int), 1, fout);

			// array to tag every slot
			// in case the last file is not full
			if (fileNum == (60000 / count)) {
				for (int k = 0; k < count; k++) {
					if (k < (60000 % count)) {
						fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
					}
					else {
						bool tagSlotNotUse = false;
						fwrite(&tagSlotNotUse, sizeof(bool), 1, fout);
					}
				}
			}
			else {
				for (int k = 0; k < count; k++) {
					fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
				}
			}
			//data
			fileNum++;
		}
		// write id
		fwrite(&i, sizeof(int), 1, fout);
		int* vector = get_vector_by_id_in_all_fly_file(i, "fly_data_mnist/");
		
		int *MAX_K = (int*)malloc(k * sizeof(int));
		for(int i = 0; i < k; i++) {
			MAX_K[i] = -1;
		}

		for(int p = 0; p < 784*10; p++) {
			if(p < k) {
				MAX_K[p] = p;
			} else {
				for(int m1 = 0; m1 < k-1; m1++) {
					for(int m2 = m1+1; m2 < k; m2++) {
						if(vector[MAX_K[m1]] < vector[MAX_K[m2]]) {
							int temp = MAX_K[m1];
							MAX_K[m1] = MAX_K[m2];
							MAX_K[m2] = temp;
						}
					}
				}
				if(vector[MAX_K[k-1]] < vector[p]) {
					MAX_K[k-1] = p;
				}
			}
		}

		for(int m1 = 0; m1 < k-1; m1++) {
			for(int m2 = m1+1; m2 < k; m2++) {
				if(vector[MAX_K[m1]] < vector[MAX_K[m2]]) {
					int temp = MAX_K[m1];
					MAX_K[m1] = MAX_K[m2];
					MAX_K[m2] = temp;
				}
			}
		}

		for(int m1 = 0; m1 < k; m1++) {
			fwrite(&MAX_K[m1], sizeof(int), 1, fout);
			fwrite(&vector[MAX_K[m1]], sizeof(int), 1, fout);
		}
		// cout << endl;
		free(vector);
	}

	if(fout)
		fclose(fout);
}

/*
*binary process for fly_data_mnist/
*
*/
void binary_process(int k, const char* filepath) {
	int *random_arr = (int*)malloc(k * sizeof(int));
	fileNum = 0;
	FILE* fout = NULL;
	char s[40];
	bool arrayToTagSlot = true;
	int count;
	if(k == 2) {
		count = 3000;
	} else if(k == 4) {
		count = 1700;
	} else if(k == 8) {
		count = 900;
	} else if(k == 16) {
		count = 470;
	} else if(k == 32) {
		count = 492;
	} 
	else if(k == 64) {
		count = 120;
	}
	// process every vector
	for (int i = 0; i < 60000; i++) {
		// Each page's header info
		if (i % count == 0) {
			if (fout != NULL)
				fclose(fout);
			memset(s, 0, sizeof(s));
			sprintf(s, "%s%d", filepath, fileNum);
			fout = fopen(s, "wb");
			// page id
			fwrite(&fileNum, sizeof(int), 1, fout);
			// slotSize = 32 + 32 * 64 = 2080bits = 260bytes
			int slotSize = 4*(k+1);
			fwrite(&slotSize, sizeof(int), 1, fout);
			// number of slot
			int numOfSlot = count;
			fwrite(&numOfSlot, sizeof(int), 1, fout);

			// array to tag every slot
			// in case the last file is not full
			if (fileNum == (60000 / count)) {
				for (int k = 0; k < count; k++) {
					if (k < (60000 % count)) {
						fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
					}
					else {
						bool tagSlotNotUse = false;
						fwrite(&tagSlotNotUse, sizeof(bool), 1, fout);
					}
				}
			}
			else {
				for (int k = 0; k < count; k++) {
					fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
				}
			}
			//data
			fileNum++;
		}
		// write id
		fwrite(&i, sizeof(int), 1, fout);
		int* vector = get_vector_by_id_in_all_fly_file(i, "fly_data_mnist/");
		
		int *MAX_K = (int*)malloc(k * sizeof(int));
		for(int i = 0; i < k; i++) {
			MAX_K[i] = -1;
		}

		for(int p = 0; p < 784*10; p++) {
			if(p < k) {
				MAX_K[p] = p;
			} else {
				for(int m1 = 0; m1 < k-1; m1++) {
					for(int m2 = m1+1; m2 < k; m2++) {
						if(vector[MAX_K[m1]] < vector[MAX_K[m2]]) {
							int temp = MAX_K[m1];
							MAX_K[m1] = MAX_K[m2];
							MAX_K[m2] = temp;
						}
					}
				}
				if(vector[MAX_K[k-1]] < vector[p]) {
					MAX_K[k-1] = p;
				}
			}
		}

		for(int m1 = 0; m1 < k-1; m1++) {
			for(int m2 = m1+1; m2 < k; m2++) {
				if(vector[MAX_K[m1]] < vector[MAX_K[m2]]) {
					int temp = MAX_K[m1];
					MAX_K[m1] = MAX_K[m2];
					MAX_K[m2] = temp;
				}
			}
		}

		for(int m1 = 0; m1 < k; m1++) {
			fwrite(&MAX_K[m1], sizeof(int), 1, fout);
			fwrite(&vector[MAX_K[m1]], sizeof(int), 1, fout);
		}
		// cout << endl;
		free(vector);
	}

	if(fout)
		fclose(fout);
}


/*
*test for different function
*
*/
void test_guassian_projection_matrix() {
	FILE *fin = fopen("matrix/gaussian_matrix/0", "rb");
	float temp;
	
	for(int i = 0; i < 784; i++) {
		fread(&temp, sizeof(float), 1, fin);
		printf("%lf ", temp);
	}
	printf("\n");
	fclose(fin);
}
void test_fly_projection_matrix() {
	FILE *fin = fopen("matrix/fly_matrix/0", "rb");
	int temp;
	
	for(int i = 0; i < 78; i++) {
		fread(&temp, sizeof(int), 1, fin);
		printf("%d ", temp);
	}
	printf("\n");
	fclose(fin);
}

void test_matrix_gaussian() {
	FILE *fin = fopen("gaussian_data_mnist_2/1", "rb");
	int tip;
	fread(&tip, sizeof(int), 1, fin);
	printf("%d\n", tip);
	int slotSize;
	fread(&slotSize, sizeof(int), 1, fin);
	printf("%d\n", slotSize);
	int numOfSlot = 0;
	fread(&numOfSlot, sizeof(int), 1, fin);
	printf("%d\n", numOfSlot);
	bool b;
	for (int i = 0; i < numOfSlot; i++) {
		fread(&b, sizeof(bool), 1, fin);
		printf("%d ", b);
	}
	float temp;
	printf("\n");
	
	int id;
	fread(&id, sizeof(int), 1, fin);
	printf("%d\n", id);
	for(int i = 0; i < 2; i++) {
		fread(&temp, sizeof(float), 1, fin);
		printf("%lf ", temp);
	}
	printf("\n");
	fclose(fin);
}

void test_matrix_fly() {
	FILE *fin = fopen("fly_data_mnist/0", "rb");
	int tip;
	fread(&tip, sizeof(int), 1, fin);
	printf("%d\n", tip);
	int slotSize;
	fread(&slotSize, sizeof(int), 1, fin);
	printf("%d\n", slotSize);
	int numOfSlot = 0;
	fread(&numOfSlot, sizeof(int), 1, fin);
	printf("%d\n", numOfSlot);
	bool b;
	for (int i = 0; i < numOfSlot; i++) {
		fread(&b, sizeof(bool), 1, fin);
		printf("%d ", b);
	}
	int temp;
	printf("\n");
	
	int id;
	fread(&id, sizeof(int), 1, fin);
	printf("%d\n", id);
	for(int i = 0; i < 784*10; i++) {
		fread(&temp, sizeof(int), 1, fin);
		printf("%d ", temp);
	}
	printf("\n");

	fclose(fin);
}

void test_random_fly() {
	FILE *fin = fopen("fly_random_data_mnist_64/10", "rb");
	int tip;
	fread(&tip, sizeof(int), 1, fin);
	printf("%d\n", tip);
	int slotSize;
	fread(&slotSize, sizeof(int), 1, fin);
	printf("%d\n", slotSize);
	int numOfSlot = 0;
	fread(&numOfSlot, sizeof(int), 1, fin);
	printf("%d\n", numOfSlot);
	bool b;
	for (int i = 0; i < numOfSlot; i++) {
		fread(&b, sizeof(bool), 1, fin);
		printf("%d ", b);
	}
	int temp;
	printf("\n");

	int id;
	fread(&id, sizeof(int), 1, fin);
	printf("%d\n", id);
	for(int i = 0; i < 250; i++) {
		fread(&temp, sizeof(int), 1, fin);
		printf("%d ", temp);
	}
	printf("\n");

	fclose(fin);
}

void WTA_test() {
	FILE *fin = fopen("fly_WTA_data_mnist/0", "rb");
	int tip;
	fread(&tip, sizeof(int), 1, fin);
	printf("%d\n", tip);
	int slotSize;
	fread(&slotSize, sizeof(int), 1, fin);
	printf("%d\n", slotSize);
	int numOfSlot = 0;
	fread(&numOfSlot, sizeof(int), 1, fin);
	printf("%d\n", numOfSlot);
	bool b;
	for (int i = 0; i < numOfSlot; i++) {
		fread(&b, sizeof(bool), 1, fin);
		printf("%d ", b);
	}
	int index, value;
	printf("\n");
	
	int id;
	fread(&id, sizeof(int), 1, fin);
	printf("%d\n", id);
	for(int i = 0; i < 32; i++) {
		fread(&index, sizeof(int), 1, fin);
		fread(&value, sizeof(int), 1, fin);
		printf("(%d, %d) ", index, value);
	}
	printf("\n");

	fclose(fin);
}

void binary_test() {
	FILE *fin = fopen("fly_binary_data_mnist/0", "rb");
	int tip;
	fread(&tip, sizeof(int), 1, fin);
	printf("%d\n", tip);
	int slotSize;
	fread(&slotSize, sizeof(int), 1, fin);
	printf("%d\n", slotSize);
	int numOfSlot = 0;
	fread(&numOfSlot, sizeof(int), 1, fin);
	printf("%d\n", numOfSlot);
	bool b;
	for (int i = 0; i < numOfSlot; i++) {
		fread(&b, sizeof(bool), 1, fin);
		printf("%d ", b);
	}
	int index;
	printf("\n");
	
	int id;
	fread(&id, sizeof(int), 1, fin);
	printf("%d\n", id);
	for(int i = 0; i < 32; i++) {
		fread(&index, sizeof(int), 1, fin);
		printf("%d ", index);
	}
	printf("\n");

	fclose(fin);
}


#endif // FILEIO_H