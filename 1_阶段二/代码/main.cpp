#include <iostream>
#include <fstream>
#include "fileIO.h"
#include "KNN.h"
#include <time.h>
#include "produce.h"

using namespace::std;

ofstream out;

// ----- 阶段二 ----
struct Pair* mnistKNN = NULL;
struct Pair* preMnistKNN = NULL;
struct Pair* topK = NULL;
double orimAP = 0.0;
double premAP = 0.0;
// ----- 阶段二 -----

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


// ------- 阶段二 ----------
bool isInMnistKNN(int id) {
	for (int i = 0; i < 200; ++i) {
		if (mnistKNN[i].id == id)
			return true;
	}
	return false;
}

bool isInPreMnistKNN(int id) {
	for (int i = 0; i < 200; ++i) {
		if (preMnistKNN[i].id == id) {
			return true;
		}
	}
	return false;
}

double AP(int mode) {
	int truth = 0;
	int total = 0;
	double precision = 0.0;

	if (mode == 0) {
		for (int i = 0; i < 200; ++i) {
			total++;

			if (isInMnistKNN(topK[i].id)) {
				truth++;
				precision += (double)truth / total;
			}
		}
	}
	else {
		for (int i = 0; i < 200; ++i) {
			total++;

			if (isInPreMnistKNN(topK[i].id)) {
				truth++;
				precision += (double)truth / total;
			}
		}
	}

	return (precision / 200);
}
// ------- 阶段二 ----------

// 谨记 KNN 返回的指针在使用完以后是需要free掉的
double query(int id, int mode) {
	
	int k = 200;
	clock_t start, finish;
	double totalTime;
	init_pages();
	start = clock();

	if (mode == 0) {
		mnistKNN = KNN(id, k, MNIST_VEC_DIM, "origin_data_mnist/");
		if (mnistKNN == NULL) {		// 此次查询不统计
			return 0.0;
		}
	}
	else if (mode == 1) {
		preMnistKNN = KNN(id, k, MNIST_VEC_DIM, "preprocess_data_mnist/");
		if (preMnistKNN == NULL) {
			return 0.0;				// 此次查询不统计
		}
	}
	// ----- 阶段二 -----
	else if (mode == 2) {
		topK = KNN_float(id, k, 32, "gaussian_data_mnist/");
		if (topK == NULL) {
			return 0.0;
		}	
	}
	else if (mode == 3) {
		topK = KNN(id, k, 7840, "fly_data_mnist/");
		if (topK == NULL) {
			return 0.0;
		}
	}
	else if (mode == 4) {
		topK = KNN(id, k, 32, "fly_random_data_mnist/");
		if (topK == NULL) {
			return 0.0;
		}
	}
	else if (mode == 5) {
		topK = KNN(id, k, 7840, "fly_WTA_data_mnist/");
		if (topK == NULL) {
			return 0.0;
		}
	}
	else if (mode == 6) {
		topK = KNN(id, k, 7840, "fly_binary_data_mnist/");
		if (topK == NULL) {
			return 0.0;
		}
	}

	// ----- 阶段二 -----
	/**
	else if (i == 2)
	topK = KNN_float(id, k, GLOVE_VEC_DIM, "origin_data_glove/");
	else
	topK = KNN_float(id, k, GLOVE_VEC_DIM, "preprocess_data_glove/");
	*/


	finish = clock();
	totalTime = (double)(finish - start) / CLOCKS_PER_SEC;

	// 写到文件里面
	for (int i = 0; i < 200; ++i)
	{
		if (mode == 0) {
			out << mnistKNN[i].id << "->";
		}
		else if (mode == 1) {
			out << preMnistKNN[i].id << "->";
		}
		else {
			out << topK[i].id << "->";
		}
	}
	out << "\n" << "the time for this search is : " << totalTime << "\n\n";
	return totalTime;
}

void experiment_for_1000_times(int fileNum, int mode, bool testmAP) {
	char name_temp[40];
	sprintf(name_temp, "%s%d%s", "result", fileNum, ".txt");
	out.open(name_temp, ios::out);

	// ----- 阶段二 -----------
	double averageTime = 0.0;
	double averageOriAP = 0.0;
	double averagePreAP = 0.0;
	// ----- 阶段二 --------

	for (int i = 0; i < 100; )
	{
		srand((unsigned)time(NULL) + i);
		int randomNUM = rand() % (MNISI_FILE_NUM * 83);

		cout << "the " << i << "th point for search: " << randomNUM << "\n";
		out << "the " << i << "th point for search: " << randomNUM << "\n";
		double temp = query(randomNUM, mode);
		if(temp != 0.0) {
			++i;
		}
		else {
			continue;
		}
		averageTime += temp;
		cout << "the time for this search is : " << temp << "\n\n";
		out << "the time for this search is : " << temp << "\n\n";

		// ----- 阶段二 ------
		if (testmAP) {
			query(randomNUM, 0);
			query(randomNUM, 1);
			temp = AP(0);
			averageOriAP += temp;
			cout << "the ori AP for this search is : " << temp << endl;
			out << "the ori AP for this search is : " << temp << endl;
			temp = AP(1);
			averagePreAP += temp;
			cout << "the pre AP for this search is : " << temp << endl;
			out << "the pre AP for this search is : " << temp << endl;
		}
		// ---- 阶段二 ------

	}
	cout << "the average time for such 1000 times search : " << averageTime / 100 << "\n\n\n\n";
	out << "the average time for such 1000 times search : " << averageTime / 100 << "\n\n\n\n";

	// ------ 阶段二 -------
	if (testmAP) {
		orimAP += averageOriAP / 100;
		premAP += averagePreAP / 100;
		cout << "the ori mAP for this search is : " << orimAP << endl;
		out << "the ori mAP for this search is : " << orimAP << endl;
		cout << "the pre mAP for this search is : " << premAP << endl;
		out << "the pre mAP for this search is : " << premAP << endl;
	}
	// ------ 阶段二 -------
	out.close();
}

