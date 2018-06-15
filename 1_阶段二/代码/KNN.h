#ifndef KNN_H
#define KNN_H
#include "try.h"
#include "produce.h"
#include "math.h" 

#define MAX_NUM 100000000.0

struct Pair
{
	int id;
	double distance;
};

struct Pair* listForKNN;
struct Pair* tempPointList;
int pageIndex;
// ----- 阶段二 ----------
int tmpPageIndex = -1;		// 保存未找到的页，一般是新页
// ----- 阶段二 -----------

// 求出两个向量之间的距离
double dist(const int* pointA, const int* pointB, int dim) {
	double result = 0.0;
	for (int i = 0; i < dim; ++i)
	{
		result += pow(pointA[i] - pointB[i], 2);
		// printf("%lf %d\n", result, i);
	}
	return pow(result, 0.5);
}

double dist(const float* pointA, const float* pointB, int dim) {
	double result = 0.0;
	for (int i = 0; i < dim; ++i)
	{
		result += pow(pointA[i] - pointB[i], 2);
	}
	return pow(result, 0.5);
}

// 归并排序中的归并步骤
void Merge(struct Pair* list, int begin, int middle, int end) {
	int i = 0;
	int j = 0;
	int k = 0;

	for (i = begin, j = middle + 1, k = 0; k <= end - begin; k++)
	{
		if (i == middle + 1) {
			tempPointList[k] = list[j++];
			continue;
		}
		if (j == end + 1) {
			tempPointList[k] = list[i++];
			continue;
		}
		if (list[i].distance < list[j].distance) {
			tempPointList[k] = list[i++];
		}
		else {
			tempPointList[k] = list[j++];
		}
	}

	for (int i = 0, j = begin; i <= end - begin; ++i, ++j)
	{
		list[j].id = tempPointList[i].id;
		list[j].distance = tempPointList[i].distance;
	}
}
// 归并排序中的分治步骤
void MergeSort(struct Pair* list, int begin, int end) {
	if (begin < end) {
		int middle = (begin + end) / 2;
		MergeSort(list, begin, middle);
		MergeSort(list, middle + 1, end);
		Merge(list, begin, middle, end);
	}
}
// KNN_ForPagesInMemory函数每次将 topK中所有元素 和 listForKNN(内存缓冲中中所有向量集合)中所有元素 合并起来进行排序
// 并将排序结果中前K个存入topK。
// 这里的参数：
// point 表示 被查找的对象
// topK  表示 当前找出的距离point最近的k个点
// k     表示 前文提到的查找范围
// dim   表示 point的维度
void KNN_ForPagesInMemory(const int * point, struct Pair* topK_, int k, int dim, const char* filePath, int numPage) {
	// 初始化listForKNN
	int totalSlot = 0;
	if (filePath == "origin_data_mnist/") totalSlot = MNIST_SLOT_NUM;
	else if (filePath == "preprocess_data_mnist/") totalSlot = PRE_PROCESS_MNIST_SLOT_NUM;
	else if (filePath == "fly_random_data_mnist/"
		|| filePath == "fly_binary_data_mnist/") {
		totalSlot = GAUSS_SLOT_NUM;
	}
	else if (filePath == "fly_data_mnist/") {
		totalSlot = FLY_SLOT_NUM;
	}
	else if (filePath == "fly_WTA_data_mnist/") {
		totalSlot = FLY_WTA_SLOT_NUM;
	}

	for (int j = 0; j < k + 1; ++j)
	{
		listForKNN[j].id = topK_[j].id;
		listForKNN[j].distance = topK_[j].distance;
	}
	for (int j = k + 1; j < 50 * totalSlot + k + 1; ++j)
	{
		listForKNN[j].id = -1;
		listForKNN[j].distance = MAX_NUM;
	}

	// 从磁盘读入50页到内存缓冲区中
	for (int i = 0; i < numPage; ++i)
	{
		int num = get_a_page(pageIndex++, filePath);
		if (num == -1) {
			// ------ 阶段二 -----------
			if (tmpPageIndex == -1) {
				tmpPageIndex = --pageIndex;
			}
			// ------ 阶段二 -----------
			printf("can not get file : in KNN_For_One_page function\n");
		}
	}

	// 将内存缓冲区的50帧数据读到listForKNN
	int * tempPoint = (int *)calloc(dim + 1, sizeof(int));         // 临时变量，用于存储一个点
	for (int i = 0; i <= (pageIndex - 1) % 50; ++i)         // 其他时候循环50次，最后一次循环23次
	{
		char pageHeader[8];
		int slotSize = 0;
		int numofSlot = 0;

		// 获取头部信息
		for (int j = 0; j < 8; ++j)
		{
			pageHeader[j] = all_pages[i][4 + j];
		}
		slotSize = (int)((pageHeader[0] & 0xff) |
			((pageHeader[1] & 0xff) << 8) |
			((pageHeader[2] & 0xff) << 16) |
			((pageHeader[3] & 0xff) << 24));
		numofSlot = (int)((pageHeader[4] & 0xff) |
			((pageHeader[5] & 0xff) << 8) |
			((pageHeader[6] & 0xff) << 16) |
			((pageHeader[7] & 0xff) << 24));

		char* pointInfo = (char*)calloc(slotSize, sizeof(char));
		char* slotBitMap = (char*)calloc(numofSlot, sizeof(char));

		for (int j = 0; j < numofSlot; ++j)
		{
			slotBitMap[j] = all_pages[i][12 + j];
		}

		for (int j = 0; j < numofSlot; ++j)
		{
			// 判断槽号是否为空
			if (slotBitMap[j] == 0x00) {
				//printf("can not find message in the slot: %d\n", j);
				continue;
			}
			// 读取对应位置的信息
			for (int h = 0; h < slotSize; ++h)
			{
				pointInfo[h] = all_pages[i][12 + numofSlot + j * slotSize + h];
			}
			memset(tempPoint, 0, (dim+1)*sizeof(char));

			tempPoint[0] = (int)((pointInfo[0] & 0xff) |
				((pointInfo[1] & 0xff) << 8) |
				((pointInfo[2] & 0xff) << 16) |
				((pointInfo[3] & 0xff) << 24));
			for (int _i = 0; _i < ((filePath == "fly_binary_data_mnist/" || filePath == "fly_WTA_data_mnist/") ? 32 : dim); ++_i)
			{
				if (filePath == "origin_data_mnist/") {
					tempPoint[_i + 1] = (int)(pointInfo[4 + _i] & 0x000000ff);
				}
				else if (filePath == "preprocess_data_mnist/") {
					tempPoint[_i + 1] = (int)((pointInfo[4 + 2 * _i] & 0x000000ff)
						| ((pointInfo[4 + 2 * _i + 1] & 0x000000ff) << 8));
				}
				else if (filePath == "fly_random_data_mnist/" 
					|| filePath == "fly_data_mnist/") {
					tempPoint[_i + 1] = (int)((pointInfo[4 + 4 * _i] & 0x000000ff)
						| ((pointInfo[4 + 4 * _i + 1] & 0x000000ff) << 8)
						| ((pointInfo[4 + 4 * _i + 2] & 0x000000ff) << 16)
						| ((pointInfo[4 + 4 * _i + 3] & 0x000000ff) << 24));
				} else if (filePath == "fly_binary_data_mnist/") {
					int tem = (int)((pointInfo[4 + 4 * _i] & 0x000000ff)
						| ((pointInfo[4 + 4 * _i + 1] & 0x000000ff) << 8)
						| ((pointInfo[4 + 4 * _i + 2] & 0x000000ff) << 16)
						| ((pointInfo[4 + 4 * _i + 3] & 0x000000ff) << 24));
					tempPoint[tem + 1] = 1;
				} else if (filePath == "fly_WTA_data_mnist/") {
					int tem = (int)((pointInfo[4 + 8 * _i] & 0x000000ff)
						| ((pointInfo[4 + 8 * _i + 1] & 0x000000ff) << 8)
						| ((pointInfo[4 + 8 * _i + 2] & 0x000000ff) << 16)
						| ((pointInfo[4 + 8 * _i + 3] & 0x000000ff) << 24));
					int val = (int)((pointInfo[4 + 8 * _i + 4] & 0x000000ff)
						| ((pointInfo[4 + 8 * _i + 5] & 0x000000ff) << 8)
						| ((pointInfo[4 + 8 * _i + 6] & 0x000000ff) << 16)
						| ((pointInfo[4 + 8 * _i + 7] & 0x000000ff) << 24));
					tempPoint[tem + 1] = val;
				}

			}
			// 得到对应的struct
			listForKNN[k + 1 + j + i * numofSlot].id = tempPoint[0];
			// listForKNN[k + j + i*numofSlot].id = pageIndex*numofSlot + j;
			listForKNN[k + 1 + j + i * numofSlot].distance = dist(tempPoint + 1, point, dim);
			// printf("%lf\n", dist(tempPoint, point, dim));
		}
		// 做完一次循环销毁一次空间
		free(pointInfo);
		free(slotBitMap);
	}

	// 对listForKNN中的元素进行归并排序
	if (filePath == "origin_data_mnist/") {
		MergeSort(listForKNN, 0, k + 50 * MNIST_SLOT_NUM - 1 + 1);
	}
	else if (filePath == "preprocess_data_mnist/") {
		MergeSort(listForKNN, 0, k + 50 * PRE_PROCESS_MNIST_SLOT_NUM - 1 + 1);
	}
	else if (filePath == "fly_random_data_mnist/"
		|| filePath == "fly_binary_data_mnist/") {
		MergeSort(listForKNN, 0, k + 50 * GAUSS_SLOT_NUM - 1 + 1);
		
	}
	else if (filePath == "fly_data_mnist/") {
		MergeSort(listForKNN, 0, k + 50 * FLY_SLOT_NUM - 1 + 1);
	}
	else if (filePath == "fly_WTA_data_mnist/") {
		MergeSort(listForKNN, 0, k + 50 * FLY_WTA_SLOT_NUM - 1 + 1);
	}
	// 将排序结果中的前k个存储到topK中
	for (int i = 0; i < k + 1; ++i)
	{
		topK_[i].id = listForKNN[i].id;
		topK_[i].distance = listForKNN[i].distance;
	}

	// // 显示结果
	// printf("page index to: %d; the %dth sort: \n", pageIndex, pageIndex / 50);
	// for (int i = 0; i < k + 1; ++i)
	// {
	// 	printf("%d: %lf\n", topK_[i].id, topK_[i].distance);
	// 	// printf("%d\n", topK[i].id);
	// }

	// 离开之前销毁空间
	free(tempPoint);
}

