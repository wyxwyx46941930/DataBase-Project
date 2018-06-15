#ifndef FILEIO_H
#define FILEIO_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "try.h"
#define PI 3.1415926
#define M (10*784)
 

void fileIO_origin_data_in_mnist(const char* infile, const int n, const int d);
void fileIO_origin_data_in_glove(const char* infile, const int n, const int d);
void fileIO_preProcess_data_in_mnist(const int n, const int d);
void fileIO_preProcess_data_in_glove(const int n, const int d);
void test_mnist();
void test_glove();
void test_preprocess_mnist();
void test_preprocess_glove();

int fileNum = 0;
int fly_matrix_file_num = 0;
int gaussian_matrix_file_num = 0;
/**
* Binary read file mnist and store data using page-by-page storage
* @param infile: filePath
* @param n: the number of vector in file
* @param d: the dimension of vector
*/
void fileIO_origin_data_in_mnist(const char* infile, const int n, const int d) {
	fileNum = 0;
	FILE* fin = NULL;
	FILE* fout = NULL;

	// open source data
	fin = fopen("mnist", "r");
	// if open failed then return directly
	if (fin == NULL) return;

	float a = 0;
	int ti = 0;
	unsigned char temp = 0x00;
	char s[40];
	bool arrayToTagSlot = true;

	// read all vector
	for (int i = 0; i < n; i++) {
		// Each page's header info
		if (i % 83 == 0) {
			if (fout != NULL)
				fclose(fout);
			memset(s, 0, sizeof(s));
			sprintf(s, "%s%d", "origin_data_mnist/", fileNum);
			fout = fopen(s, "wb");
			// page id
			fwrite(&fileNum, sizeof(int), 1, fout);
			// slotsize = 32 + 8 * 784 = 6304bit = 788bit
			int slotSize = 788;
			fwrite(&slotSize, sizeof(int), 1, fout);
			// number of slot
			int numOfSlot = 83;
			fwrite(&numOfSlot, sizeof(int), 1, fout);

			// array to tag every slot
			// in case the last file is not full
			if (fileNum == (n / 83)) {
				for (int k = 0; k < 83; k++) {
					if (k < (n % 83)) {
						fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
					}
					else {
						bool tagSlotNotUse = false;
						fwrite(&tagSlotNotUse, sizeof(bool), 1, fout);
					}
				}
			}
			else {
				for (int k = 0; k < 83; k++) {
					fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
				}
			}

			//data
			fileNum++;
		}

		// read vectorId and write it as ti
		fscanf(fin, "%d", &ti);
		//printf("%d\n", ti);
		fwrite(&ti, sizeof(int), 1, fout);


		for (int j = 0; j < d; j++)
		{
			fscanf(fin, "%f", &a);
			temp = (((int)a) & 0x000000ff);
			fwrite(&temp, sizeof(unsigned char), 1, fout);
		}

	}

	fclose(fin);
}