void testManyTimes() {

	// 原数据实验
	/*
	cout << "test" << endl;
	for (int i = 0; i < 6; ++i)
	{
		experiment_for_1000_times(i, 0, false);
	}
	// 预处理后数据处理
	for (int i = 0; i < 6; ++i)
	{
		experiment_for_1000_times(i + 6, 1, false);
	}
	*/
	// -------- 阶段二 --------
	// 近似处理，这里先假设 mode 为 2
	for (int i = 0; i < 6; ++i) {
		experiment_for_1000_times(i + 12, 2, false);
	}
	cout << "final ori mAP : " << orimAP / 6 << endl;
	cout << "final pre mAP : " << premAP / 6 << endl;
	orimAP = 0;
	premAP = 0;
	// -------- 阶段二 -------

}

void seeConf() {
	cout << "MNISI_FILE_NUM: " << MNISI_FILE_NUM << endl;
	cout << "GLOVE_FILE_NUM: " << GLOVE_FILE_NUM << endl;
	cout << "PRE_PROCESS_MNIST_FILE_NUM: " << PRE_PROCESS_MNIST_FILE_NUM << endl;
	cout << "MNIST_VEC_NUM: " << MNIST_VEC_NUM << endl;
	cout << "GLOVE_VEC_NUM: " << GLOVE_VEC_NUM << endl;
}

void testInsert() {
	init_pages();
	seeConf();


	cout << "###########################" << endl
		<< "#         Before          #" << endl
		<< "###########################" << endl;
	test_mnist();

	cout << "###########################" << endl
		<< "#         Insert          #" << endl
		<< "###########################" << endl;

	int vec[784] = {0};
	cout << "Input the vector you want to insert" << endl;

	for (int i = 0; i < 784; ++i) {
		cin >> vec[i];
	}
	insert_vector(vec);
	
	cout << "\n ---- Test if insert ---- \n\n";	

	int *v = get_vector_by_id(60000, "origin_data_mnist/");
	for (int i = 0; i < 784; ++i) {
		cout << v[i] << " ";
	}
	cout << "\\nn -------------------------- \n";

	cout << "###########################" << endl
		<< "#         After           #" << endl
		<< "###########################" << endl;
	test_mnist();

	seeConf();

}

void testDelete() {
	init_pages();
	seeConf();
	cout << "###########################" << endl
		<< "#         Before          #" << endl
		<< "###########################" << endl;
	test_mnist();

	cout << "###########################" << endl
		<< "#         Delete          #" << endl
		<< "###########################" << endl;
	delete_vector_by_id(60000);
	
	cout << "###########################" << endl
		<< "#         After           #" << endl
		<< "###########################" << endl;
	test_mnist();
	seeConf();
}


int main() {
	// fileIO_origin_data_in_mnist("mnist", MNIST_VEC_NUM, MNIST_VEC_DIM);
	// fileIO_preProcess_data_in_mnist(MNIST_VEC_NUM, MNIST_VEC_DIM);
	// fileIO_origin_data_in_glove("glove", GLOVE_VEC_NUM, GLOVE_VEC_DIM);
	// fileIO_preProcess_data_in_mnist(GLOVE_VEC_NUM, GLOVE_VEC_DIM);
	// test_mnist();
	// test_preprocess_mnist();
	// testInsert();
	// testDelete();
	init_pages();
	
	// for (int i = 0; i < 5; ++i)
	// {
	// 	experiment_for_1000_times(i,2,1);
	// }

	// for (int i = 0; i < 5; ++i)
	// {
	// 	experiment_for_1000_times(i + 5,4,1);
	// }

	// for (int i = 0; i < 5; ++i)
	// {
	// 	experiment_for_1000_times(i + 10,5,1);
	// }

	for (int i = 0; i < 5; ++i)
	{
		experiment_for_1000_times(i + 15,6,1);
	}

	// Pair* temp = KNN(234, 10, 32, "fly_random_data_mnist/");
	// for (int i = 0; i < 10; ++i)
	// {
	// 	cout << temp[i].id << " " << temp[i].distance << endl;
	// }
	// temp = KNN(234, 10, 784, "preprocess_data_mnist/");
	// cout << endl << endl;

	// for (int i = 0; i < 10; ++i)
	// {
	// 	cout << temp[i].id << " " << temp[i].distance << endl;
	// }
	// cout << endl << endl;
	// temp = KNN(234, 10, 784, "origin_data_mnist/");
	// 	for (int i = 0; i < 10; ++i)
	// {
	// 	cout << temp[i].id << " " << temp[i].distance << endl;
	// }





	// int* tem = get_vector_by_id(20, "preprocess_data_mnist/");
	// for (int i = 0; i < 700; ++i)
	// {
	// 	cout << tem[i] << " ";
	// }

	// KNN_float(1, 200, 32, "gaussian_data_mnist/");
	

	// testManyTimes();
}