// KNN求算磁盘中所有页中向量集合里距离给定点最近的K个点
struct Pair* KNN(const int pointId, int k, int dim, const char* filePath) {
	// 内存分配
	tmpPageIndex = -1;
	struct Pair* topK_ = (struct Pair*)calloc(k + 1, sizeof(struct Pair));
	int totalFile = 0;
	if (filePath == "origin_data_mnist/") {
		listForKNN = (struct Pair*)calloc(50 * MNIST_SLOT_NUM + k + 1, sizeof(struct Pair));
		tempPointList = (struct Pair*)calloc(50 * MNIST_SLOT_NUM + k + 1, sizeof(struct Pair));
		totalFile = MNISI_FILE_NUM;
	}
	else if (filePath == "preprocess_data_mnist/") {
		listForKNN = (struct Pair*)calloc(50 * PRE_PROCESS_MNIST_SLOT_NUM + k + 1, sizeof(struct Pair));
		tempPointList = (struct Pair*)calloc(50 * PRE_PROCESS_MNIST_SLOT_NUM + k + 1, sizeof(struct Pair));
		totalFile = PRE_PROCESS_MNIST_FILE_NUM;
	}
	else if (filePath == "fly_random_data_mnist/"
		|| filePath == "fly_binary_data_mnist/") {
		listForKNN = (struct Pair*)calloc(50 * GAUSS_SLOT_NUM + k + 1, sizeof(struct Pair));
		tempPointList = (struct Pair*)calloc(50 * GAUSS_SLOT_NUM + k + 1, sizeof(struct Pair));
		totalFile = 122;
	}
	else if (filePath == "fly_data_mnist/") {
		listForKNN = (struct Pair*)calloc(50 * FLY_SLOT_NUM + k + 1, sizeof(struct Pair));
		tempPointList = (struct Pair*)calloc(50 * FLY_SLOT_NUM + k + 1, sizeof(struct Pair));
		totalFile = 30000;
	}
	else if (filePath == "fly_WTA_data_mnist/") {
		listForKNN = (struct Pair*)calloc(50 * FLY_WTA_SLOT_NUM + k + 1, sizeof(struct Pair));
		tempPointList = (struct Pair*)calloc(50 * FLY_WTA_SLOT_NUM + k + 1, sizeof(struct Pair));
		totalFile = 240;
	}

