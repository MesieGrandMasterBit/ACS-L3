#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include <math.h>

using namespace std;

double meanSquareDeviation(double array[], int n, char combination) { // Нахождение среднеквадратиченого отклонения
	double averageSum = 0;
	double difference = 0;
	double sumDifference = 0;
	double dispersion = 0;
	double deviation = 0;

	for (int i = 0; i < n; i++)
		averageSum += array[i];

	averageSum /= n;

	for (int i = 0; i < n; i++) {
		difference = array[i] - averageSum;
		sumDifference += pow(difference, 2);
	}

	if (combination == 'g')
		dispersion = sumDifference / n;
	else if (combination == 'n')
		dispersion = sumDifference / (n - 1);

	deviation = sqrt(dispersion);
	cout << "mSD = " << deviation << endl;
	return deviation;
}

long int writeTime(int *block, long int size) { // Функция записи
	long int i;
	srand(time(0));
	struct timespec mt1, mt2;
	long int tt = 0;
	int buffer;

	for (i = 0; i < size; i++) {
		buffer = rand() % 1000;
		clock_gettime (CLOCK_REALTIME, &mt1);
		block[i] = buffer;
		clock_gettime (CLOCK_REALTIME, &mt2);
		tt += 1000000000 * (mt2.tv_sec - mt1.tv_sec) + (mt2.tv_nsec - mt1.tv_nsec);
	}
	return tt;
}

long int readTime(int *block, long int size) { // Функция чтения
	long int i;
	srand(time(0));
	struct timespec mt1, mt2;
	long int tt = 0;

	for (i = 0; i < size; i++) {
		clock_gettime (CLOCK_REALTIME, &mt1);
		block[i];
		clock_gettime (CLOCK_REALTIME, &mt2);
		tt += 1000000000 * (mt2.tv_sec - mt1.tv_sec) + (mt2.tv_nsec - mt1.tv_nsec);
	}
	return tt;
}

long int writeTimeFile(int *block, long int size, string path) { // Функция записи в файл
	int n = 10000;
	ofstream file(path);
	long int i;
	int buffer[n];
	srand(time(0));
	struct timespec mt1, mt2;
	long int tt = 0;

	for (i = 0; i < size; i++) {
		block[i] = rand() % 1000;
	}

	for (i = 0; i < size / n; i++) {
		clock_gettime (CLOCK_REALTIME, &mt1);
		file.write((char*)block, n);
		clock_gettime (CLOCK_REALTIME, &mt2);
		tt += 1000000000 * (mt2.tv_sec - mt1.tv_sec) + (mt2.tv_nsec - mt1.tv_nsec);
	}

	file.close();
	return tt;
}

long int readTimeFile(int *block, long int size, string path) { // Функция чтения из файла
	int n = 10000;
	ifstream file(path);
	long int i;
	struct timespec mt1, mt2;
	long int tt = 0;

	for (i = 0; i < size / n; i++) {
		clock_gettime (CLOCK_REALTIME, &mt1);
		file.read((char*)block, n);
		clock_gettime (CLOCK_REALTIME, &mt2);
		tt += 1000000000 * (mt2.tv_sec - mt1.tv_sec) + (mt2.tv_nsec - mt1.tv_nsec);
	}

	file.close();
	return tt;
}

void PrintFile(char *MT, long int BS, int launchNum, double WT, double AWT, double WB, double AEw, double REw, double RT, double ART, double RB, double AEr, double REr)
{
	FILE *fp;
	fp = fopen("result.csv", "a");
	char *info;
	info = "MemoryType;BlockSize;ElementType;BufferSize;LaunchNum;Timer;WriteTime;AverageWriteTime;WriteBandwidth;AbsError(write);RelError(write);ReadTime;AverageReadTime;ReadBandwidth;AbsError(read);RelError(read)";
	fprintf(fp, "%s;%d;Int;BufferSize;%d;clock_gettime();%f;%f;%f;%f;%f;%f;%f;%f;%f;%f\n", MT, BS, launchNum, WT, AWT, WB, AEw, REw, RT, ART, RB, AEr, REr);
	fclose(fp);
}