/**
* Binary read file glove and store data using page-by-page storage
* @param infile: filePath
* @param n: the number of vector in file
* @param d: the dimension of vector
*/
void fileIO_origin_data_in_glove(const char* infile, const int n, const int d) {
	fileNum = 0;
	FILE* fin = NULL;
	FILE* fout = NULL;

	// open source data
	fin = fopen(infile, "r");
	if (fin == NULL) return;

	float td = 0;
	int ti = 0;
	char s[40];
	bool arrayToTagSlot = true;

	// read all vector
	for (int i = 0; i < n; i++) {
		// Each page's header info
		if (i % 54 == 0) {
			if (fout != NULL)
				fclose(fout);
			memset(s, 0, sizeof(s));
			sprintf(s, "%s%d", "origin_data_glove/", fileNum);
			fout = fopen(s, "wb");
			//page id
			fwrite(&fileNum, sizeof(int), 1, fout);
			//slotsize = 32 + 32 * 300 = 9632bit
			int slotSize = 1204;
			fwrite(&slotSize, sizeof(int), 1, fout);
			//number of slot
			int numOfSlot = 54;
			fwrite(&numOfSlot, sizeof(int), 1, fout);
			//array to tag every slot
			// in case the last file is not full
			if (fileNum == (n / 54)) {
				for (int k = 0; k < 54; k++) {
					if (k < (n % 54)) {
						fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
					}
					else {
						bool tagNotUse = false;
						fwrite(&tagNotUse, sizeof(bool), 1, fout);
					}

				}
			}
			else {
				for (int k = 0; k < 54; k++) {
					fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
				}
			}
			//data
			fileNum++;
		}

		// read vectorId and write it as ti
		fscanf(fin, "%d", &ti);
		fwrite(&ti, sizeof(int), 1, fout);


		for (int j = 0; j < d; ++j)
		{
			fscanf(fin, "%f", &td);
			fwrite(&td, sizeof(float), 1, fout);
		}

	}

	fclose(fin);
}

void fileIO_preProcess_data_in_mnist(const int n, const int d) {
	fileNum = 0;

	FILE* fout = NULL;
	short int temp = 0;
	char s[40];
	bool arrayToTagSlot = true;

	// process every vector
	for (int i = 0; i < n; i++) {
		// Each page's header info
		if (i % 41 == 0) {
			if (fout != NULL)
				fclose(fout);
			memset(s, 0, sizeof(s));
			sprintf(s, "%s%d", "preprocess_data_mnist/", fileNum);
			fout = fopen(s, "wb");
			// page id
			fwrite(&fileNum, sizeof(int), 1, fout);
			// slotSize = 32 + 16 * 784 = 12576bit
			int slotSize = 1572;
			fwrite(&slotSize, sizeof(int), 1, fout);
			// number of slot
			int numOfSlot = 41;
			fwrite(&numOfSlot, sizeof(int), 1, fout);

			// array to tag every slot
			// in case the last file is not full
			if (fileNum == (n / 41)) {
				for (int k = 0; k < 41; k++) {
					if (k < (n % 41)) {
						fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
					}
					else {
						bool tagSlotNotUse = false;
						fwrite(&tagSlotNotUse, sizeof(bool), 1, fout);
					}
				}
			}
			else {
				for (int k = 0; k < 41; k++) {
					fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
				}
			}
			//data
			fileNum++;
		}
		// write id
		fwrite(&i, sizeof(int), 1, fout);
		int* vector = get_vector_by_id(i, "origin_data_mnist/");

		// process: total / d = 100 => total = 100 * d
		int total = 0;
		double divFactor = 0;
		for (int j = 0; j < d; ++j) {
			total += vector[j];
			// cout << vector[j] << " ";
		}

		divFactor = (double)total / (100 * d);
		// cout << endl;
		for (int j = 0; j < d; ++j) {
			vector[j] = vector[j] / divFactor;
			temp = vector[j] & 0x0000ffff;
			// cout << vector[j] << " ";
			fwrite(&temp, sizeof(short int), 1, fout);
		}
		// cout << endl;

		free(vector);
	}
}

