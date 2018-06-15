#ifndef TRY_H
#define TRY_H

#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <sstream>
using namespace std;

#define SIZEOFPAGE 65536
#define MNIST_SLOT_NUM 83
#define GLOVE_SLOT_NUM 54
#define PRE_PROCESS_MNIST_SLOT_NUM 41
#define GAUSS_SLOT_NUM 492
#define FLY_SLOT_NUM 2
#define FLY_RANDOM_SLOT_NUM 492
#define FLY_WTA_SLOT_NUM 251
#define FLY_BINARY_SLOT_NUM 492
#define MNIST_VEC_DIM 784
#define GLOVE_VEC_DIM 300

int MNISI_FILE_NUM = 723;
int GLOVE_FILE_NUM = 40667;
int PRE_PROCESS_MNIST_FILE_NUM = 1464;
int MNIST_VEC_NUM = 60000;
int GLOVE_VEC_NUM = 2196017;

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


	// ----- 阶段二 ------
	FILE* fin = NULL;
	fin = fopen("dbpro.conf", "r");
	fscanf(fin, "%d", &MNISI_FILE_NUM);
	fscanf(fin, "%d", &GLOVE_FILE_NUM);
	fscanf(fin, "%d", &PRE_PROCESS_MNIST_FILE_NUM);
	fscanf(fin, "%d", &MNIST_VEC_NUM);
	fscanf(fin, "%d", &GLOVE_VEC_NUM);
	fclose(fin);
	// ----- 阶段二 -------
}

