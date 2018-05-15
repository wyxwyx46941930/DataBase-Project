#include <iostream>
#include <fstream>
#include "fileIO.h"
#include "KNN.h"
#include <time.h>

using namespace::std;

ofstream out;

void test1() {
	cout << "#		origin_data_mnist		#" << endl;
	init_pages();
	int* test_arr1 = get_vector_by_id(123, "origin_data_mnist/");
	for (int i = 0; i < MNIST_VEC_DIM; i++) {
		cout << test_arr1[i] << " ";
	}
	free(test_arr1);
	cout << endl;

	cout << "#		preprocess_data_mnist		#" << endl;
	init_pages();
	int* test_arr2 = get_vector_by_id(123, "preprocess_data_mnist/");
	for (int i = 0; i < MNIST_VEC_DIM; i++) {
		cout << test_arr2[i] << " ";
	}
	free(test_arr2);
	cout << endl;
}


void test2() {
	cout << "#		origin_data_glove		#" << endl;
	init_pages();
	float* test_arr1 = get_vector_by_id_glove(3, "origin_data_glove/");
	for (int i = 0; i < GLOVE_VEC_DIM; i++) {
		cout << test_arr1[i] << " ";
	}
	free(test_arr1);
	cout << endl;

	cout << "#		preprocess_data_glove		#" << endl;
	init_pages();
	float* test_arr2 = get_vector_by_id_glove(3, "preprocess_data_glove/");
	for (int i = 0; i < GLOVE_VEC_DIM; i++) {
		cout << test_arr2[i] << " ";
	}
	free(test_arr2);
	cout << endl;
}

double query(int id, int mode) {
	int i = mode;
	int k = 200;
	struct Pair* topK;
	clock_t start, finish;
	double totalTime;
	init_pages();
	start = clock();
	if (i == 0) 
		topK = KNN(id, k, MNIST_VEC_DIM, "origin_data_mnist/");
	else if(i == 1)
		topK = KNN(id, k, MNIST_VEC_DIM, "preprocess_data_mnist/");
	else if(i == 2) 
		KNN_glove(id, k, GLOVE_VEC_DIM, "origin_data_glove/");
	else 
		KNN_glove(id, k, GLOVE_VEC_DIM, "preprocess_data_glove/");
	finish = clock();
	totalTime = (double)(finish - start) / CLOCKS_PER_SEC;

	// 写到文件里面
	for (int i = 0; i < 200; ++i)
	{
		out << topK[i].id << "->";
	}
	out << "\n" << "the time for this search is : " << totalTime << "\n\n";
	return totalTime;
}

void experiment_for_1000_times(int fileNum, int mode) {
	char name_temp[40];
	sprintf(name_temp, "%s%d%s", "result", fileNum, ".txt");
	out.open(name_temp, ios::out);
	double average = 0.0;
	for (int i = 0; i < 1000; ++i)
	{
		srand((unsigned)time(NULL));
		int randomNUM = (rand()*2 + rand()%2);
		while (randomNUM >= 60000) {
			randomNUM = (rand()*2 + rand()%2);
		}
		cout << "the " << i << "th point for search: " << randomNUM << "\n";
		out << "the " << i << "th point for search: " << randomNUM << "\n";
		double temp = query(randomNUM, mode);
		average += temp;
		cout << "the time for this search is : " << temp << "\n\n"; 
	}
	cout << "the average time for such 1000 times search : " << average/1000 << "\n\n\n\n";

	out << "the average time of search is : " << average/1000 << "\n";
	out.close();
}

/**
 * compare id of origin and preprocess, find its intersection
 */
void intersect() {
	int A[60001] = { 0 };
	int num = 0;
	for (int i = 0; i < 600; ++i) {
		cin >> num;
		A[num]++;
	}
	int count = 0;
	cout << "######################"
		<< "#                    #"
		<< "######################" << endl;
	for (int i = 0; i < 60001; ++i) {
		if (A[i] == 2) {
			++count;
			cout << i << endl;
		}
	}
	cout << "The same: " << count << endl;
}



int main() {
	
	// fileIO_origin_data_in_mnist("mnist", MNIST_VEC_NUM, MNIST_VEC_DIM);
	// fileIO_preProcess_data_in_mnist(MNIST_VEC_NUM, MNIST_VEC_DIM);
	// fileIO_origin_data_in_glove("glove", GLOVE_VEC_NUM, GLOVE_VEC_DIM);
	// fileIO_preProcess_data_in_mnist(GLOVE_VEC_NUM, GLOVE_VEC_DIM);
	// test1();
	// test2();
	// query();
	// intersect();
	// 原数据实验
	cout << "test" << endl;
	for (int i = 0; i < 6; ++i)
	{
		experiment_for_1000_times(i, 0);
	}
	// 预处理后数据处理
	for (int i = 0; i < 6; ++i)
	{
		experiment_for_1000_times(i+6, 1);
	}
	return 0;
}