	// 完成初始化
	pageIndex = 0;

	for (int i = 0; i < k + 1; ++i)
	{
		topK_[i].id = -1;
		topK_[i].distance = MAX_NUM;
	}

	int* point = NULL;
	// 根据id寻找给定要查找的点
	if (filePath == "origin_data_mnist/"
		|| filePath == "preprocess_data_mnist/") {
		point = get_vector_by_id(pointId, filePath);

	}
	else if (filePath == "fly_random_data_mnist/"
		|| filePath == "fly_binary_data_mnist/"
		|| filePath == "fly_data_mnist/"
		|| filePath == "fly_WTA_data_mnist/") {
		point = get_vector_by_id_in_all_fly_file(pointId, filePath);
	}

	
	if (point == NULL) {
		//printf("point can not find: in KNN function\n");
		// ------ 阶段二 ------
		free(topK_);
		return NULL;
		// ------ 阶段二 ------
	}

	// 求解KNN
	for (int i = 0; i < totalFile / 50 + 1; ++i)
		// for (int i = 0; i < 1; ++i)
	{
		KNN_ForPagesInMemory(point, topK_, k, dim, filePath, (((totalFile - i * 50) > 50) ? (50) : (totalFile - i * 50)));
	}

	free(point);
	free(listForKNN);
	free(tempPointList);
	return &topK_[1];
}