int main(int argc, char *argv[]) {
	string mKey = "-m";
	string bKey = "-b";
	string lKey = "-l";
	string strRam = "RAM";
	string strSSD = "SSD";
	string strFlash = "flash";
	string sizeKb = "Kb";
	string sizeMb = "Mb";

	int launchNum = 0;
	long int arrayLength, blockSize = 0;
	double averageWriteTime = 0;
	double averageReadTime = 0;
	double writeBandWidth = 0;
	double readBandWidth = 0;
	double absErrorWrite, absErrorRead, relErrorWrite, relErrorRead;

	if (argc >= 7) {
		if (argv[1] == mKey && argv[3] == bKey && (argv[5] == lKey || argv[6] == lKey)) {
			if (argv[5] == sizeKb)
				blockSize = atoi(argv[4]) * 1024;
		else if (argv[5] == sizeMb) {
			blockSize = atoi(argv[4]) * 1024 * 1024;
		}
	else
		blockSize = atoi(argv[4]);

	arrayLength = blockSize / sizeof(int);
	int *block;
	block = (int*)malloc(sizeof(int) * arrayLength);

	if (argv[5] == lKey)
		launchNum = atoi(argv[6]);
	else
		launchNum = atoi(argv[7]);

	double timesWrite[launchNum], timesRead[launchNum];

	if (argv[2] == strSSD) {
		for (int i = 0; i < launchNum; i++) {
			timesWrite[i] = writeTimeFile(block, arrayLength, "file.txt") / pow(10, 9);
			timesRead[i] = readTimeFile(block, arrayLength, "file.txt") / pow(10, 9);
			averageWriteTime += timesWrite[i] / (i + 1);
			averageReadTime += timesRead[i] / (i + 1);
			writeBandWidth = (blockSize / averageWriteTime) / pow(10, 6);
			readBandWidth = (blockSize / averageReadTime) / pow(10, 6);
			absErrorWrite = meanSquareDeviation(timesWrite, i + 1, 'g');
			absErrorRead = meanSquareDeviation(timesRead, i + 1, 'g');
			relErrorWrite = absErrorWrite / averageWriteTime * 100;
			relErrorRead = absErrorRead / averageReadTime * 100;
			PrintFile(argv[2],blockSize,i+1,timesWrite[i],averageWriteTime,writeBandWidth,absErrorWrite,relErrorWrite,timesRead[i],averageReadTime,readBandWidth,absErrorRead,relErrorRead);
			cout << "Испытание № " << i + 1 << endl;
			cout << "Время записи в секундах: " << timesWrite[i] << endl;
			cout << "Время чтения в секундах: " << timesRead[i] << endl;
			cout << endl;
			blockSize = blockSize + (4 * 1024 * 1024);
		}
	} else if (argv[2] == strRam) {
		for (int i = 0; i < launchNum; i++) {
		timesWrite[i] = writeTime(block, arrayLength) / pow(10, 9);
		timesRead[i] = readTime(block, arrayLength) / pow(10, 9);
		averageWriteTime += timesWrite[i] / (i + 1);
		averageReadTime += timesRead[i] / (i + 1);
		writeBandWidth = (blockSize / averageWriteTime) / pow(10, 6);
		readBandWidth = (blockSize / averageReadTime) / pow(10, 6);
		absErrorWrite = meanSquareDeviation(timesWrite, i + 1, 'g');
		absErrorRead = meanSquareDeviation(timesRead, i + 1, 'g');
		relErrorWrite = absErrorWrite / averageWriteTime * 100;
		relErrorRead = absErrorRead / averageReadTime * 100;
		PrintFile(argv[2],blockSize,i+1,timesWrite[i],averageWriteTime,writeBandWidth,absErrorWrite,relErrorWrite,timesRead[i],averageReadTime,readBandWidth,absErrorRead,relErrorRead);
		cout << "Испытание № " << i + 1 << endl;
		cout << "Время записи в секундах: " << timesWrite[i] << endl;
		cout << "Время чтения в секундах: " << timesRead[i] << endl;
		cout << endl;
		blockSize = blockSize + (8 * 1024 * 1024);
		}
	} else {
		cout << "Incorrect argument!" << endl;
	}
	} else {
		cout << "Incorrect argument!!" << endl;
	}
	} else {
		cout << "Incorrect argument!!!" << endl;
	}
	return 0;
}
