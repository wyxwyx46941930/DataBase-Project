#ifndef TRY_H
#define TRY_H

#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <sstream>
using namespace std;

#define SIZEOFPAGE 65536
#define MNISI_FILE_NUM 723
#define GLOVE_FILE_NUM 40667
#define PRE_PROCESS_MNIST_FILE_NUM 1464
#define MNIST_SLOT_NUM 83
#define GLOVE_SLOT_NUM 54
#define PRE_PROCESS_MNIST_SLOT_NUM 41
#define MNIST_VEC_NUM 60000
#define GLOVE_VEC_NUM 2196017
#define MNIST_VEC_DIM 784
#define GLOVE_VEC_DIM 300

// 一个内存缓存区，里面保存了50个帧，也即50个页
char all_pages[50][SIZEOFPAGE];
//in array dirty, 0 stands for not changed since read from disk
bool dirty[50];
//record the file index of the pages in all_pages
int file_index[50];
//empty:head_index == end_index, full:head_index == (end_index + 1) % 50
int head_index, end_index;
int pageCount;
FILE* file_ptr;


void init_pages() {
	for (int i = 0; i < 50; i++) {
		memset(all_pages[i], 0, SIZEOFPAGE * sizeof(char));
	}
	memset(dirty, 0, 50 * sizeof(bool));
	memset(file_index, -1, 50 * sizeof(int));
	head_index = end_index = pageCount = 0;
	file_ptr = NULL;
}

// 把指定帧的页覆盖地写回到磁盘中去
bool write_page(int array_index, const char* filePath) {

	// 如果写回，那么磁盘中的数据的一致性就得到保证了，设为0
	dirty[array_index] = 0;
	char name_temp[40];
	sprintf(name_temp, "%s%d", filePath, file_index[array_index]);
	file_ptr = NULL;
	file_ptr = fopen(name_temp, "w");
	if (file_ptr == NULL) {
		printf("Error when writing file :%s\n", name_temp);
		return 0;
	}
	fwrite(all_pages[array_index], 1, SIZEOFPAGE, file_ptr);
	fclose(file_ptr);
	file_ptr = NULL;
	return 1;
}

/**
* 把指定页读到指定帧
*/
bool read_disk_file(int page_index, int array_index, const char* filePath) {
	// 如果指定帧已经存在页，那么指定帧的页覆盖地写回到磁盘中去，如果写失败，直接返回。
	// 如果写成功的话，继续讲新的指定页写到这个指定帧以覆盖这个帧中原来的页
	if (file_index[array_index] != -1) {
		if (dirty[array_index] == 1 && !write_page(array_index, filePath)) {
			return 0;
		}
	}
	char name_temp[40];
	sprintf(name_temp, "%s%d", filePath, page_index);
	//cout << "name_temp: " << name_temp << endl;
	file_ptr = NULL;
	file_ptr = fopen(name_temp, "r");
	if (file_ptr == NULL) {
		printf("Error when reading file :%s\n", name_temp);
		return 0;
	}
	fread(all_pages[array_index], SIZEOFPAGE, 1, file_ptr);
	file_index[array_index] = page_index;
	fclose(file_ptr);
	file_ptr = NULL;
	return 1;
}