void fileIO_preProcess_data_in_glove(const int n, const int d) {
	fileNum = 0;

	FILE* fout = NULL;
	float temp = 0;
	char s[40];
	bool arrayToTagSlot = true;

	// process every vector
	for (int i = 0; i < n; i++) {
		// Each page's header info
		if (i % 54 == 0) {
			if (fout != NULL)
				fclose(fout);
			memset(s, 0, sizeof(s));
			sprintf(s, "%s%d", "preprocess_data_glove/", fileNum);
			fout = fopen(s, "wb");
			// page id
			fwrite(&fileNum, sizeof(int), 1, fout);
			//slotsize = 32 + 32 * 300 = 9632bit = 1204byte
			int slotSize = 1204;
			fwrite(&slotSize, sizeof(int), 1, fout);
			// number of slot
			int numOfSlot = 54;
			fwrite(&numOfSlot, sizeof(int), 1, fout);

			// array to tag every slot
			// in case the last file is not full
			if (fileNum == (n / 54)) {
				for (int k = 0; k < 54; k++) {
					if (k < (n % 54)) {
						fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
					}
					else {
						bool tagSlotNotUse = false;
						fwrite(&tagSlotNotUse, sizeof(bool), 1, fout);
					}
				}
			}
			else {
				for (int k = 0; k < 54; k++) {
					fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
				}
			}
			//data
			fileNum++;
		}
		// write id
		fwrite(&i, sizeof(int), 1, fout);
		float* vector = get_vector_by_id_glove(i, "origin_data_glove/");

		// process: total / d = 100 => total = 100 * d
		float total = 0;
		float divFactor = 0;
		for (int j = 0; j < d; ++j) {
			total += vector[j];
			// cout << vector[j] << " ";
		}

		divFactor = total / (100 * d);
		// cout << endl;
		for (int j = 0; j < d; ++j) {
			vector[j] = vector[j] / divFactor;
			temp = vector[j];
			// cout << vector[j] << " ";
			fwrite(&temp, sizeof(float), 1, fout);
		}
		// cout << endl;

		free(vector);
	}

}


/**
 * test mnist IO, out put the first vector(with id) in page 2
 */
void test_mnist() {
	FILE *fin = fopen("origin_data_mnist/0", "rb");
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
	for (int i = 0; i < 83; i++) {
		fread(&b, sizeof(bool), 1, fin);
		printf("%d ", b);
	}
	printf("\n");
	unsigned char temp;
	int ti;
	fread(&ti, sizeof(int), 1, fin);
	printf("%d ", ti);
	for (int i = 0; i < 784; i++) {
		fread(&temp, sizeof(unsigned char), 1, fin);
		int m = temp & 0x000000ff;
		printf("%d ", m);
	}
	printf("\n");
	fclose(fin);
}

/**
 * test glove IO, out put the first vector(with id) in page 0
 */
void test_glove() {
	FILE *fin = fopen("origin_data_glove/0", "rb");
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
	for (int i = 0; i < 54; i++) {
		fread(&b, sizeof(bool), 1, fin);
		printf("%d ", b);
	}
	printf("\n");
	int ti;
	float temp;
	fread(&ti, sizeof(int), 1, fin);
	printf("%d ", ti);
	for (int i = 0; i < 300; i++) {
		fread(&temp, sizeof(float), 1, fin);
		printf("%f ", temp);
	}
	printf("\n");
	fclose(fin);
}

/**
 * test preprocess mnist io, out put the first vector(with id) in page 0
 */
void test_preprocess_mnist() {
	FILE *fin = fopen("preprocess_data_mnist/0", "rb");
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
	printf("\n");
	short int temp;
	int ti;
	fread(&ti, sizeof(int), 1, fin);
	printf("%d ", ti);
	for (int i = 0; i < 784; i++) {
		fread(&temp, sizeof(short int), 1, fin);
		printf("%d ", temp);
	}
	printf("\n");
	fclose(fin);
}

/**
 * test preprocess glove IO, out put the first vector(with id) in page 0
 */
void test_preprocess_glove() {
	FILE *fin = fopen("preprocess_data_glove/0", "rb");
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
	for (int i = 0; i < 54; i++) {
		fread(&b, sizeof(bool), 1, fin);
		printf("%d ", b);
	}
	printf("\n");
	int ti;
	float temp;
	fread(&ti, sizeof(int), 1, fin);
	printf("%d ", ti);
	for (int i = 0; i < 300; i++) {
		fread(&temp, sizeof(float), 1, fin);
		printf("%f ", temp);
	}
	printf("\n");
	fclose(fin);
}



#endif // FILEIO_H