// 把指定帧的页覆盖地写回到磁盘中去
bool write_page(int array_index, const char* filePath) {

	// 如果写回，那么磁盘中的数据的一致性就得到保证了，设为0
	dirty[array_index] = 0;
	char name_temp[40];
	sprintf(name_temp, "%s%d", filePath, file_index[array_index]);
	file_ptr = NULL;
	file_ptr = fopen(name_temp, "wb");
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
		// cout << "id: " << id_check << endl;
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

		// cout << return_array[i] << " ";
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


// ----- 阶段二 ----
void end_pages(const char* filePath) {
	for (int i = 0; i < 50; ++i) {
		if (dirty[i] == 1) {
			write_page(i, filePath);
			dirty[i] = 0;
		}
	}
}

// 插入向量到原始数据页中，然后把插入向量更新到预处理和做投影页中去
// pre_vector在投影之后需要释放
void insert_vector(int *new_vector) {
	// pageIndex专用于读入页到内存缓存区
	int pageIndex = 0;
	int tmpPageIndex = -1;

	int vecId = 0;
	int pageId = 0;
	int slotSize = 0;
	int numofSlot = 0;
	int offset = -1;

	char s[40];
	FILE* fout = NULL;
	bool arrayToTagSlot = true;
	unsigned char temp = 0x00;

	

	// 每50页一批处理所有原始数据页
	while (1) {
		// 从磁盘读入50页到内存缓存区中
		for (int i = 0; i < 50; ++i) {
			int num = get_a_page(pageIndex++, "origin_data_mnist/");
			if (num == -1) {
				if (tmpPageIndex == -1) {
					tmpPageIndex = --pageIndex;
					break;
				}
				//printf("can not get file : in KNN_For_One_page function");
			}
		}

		for (int i = 0; i <= (pageIndex - 1) % 50; ++i)
		{
			char pageHeader[12];

			// 获取头部信息
			for (int j = 0; j < 12; ++j)
			{
				pageHeader[j] = all_pages[i][j];
			}
			pageId = (int)((pageHeader[0] & 0xff) |
				((pageHeader[1] & 0xff) << 8) |
				((pageHeader[2] & 0xff) << 16) |
				((pageHeader[3] & 0xff) << 24));
			slotSize = (int)((pageHeader[4] & 0xff) |
				((pageHeader[5] & 0xff) << 8) |
				((pageHeader[6] & 0xff) << 16) |
				((pageHeader[7] & 0xff) << 24));
			numofSlot = (int)((pageHeader[8] & 0xff) |
				((pageHeader[9] & 0xff) << 8) |
				((pageHeader[10] & 0xff) << 16) |
				((pageHeader[11] & 0xff) << 24));

			char* slotBitMap = (char*)calloc(numofSlot, sizeof(char));

			for (int j = 0; j < numofSlot; ++j)
			{
				slotBitMap[j] = all_pages[i][12 + j];
			}

			for (int j = 0; j < numofSlot; ++j) {
				if (slotBitMap[j] == 0x00) {		// 能找到空闲槽
					offset = j;
					all_pages[i][12 + j] = 0x01;	// 把 slotBitMap 相应部分设为 true
					dirty[i] = 1;
					break;
				}
			}
			if (offset != -1) break;
		}
		if (offset != -1 || tmpPageIndex != -1) break;
	}
	// 所有页都无空闲槽可供向量插入，此时需要创建新页，将内容写入
	if (offset == -1) {
		memset(s, 0, sizeof(s));
		sprintf(s, "%s%d", "origin_data_mnist/", tmpPageIndex);
		fout = fopen(s, "wb");
		// new page id
		fwrite(&tmpPageIndex, sizeof(int), 1, fout);
		int slotSize = 788;
		fwrite(&slotSize, sizeof(int), 1, fout);
		int numOfSlot = 83;
		fwrite(&numOfSlot, sizeof(int), 1, fout);

		for (int k = 0; k < 83; k++) {
			// 只有第一个槽被使用，是插入的新向量 
			if (k == 0) {
				fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
			}
			else {
				bool tagSlotNotUse = false;
				fwrite(&tagSlotNotUse, sizeof(bool), 1, fout);
			}
		}

		vecId = tmpPageIndex * numofSlot;
		fwrite(&vecId, sizeof(int), 1, fout);

		// 只写第一个槽
		for (int j = 0; j < MNIST_VEC_DIM; ++j) {
			temp = new_vector[j] & 0x000000ff;
			fwrite(&temp, sizeof(unsigned char), 1, fout);
		}
		fclose(fout);
		
		++MNISI_FILE_NUM;
		++MNIST_VEC_NUM;

	}
	// 否则此时能找到空闲槽，其位置在 pageId 页的 offset 位置，使用end_pages更新内存到磁盘
	else {
		vecId = pageId * numofSlot + offset;
		int real_array_index = get_a_page(pageId, "origin_data_mnist/");
		// cout << "vecId" << vecId << endl;
		all_pages[real_array_index][MNIST_SLOT_NUM + 12 + ((offset) * (MNIST_VEC_DIM + 4))] = (vecId & 0x000000ff);
		all_pages[real_array_index][MNIST_SLOT_NUM + 12 + ((offset) * (MNIST_VEC_DIM + 4)) + 1] = (vecId >> 8) & 0x000000ff;
		all_pages[real_array_index][MNIST_SLOT_NUM + 12 + ((offset) * (MNIST_VEC_DIM + 4)) + 2] = (vecId >> 16) & 0x000000ff;
		all_pages[real_array_index][MNIST_SLOT_NUM + 12 + ((offset) * (MNIST_VEC_DIM + 4)) + 3] = (vecId >> 24);
		for (int i = 0; i < MNIST_VEC_DIM; i++) {
			all_pages[real_array_index][MNIST_SLOT_NUM + 12 + ((offset) * (MNIST_VEC_DIM + 4)) + 4 + i] = (new_vector[i] & 0x000000ff);
			// cout << (new_vector[i] & 0x000000ff);
		}
		dirty[real_array_index] = 1;
		end_pages("origin_data_mnist/");

		++MNIST_VEC_NUM;
	}

	// 预处理
	int prePageId = vecId / PRE_PROCESS_MNIST_SLOT_NUM;
	int preOffset = vecId % PRE_PROCESS_MNIST_SLOT_NUM;
	short int pretemp = 0;
	// 记得释放
	int *pre_vector = (int*)malloc(MNIST_VEC_DIM * sizeof(int));

	// 新向量的预处理
	int total = 0;
	double divFactor = 0;
	for (int j = 0; j < MNIST_VEC_DIM; ++j) {
		total += new_vector[j];
	}
	divFactor = (double)total / (100 * MNIST_VEC_DIM);
	for (int j = 0; j < MNIST_VEC_DIM; ++j) {
		pre_vector[j] = new_vector[j] / divFactor;
		cout << pre_vector[j] << " ";
	}
	cout << endl;

	
	int real_array_index = get_a_page(prePageId, "preprocess_data_mnist/");
	// 获取不到指定页，对应情况是预处理页中没有空闲槽，需要额外开新页
	if (real_array_index == -1) {
		memset(s, 0, sizeof(s));
		sprintf(s, "%s%d", "preprocess_data_mnist/", prePageId);
		fout = fopen(s, "wb");
		fwrite(&prePageId, sizeof(int), 1, fout);
		int slotSize = 1572;
		fwrite(&slotSize, sizeof(int), 1, fout);
		// number of slot
		int numOfSlot = 41;
		fwrite(&numOfSlot, sizeof(int), 1, fout);

		for (int k = 0; k < 41; ++k) {
			// 只有第一个槽被使用
			if (k == 0) {
				fwrite(&arrayToTagSlot, sizeof(bool), 1, fout);
			}
			else {
				bool tagSlotNotUse = false;
				fwrite(&tagSlotNotUse, sizeof(bool), 1, fout);
			}
		}

		fwrite(&vecId, sizeof(int), 1, fout);

		// 只写第一个槽
		for (int j = 0; j < MNIST_VEC_DIM; ++j) {
			pretemp = pre_vector[j] & 0x0000ffff;
			fwrite(&pretemp, sizeof(short int), 1, fout);
		}
		++PRE_PROCESS_MNIST_FILE_NUM;
	}
	else { 	// 能获取到指定的预处理页，修改其slotBitMap和槽部分
		all_pages[real_array_index][12 + preOffset] = 0x01;
		all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((preOffset) * (2 * MNIST_VEC_DIM + 4))] = (vecId & 0x000000ff);
		all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((preOffset) * (2 * MNIST_VEC_DIM + 4)) + 1] = (vecId >> 8) & 0x000000ff;
		all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((preOffset) * (2 * MNIST_VEC_DIM + 4)) + 2] = (vecId >> 16) & 0x000000ff;
		all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((preOffset) * (2 * MNIST_VEC_DIM + 4)) + 3] = (vecId >> 24);
		for (int i = 0; i < MNIST_VEC_DIM; i++) {
			all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((preOffset) * (2 * MNIST_VEC_DIM + 4)) + 4 + 2 * i] = (pre_vector[i] & 0x000000ff);
			all_pages[real_array_index][PRE_PROCESS_MNIST_SLOT_NUM + 12 + ((preOffset) * (2 * MNIST_VEC_DIM + 4)) + 4 + 2 * i + 1] = (pre_vector[i] & 0x0000ff00);
		}
		dirty[real_array_index] = 1;
		end_pages("preprocess_data_mnist/");
	}

	// 投影


	// 重写信息
	fout = fopen("dbpro.conf", "w");
	fprintf(fout, "%d", MNISI_FILE_NUM);
	fprintf(fout, " ");
	fprintf(fout, "%d", GLOVE_FILE_NUM);
	fprintf(fout, " ");
	fprintf(fout, "%d", PRE_PROCESS_MNIST_FILE_NUM);
	fprintf(fout, " ");
	fprintf(fout, "%d", MNIST_VEC_NUM);
	fprintf(fout, " ");
	fprintf(fout, "%d", GLOVE_VEC_NUM);
	fclose(fout);
}