//if successfully read a page/it has existed in the memory, return its index in all_pages
//if failed, return -1
/**
* 把指定页写入内存，写成功这返回该页对应的帧，否则返回-1
*/
int get_a_page(int page_index, const char* filePath) {
	// 检查指定页是否已经在内存中，若在，返回对应内存中的帧
	bool exist_in_memory_flag = 0;
	for (int i = 0; i < 50; i++) {
		if (file_index[i] == page_index) {
			exist_in_memory_flag = 1;
			return i;
		}
	}
	// 如果指定页不存在在内存中，则需要将指定页写入
	if (!exist_in_memory_flag) {
		//check if the array is full
		if (pageCount == 50) {
			//get the last one out(head_index)
			memset(all_pages[head_index], 0, SIZEOFPAGE * sizeof(char));
			//read the page from disk
			if (read_disk_file(page_index, head_index, filePath) == 0) {
				return -1;
			}
			int return_index = head_index;
			head_index = (head_index + 1) % 50;
			end_index = (end_index + 1) % 50;
			return return_index;
		}
		else {
			//not full
			// 如果非空，把指定页写到 end_index 对应的帧中，写失败返回-1，否则返回 end_index
			if (read_disk_file(page_index, end_index, filePath) == 0) {
				return -1;
			}
			++pageCount;
			int return_index = end_index;
			// end_index 对应的帧已经被写入页，往前递增
			end_index = (end_index + 1) % 50;
			return return_index;
		}
	}
}

int* get_vector_by_id(int id, const char* filePath) {
	int *return_array = (int*)malloc(MNIST_VEC_DIM * sizeof(int));
	int temp_page_index, temp_slot_index;
	if (filePath == "origin_data_mnist/") {
		temp_page_index = id / MNIST_SLOT_NUM;
		temp_slot_index = id % MNIST_SLOT_NUM;
	}
	else if (filePath == "preprocess_data_mnist/") {
		temp_page_index = id / PRE_PROCESS_MNIST_SLOT_NUM;
		temp_slot_index = id % PRE_PROCESS_MNIST_SLOT_NUM;
	}
	
	//cout << "temp_page_index: " << temp_page_index << " temp_slot_index: " << temp_slot_index << endl;
	int real_array_index = get_a_page(temp_page_index, filePath);
	//cout << "real_array_index: " << real_array_index << endl;
	if (real_array_index == -1) {
		return NULL;
	}
	if (filePath == "origin_data_mnist/") {
		int id_check = (all_pages[real_array_index][MNIST_SLOT_NUM + 12 + ((temp_slot_index) * (MNIST_VEC_DIM + 4))] & 0xff) +
			((all_pages[real_array_index][MNIST_SLOT_NUM + 12 + ((temp_slot_index) * (MNIST_VEC_DIM + 4)) + 1] & 0xff) << 8) +
			((all_pages[real_array_index][MNIST_SLOT_NUM + 12 + ((temp_slot_index) * (MNIST_VEC_DIM + 4)) + 2] & 0xff) << 16) +
			((all_pages[real_array_index][MNIST_SLOT_NUM + 12 + ((temp_slot_index) * (MNIST_VEC_DIM + 4)) + 3] & 0xff) << 24);
	}
	else if (filePath == "preprocess_data_mnist/") {
		
		int id_check = (all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((temp_slot_index) * (2 * MNIST_VEC_DIM + 4))] & 0xff) +
			((all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((temp_slot_index) * (2 * MNIST_VEC_DIM + 4)) + 1] & 0xff) << 8) +
			((all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((temp_slot_index) * (2 * MNIST_VEC_DIM + 4)) + 2] & 0xff) << 16) +
			((all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((temp_slot_index) * (2 * MNIST_VEC_DIM + 4)) + 3] & 0xff) << 24);
	}
	
	for (int i = 0; i < MNIST_VEC_DIM; i++) {
		
		if (filePath == "origin_data_mnist/") {
			return_array[i] = (int)all_pages[real_array_index][MNIST_SLOT_NUM + 12 + ((temp_slot_index) * (MNIST_VEC_DIM + 4)) + 4 + i] & 0x000000ff;
		}
		if (filePath == "preprocess_data_mnist/") {
			return_array[i] = (((int)all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((temp_slot_index) * (2 * MNIST_VEC_DIM + 4)) + 4 + 2 * i] & 0x000000ff))
				| (((int)all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((temp_slot_index) * (2 * MNIST_VEC_DIM + 4)) + 4 + 2 * i + 1] & 0x000000ff) << 8);
		}
	}

	return return_array;
}

