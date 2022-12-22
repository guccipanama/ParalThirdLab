#define SIZE 200
#include <cstdio>
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include "mpi.h"
#include <cstdlib>
#include "time.h"

using namespace std;
int ProcNum, ProcRank;

void CreateMatrix(string path, int rows, int columns) {
	ofstream fprint;
	fprint.open(path);

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < columns; j++) {
			int tmp = rand() % 100;
			fprint << setw(5) << tmp;
		}
		//fprint << endl;
	}
	fprint.close();
}

int* ReadMatrix(string path, int rows, int columns) {
	ifstream M;
	M.open(path, ios_base::in);
	int* m = new int [rows];
	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < columns; j++) {
			M >> m[i];
		}
	}
	return m;
}

void MulMatrix(int*& A, int*& B, int*& C, int Size)
{
	int s = Size;
	int p, ind;

	MPI_Status st;
	int ProcPartSize = s / ProcNum;
	int ProcPartElem = ProcPartSize * s;
	int* bufA = new int[s * ProcPartSize];
	int* bufB = new int[s * ProcPartSize];
	int* bufC = new int[s * ProcPartSize];
	int ProcPart = s / ProcNum, part = ProcPart * s;

	MPI_Scatter(A, part, MPI_DOUBLE, bufA, part, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(B, part, MPI_DOUBLE, bufB, part, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	int temp = 0;
	for (int i = 0; i < ProcPartSize; i++) {
		for (int j = 0; j < ProcPartSize; j++) {
			for (int k = 0; k < s; k++)
				temp += bufA[i * s + k] * bufB[k * s + j];
			bufC[i * s + j + ProcPartSize * ProcRank] = temp;
		}
		temp = 0;
	}
	MPI_Finalize();
}

int main(int argc, char* argv[]) {

	srand(time(NULL));

	int r1 = SIZE, c1 = SIZE;
	int r2 = SIZE, c2 = SIZE;

	string path1 = "D:\\first.txt";
	string path2 = "D:\\second.txt";
	string path3 = "D:\\third.txt";
	string path4 = "D:\\meas.txt";

	CreateMatrix(path1, r1, c1);
	CreateMatrix(path2, r2, c2);

	int* m1 = ReadMatrix(path1, r1, c1);
	int* m2 = ReadMatrix(path2, r2, c2);

	
	int* result = new int[r1 * c2];

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	chrono::steady_clock::time_point begin = chrono::steady_clock::now();
	MulMatrix(m1, m2, result, SIZE);

	MPI_Finalize();

	chrono::steady_clock::time_point end = chrono::steady_clock::now();

	std::cout << "Multiplication:" << endl;
	double mulTime = chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	std::cout << "Time(ms) = " << chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << endl;

	ofstream R;
	R.open(path3);

	for (int i = 0; i < r1; i++) {
		for (int j = 0; j < c2; j++) {
			R << setw(8) << result[i];
		}
		//R << endl;
	}

	ofstream measurements;

	measurements.open(path4, ios::app);
	measurements << SIZE << "\t" << mulTime << endl;
	measurements.close();


	return 0;
}
/*#define _CRT_SECURE_NO_WARNINGS
//#include <iostream>
//#include "time.h"
//#include <fstream>
//#include <string>
//#include "mpi.h"
using namespace std;
int ProcNum, ProcRank;

void mulMatrix(double*& A, double*& B, double*& C, int& Size)
{
	int s = Size;
	int p, ind;

	MPI_Status st;
	int ProcPartSize = s / ProcNum;
	int ProcPartElem = ProcPartSize * s;
	double* bufA = new double[s * ProcPartSize];
	double* bufB = new double[s * ProcPartSize];
	double* bufC = new double[s * ProcPartSize];
	int ProcPart = s / ProcNum, part = ProcPart * s;

	MPI_Scatter(A, part, MPI_DOUBLE, bufA, part, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	MPI_Scatter(B, part, MPI_DOUBLE, bufB, part, MPI_DOUBLE, 0, MPI_COMM_WORLD);

	double temp = 0.0;
	for (int i = 0; i < ProcPartSize; i++) {
		for (int j = 0; j < ProcPartSize; j++) {
			for (int k = 0; k < s; k++)
				temp += bufA[i * s + k] * bufB[k * s + j];
			bufC[i * s + j + ProcPartSize * ProcRank] = temp;
		}
		temp = 0.0;
	}
}

double go(double* a, double* b, double* c, int N)
{
	a = new double[N * N];
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			a[i * N + j] = rand() % 10;
		}
	}
	ofstream fout;
	fout.open("D:\\first1.txt");


	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			fout << a[i * N + j] << ' ';
		fout << '\n';
	}
	fout.close();
	cout << "comlete write file A.txt!\n";





	ifstream fin;
	fin.open("D:\\first1.txt");
	double* A = new double[N * N];
	cout << "new A: \n";
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			fin >> A[i * N + j];
		}
	}
	fin.close();



	b = new double[N * N];
	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
		{
			b[i * N + j] = rand() % 10;
		}
	}

	fout.open("D:\\second1.txt");

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			fout << b[i * N + j] << ' ';
		fout << '\n';

	}
	fout.close();


	fin.open("D:\\second1.txt");
	double* B = new double[N * N];
	cout << "new B: \n";
	for (int i = 0; i < N; i++)
	{

		for (int j = 0; j < N; j++)
		{
			fin >> B[i * N + j];
		}
	}
	fin.close();

	c = new double[N * N];
	size_t count = 0;
	clock_t start = clock();
	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);

	mulMatrix(A, B, c, N);


	double result_time = double(clock() - start) / CLOCKS_PER_SEC;
	cout << endl << "Calculation time:" << result_time << " seconds" << endl;
	cout << "Complete..." << endl;
	fout.open("C.txt");

	for (int i = 0; i < N; i++)
	{
		for (int j = 0; j < N; j++)
			fout << c[i * N + j] << ' ';
		fout << '\n';
	}
	fout.close();

	return result_time;
}

int main()
{
	int N = 100;
	double* a = NULL, * b = NULL, * c = NULL;
	system("cls");

	ofstream fio;
	fio.open("D:\\meas.txt");
	for (int i = 0; i < 9; i++)
	{
		fio << N;
		fio << '\n';
		fio << "Calculation time for " << N << '*' << N << " matrix = " << go(a, b, c, N) << " seconds\n";
		N += 100;
		cout << i + 1 << endl;
	}


	fio.close();

	return 0;
}*/