void KNN_ForPagesInMemory_float(const float* point, struct Pair* topK_, int k, int dim, const char* filePath, int numPage) {
	// 初始化listForKNN
	tmpPageIndex = -1;
	int totalSlot = 0;
	if (filePath != "gaussian_data_mnist/")
		totalSlot = GLOVE_SLOT_NUM;
	else
		totalSlot = GAUSS_SLOT_NUM;

	for (int j = 0; j < k + 1; ++j)
	{
		listForKNN[j].id = topK_[j].id;
		listForKNN[j].distance = topK_[j].distance;
	}
	for (int j = k + 1; j < 50 * totalSlot + k + 1; ++j)
	{
		listForKNN[j].id = -1;
		listForKNN[j].distance = MAX_NUM;
	}

	// 从磁盘读入50页到内存缓冲区中
	for (int i = 0; i < numPage; ++i)
	{
		int num = get_a_page(pageIndex++, filePath);
		if (num == -1) {
			//printf("can not get file : in KNN_For_One_page function\n");
		}
	}

	// 将内存缓冲区的50帧数据读到listForKNN
	float* tempPoint = (float *)calloc(dim + 1, sizeof(float));         // 临时变量，用于存储一个点
	for (int i = 0; i <= (pageIndex - 1) % 50; ++i)         // 其他时候循环50次，最后一次循环23次
	{
		char pageHeader[8];
		int slotSize = 0;
		int numofSlot = 0;

		// 获取头部信息
		for (int j = 0; j < 8; ++j)
		{
			pageHeader[j] = all_pages[i][4 + j];
		}
		slotSize = (int)((pageHeader[0] & 0xff) |
			((pageHeader[1] & 0xff) << 8) |
			((pageHeader[2] & 0xff) << 16) |
			((pageHeader[3] & 0xff) << 24));
		numofSlot = (int)((pageHeader[4] & 0xff) |
			((pageHeader[5] & 0xff) << 8) |
			((pageHeader[6] & 0xff) << 16) |
			((pageHeader[7] & 0xff) << 24));

		slotSize = slotSize;

		char* pointInfo = (char*)calloc(slotSize, sizeof(char));
		char* slotBitMap = (char*)calloc(numofSlot, sizeof(char));

		for (int j = 0; j < numofSlot; ++j)
		{
			slotBitMap[j] = all_pages[i][12 + j];
		}

		for (int j = 0; j < numofSlot; ++j)
		{
			// 判断槽号是否为空
			if (slotBitMap[j] == 0x00) {
				//printf("can not find message in the slot: %d\n", j);
				continue;
			}
			// 读取对应位置的信息
			for (int h = 0; h < slotSize; ++h)
			{
				pointInfo[h] = all_pages[i][12 + numofSlot + j * slotSize + h];
			}
			tempPoint[0] = (int)((pointInfo[0] & 0xff) |
				((pointInfo[1] & 0xff) << 8) |
				((pointInfo[2] & 0xff) << 16) |
				((pointInfo[3] & 0xff) << 24));
			for (int _i = 0; _i < dim; ++_i)
			{
				char floatBuf[4];
				floatBuf[0] = pointInfo[4 + 4 * _i] & 0x000000ff;
				floatBuf[1] = pointInfo[4 + 4 * _i + 1] & 0x000000ff;
				floatBuf[2] = pointInfo[4 + 4 * _i + 2] & 0x000000ff;
				floatBuf[3] = pointInfo[4 + 4 * _i + 3] & 0x000000ff;
				float* data = (float*)(&floatBuf);
				tempPoint[_i + 1] = *data;
			}
			// 得到对应的struct
			listForKNN[k + 1 + j + i * numofSlot].id = tempPoint[0];
			// listForKNN[k + j + i*numofSlot].id = pageIndex*numofSlot + j;
			listForKNN[k + 1 + j + i * numofSlot].distance = dist(tempPoint + 1, point, dim);
			// printf("%lf\n", dist(tempPoint, point, dim));
		}
		// 做完一次循环销毁一次空间
		free(pointInfo);
		free(slotBitMap);
	}

	if(filePath != "gaussian_data_mnist/")
		// 对listForKNN中的元素进行归并排序
		MergeSort(listForKNN, 0, k + 50 * GLOVE_SLOT_NUM - 1 + 1);
	else 
		MergeSort(listForKNN, 0, k + 50 * GAUSS_SLOT_NUM - 1 + 1);
	// 将排序结果中的前k个存储到topK中
	for (int i = 0; i < k + 1; ++i)
	{
		topK_[i].id = listForKNN[i].id;
		topK_[i].distance = listForKNN[i].distance;
	}

	// // 显示结果
	// printf("page index to: %d; the %dth sort: \n", pageIndex, pageIndex / 50);
	// for (int i = 1; i < k + 1; ++i)
	// {
	// 	printf("%d: %lf\n", topK_[i].id, topK_[i].distance);
	// 	// printf("%d\n", topK[i].id);
	// }

	// 离开之前销毁空间
	free(tempPoint);
}