// 根据id从原始数据页中删除向量，并把删除更新到预处理和做投影页中去
void delete_vector_by_id(int id) {
	
	FILE* fout = NULL;
	// 从原始数据页中删除
	int pageId = id / MNIST_SLOT_NUM;
	int offset = id % MNIST_SLOT_NUM;

	int real_array_index = get_a_page(pageId, "origin_data_mnist/");
	if (real_array_index == -1) {
		printf("The id you provide may be wrong!");
		return;
	} 
	else {
		if (all_pages[real_array_index][12 + offset] == 0x00) {
			printf("The vector of this id has already been removed!");
			return;
		}
		else {
			all_pages[real_array_index][12 + offset] = 0x00;
			dirty[real_array_index] = 1;
			end_pages("origin_data_mnist/");
			--MNIST_VEC_NUM;
		}
	}

	// 预处理
	int prePageId = id / PRE_PROCESS_MNIST_SLOT_NUM;
	int preOffset = id % PRE_PROCESS_MNIST_SLOT_NUM;


	real_array_index = get_a_page(prePageId, "preprocess_data_mnist/");
	all_pages[real_array_index][12 + preOffset] = 0x00;
	dirty[real_array_index] = 1;
	end_pages("preprocess_data_mnist");

	// 做投影

	fout = fopen("dbpro.conf", "w");
	fprintf(fout, "%d", MNISI_FILE_NUM);
	fprintf(fout, " ");
	fprintf(fout, "%d", GLOVE_FILE_NUM);
	fprintf(fout, " ");
	fprintf(fout, "%d", PRE_PROCESS_MNIST_FILE_NUM);
	fprintf(fout, " ");
	fprintf(fout, "%d", MNIST_VEC_NUM);
	fprintf(fout, " ");
	fprintf(fout, "%d", GLOVE_VEC_NUM);
	fclose(fout);

}


// ----- 阶段二 -----

#endif // TRY_H
