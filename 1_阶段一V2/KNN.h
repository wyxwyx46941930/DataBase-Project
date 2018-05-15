#ifndef KNN_H
#define KNN_H
#include "try.h"
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
void KNN_ForPagesInMemory(const int * point, struct Pair* topK, int k, int dim, const char* filePath) {
	// 初始化listForKNN
	int totalSlot = 0;
	if (filePath == "origin_data_mnist/") totalSlot = MNIST_SLOT_NUM;
	else if (filePath == "preprocess_data_mnist/") totalSlot = PRE_PROCESS_MNIST_SLOT_NUM;
	for (int j = 0; j < k + 1; ++j)
	{
		listForKNN[j].id = topK[j].id;
		listForKNN[j].distance = topK[j].distance;
	}
	for (int j = k + 1; j < 50 * totalSlot + k + 1; ++j)
	{
		listForKNN[j].id = -1;
		listForKNN[j].distance = MAX_NUM;
	}

	// 从磁盘读入50页到内存缓冲区中
	for (int i = 0; i < 50; ++i)
	{
		int num = get_a_page(pageIndex++, filePath);
		if (num == -1) {
			//printf("can not get file : in KNN_For_One_page function\n");
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
			tempPoint[0] = (int)((pointInfo[0] & 0xff) |
				((pointInfo[1] & 0xff) << 8) |
				((pointInfo[2] & 0xff) << 16) |
				((pointInfo[3] & 0xff) << 24));
			for (int _i = 0; _i < dim; ++_i)
			{
				if (filePath == "origin_data_mnist/") {
					tempPoint[_i + 1] = (int)(pointInfo[4 + _i] & 0x000000ff);
				}
				else if (filePath == "preprocess_data_mnist/") {
					tempPoint[_i + 1] = (int)(pointInfo[4 + 2 * _i] & 0x000000ff)
						| ((pointInfo[4 + 2 * _i + 1] & 0x000000ff) << 8);
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
	// 将排序结果中的前k个存储到topK中
	for (int i = 0; i < k + 1; ++i)
	{
		topK[i].id = listForKNN[i].id;
		topK[i].distance = listForKNN[i].distance;
	}

	// // 显示结果
	// printf("page index to: %d; the %dth sort: \n", pageIndex, pageIndex / 50);
	// for (int i = 1; i < k + 1; ++i)
	// {
	// 	printf("%d: %lf\n", topK[i].id, topK[i].distance);
	// 	// printf("%d\n", topK[i].id);
	// }

	// 离开之前销毁空间
	free(tempPoint);
}

// KNN求算磁盘中所有页中向量集合里距离给定点最近的K个点
struct Pair* KNN(const int pointId, int k, int dim, const char* filePath) {
	// 内存分配
	struct Pair* topK = (struct Pair*)calloc(k + 1, sizeof(struct Pair));
	int totalFile = 0;
	if (filePath == "origin_data_mnist/") {
		listForKNN = (struct Pair*)calloc(50 * MNIST_SLOT_NUM + k + 1, sizeof(struct Pair));
		tempPointList = (struct Pair*)calloc(50 * MNIST_SLOT_NUM + k + 1, sizeof(struct Pair));
		totalFile = MNISI_FILE_NUM;
	}
	else if (filePath == "preprocess_data_mnist/") {
		listForKNN = (struct Pair*)calloc(50 * PRE_PROCESS_MNIST_SLOT_NUM+ k + 1, sizeof(struct Pair));
		tempPointList = (struct Pair*)calloc(50 * PRE_PROCESS_MNIST_SLOT_NUM + k + 1, sizeof(struct Pair));
		totalFile = PRE_PROCESS_MNIST_FILE_NUM;
	}
	// 完成初始化
	pageIndex = 0;

	for (int i = 0; i < k + 1; ++i)
	{
		topK[i].id = -1;
		topK[i].distance = MAX_NUM;
	}

	// 根据id寻找给定要查找的点
	int* point = get_vector_by_id(pointId, filePath);
	if (point == NULL) {
		//printf("point can not find: in KNN function\n");
		return &topK[1];
	}
	
	// 求解KNN
	for (int i = 0; i < totalFile / 50 + 1; ++i)
		// for (int i = 0; i < 1; ++i)
	{
		KNN_ForPagesInMemory(point, topK, k, dim, filePath);
	}

	free(point);
	return &topK[1];
}

void KNN_ForPagesInMemory_glove(const float* point, struct Pair* topK, int k, int dim, const char* filePath) {
	// 初始化listForKNN
	int totalSlot = 0;
	totalSlot = GLOVE_SLOT_NUM;
	
	for (int j = 0; j < k + 1; ++j)
	{
		listForKNN[j].id = topK[j].id;
		listForKNN[j].distance = topK[j].distance;
	}
	for (int j = k + 1; j < 50 * totalSlot + k + 1; ++j)
	{
		listForKNN[j].id = -1;
		listForKNN[j].distance = MAX_NUM;
	}

	// 从磁盘读入50页到内存缓冲区中
	for (int i = 0; i < 50; ++i)
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

	// 对listForKNN中的元素进行归并排序
	MergeSort(listForKNN, 0, k + 50 * GLOVE_SLOT_NUM - 1 + 1);
	// 将排序结果中的前k个存储到topK中
	for (int i = 0; i < k + 1; ++i)
	{
		topK[i].id = listForKNN[i].id;
		topK[i].distance = listForKNN[i].distance;
	}

	// // 显示结果
	// printf("page index to: %d; the %dth sort: \n", pageIndex, pageIndex / 50);
	// for (int i = 1; i < k + 1; ++i)
	// {
	// 	printf("%d: %lf\n", topK[i].id, topK[i].distance);
	// 	// printf("%d\n", topK[i].id);
	// }

	// 离开之前销毁空间
	free(tempPoint);
}

struct Pair* KNN_glove(const int pointId, int k, int dim, const char* filePath) {
	// 内存分配
	struct Pair* topK = (struct Pair*)calloc(k + 1, sizeof(struct Pair));
	int totalFile = 0;
	listForKNN = (struct Pair*)calloc(50 * GLOVE_SLOT_NUM + k + 1, sizeof(struct Pair));
	tempPointList = (struct Pair*)calloc(50 * GLOVE_SLOT_NUM + k + 1, sizeof(struct Pair));
	totalFile = GLOVE_FILE_NUM;
	// 完成初始化
	pageIndex = 0;

	for (int i = 0; i < k + 1; ++i)
	{
		topK[i].id = -1;
		topK[i].distance = MAX_NUM;
	}

	// 根据id寻找给定要查找的点
	float* point = get_vector_by_id_glove(pointId, filePath);
	if (point == NULL) {
		//printf("point can not find: in KNN function\n");
		return &topK[1];
	}

	// 求解KNN
	for (int i = 0; i < totalFile / 50 + 1; ++i)
		// for (int i = 0; i < 1; ++i)
	{
		KNN_ForPagesInMemory_glove(point, topK, k, dim, filePath);
	}

	free(point);
	return &topK[1];
}

#endif // KNN_H