struct Pair* KNN_float(const int pointId, int k, int dim, const char* filePath) {
	// 内存分配
	struct Pair* topK_ = (struct Pair*)calloc(k + 1, sizeof(struct Pair));
	int totalFile = 0;
	if (filePath != "gaussian_data_mnist/") {
		listForKNN = (struct Pair*)calloc(50 * GLOVE_SLOT_NUM + k + 1, sizeof(struct Pair));
		tempPointList = (struct Pair*)calloc(50 * GLOVE_SLOT_NUM + k + 1, sizeof(struct Pair));
		totalFile = GLOVE_FILE_NUM;
	} 
	else {
		listForKNN = (struct Pair*)calloc(50 * GAUSS_SLOT_NUM + k + 1, sizeof(struct Pair));
		tempPointList = (struct Pair*)calloc(50 * GAUSS_SLOT_NUM + k + 1, sizeof(struct Pair));
		totalFile = 122;
	}
	// 完成初始化
	pageIndex = 0;

	for (int i = 0; i < k + 1; ++i)
	{
		topK_[i].id = -1;
		topK_[i].distance = MAX_NUM;
	}
	float* point = NULL;
	// 根据id寻找给定要查找的点
	if (filePath != "gaussian_data_mnist/") {
		point = get_vector_by_id_glove(pointId, filePath);
	}
	else {
		point = get_vector_by_id_in_gaussian_data_mnist(pointId, filePath);
	}
	if (point == NULL) {
		//printf("point can not find: in KNN function\n");
		return &topK_[1];
	}

	// 求解KNN
	for (int i = 0; i < totalFile / 50 + 1; ++i)
		// for (int i = 0; i < 1; ++i)
	{
		KNN_ForPagesInMemory_float(point, topK_, k, dim, filePath, (((totalFile - i * 50) > 50) ? (50) : (totalFile - i * 50)));
	}

	free(point);
	free(listForKNN);
	free(tempPointList);
	return &topK_[1];
}

#endif // KNN_H