float* get_vector_by_id_glove(int id, const char* filePath) {
	float *return_array = (float*)malloc(GLOVE_VEC_DIM * sizeof(float));
	int temp_page_index = id / GLOVE_SLOT_NUM;
	int temp_slot_index = id % GLOVE_SLOT_NUM;

	int real_array_index = get_a_page(temp_page_index, filePath);
	if (real_array_index == -1) {
		return NULL;
	}
	int id_check = (all_pages[real_array_index][GLOVE_SLOT_NUM + 12 + ((temp_slot_index) * (4 * GLOVE_VEC_DIM + 4))] & 0xff) +
		((all_pages[real_array_index][GLOVE_SLOT_NUM + 12 + ((temp_slot_index) * (4 * GLOVE_VEC_DIM + 4)) + 1] & 0xff) << 8) +
		((all_pages[real_array_index][GLOVE_SLOT_NUM + 12 + ((temp_slot_index) * (4 * GLOVE_VEC_DIM + 4)) + 2] & 0xff) << 16) +
		((all_pages[real_array_index][GLOVE_SLOT_NUM + 12 + ((temp_slot_index) * (4 * GLOVE_VEC_DIM + 4)) + 3] & 0xff) << 24);
	
	for (int i = 0; i < GLOVE_VEC_DIM; i++) {
		char floatBuf[4];
		floatBuf[0] = (all_pages[real_array_index][GLOVE_SLOT_NUM + 12 + ((temp_slot_index) * (4 * GLOVE_VEC_DIM + 4)) + 4 + 4 * i] & 0xff);
		floatBuf[1] = (all_pages[real_array_index][GLOVE_SLOT_NUM + 12 + ((temp_slot_index) * (4 * GLOVE_VEC_DIM + 4)) + 4 + 4 * i + 1] & 0xff);
		floatBuf[2] = (all_pages[real_array_index][GLOVE_SLOT_NUM + 12 + ((temp_slot_index) * (4 * GLOVE_VEC_DIM + 4)) + 4 + 4 * i + 2] & 0xff);
		floatBuf[3] = (all_pages[real_array_index][GLOVE_SLOT_NUM + 12 + ((temp_slot_index) * (4 * GLOVE_VEC_DIM + 4)) + 4 + 4 * i + 3] & 0xff);
		float* data = (float*)(&floatBuf);
		return_array[i] = *data;
	}

	return return_array;
}

/**
* 赋予指定id一个vector
* need many change
*/
bool set_vector_by_id(int id, int *new_vector, const char* filePath) {
	int index_page, index_slot;
	if (filePath == "origin_data_mnist/") {
		index_page = id / MNIST_SLOT_NUM;
		index_slot = id % MNIST_SLOT_NUM;
	}
	else if (filePath == "preprocess_data_mnist/") {
		index_page = id / PRE_PROCESS_MNIST_SLOT_NUM;
		index_slot = id % PRE_PROCESS_MNIST_SLOT_NUM;
	}
	int real_array_index = get_a_page(index_page, filePath);
	if (real_array_index < 0) {
		return 0;
	}
	for (int i = 0; i < MNIST_VEC_DIM; i++) {
		if (filePath == "origin_data_mnist/") {
			all_pages[real_array_index][MNIST_SLOT_NUM + 12 + ((index_slot) * (MNIST_VEC_DIM + 4)) + 4 + i] = (new_vector[i] & 0x000000ff);
		}
		else if (filePath == "preprocess_data_mnist/") {
			all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((index_slot) * (2 * MNIST_VEC_DIM + 4)) + 4 + 2 * i] = (new_vector[i] & 0x000000ff);
			all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((index_slot) * (2 * MNIST_VEC_DIM + 4)) + 4 + 2 * i + 1] = (new_vector[i] & 0x0000ff00);
		}

	}
	dirty[real_array_index] = 1;
	return 1;
}

#endif // TRY_H
