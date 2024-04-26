#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <semaphore.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sstream>
using namespace std;
struct Buffer {
	int totalmovies;
	int movieNum;
	int ticNum;
	int cnic;
	char arr[2048];
};
struct Buffer2 {
	char ans;
	bool check;
	char arr[60];
};
void readmoviesfromfile(char**& names, int*& tickets, int& total, int*& sizes) {
	int file = open("movieTicketsInfo.txt", O_RDONLY | O_CREAT);
	if (file == -1) {
		perror("File not found!");
		exit(0);
	}
	ssize_t bytesread;
	char number;
	bytesread = read(file, &number, 1);
	int num = atoi(&number);
	total = num;
	names = new char* [num];
	tickets = new int[num];
	sizes = new int[num];
	for (int i = 0; i < num; i++) {
		names[i] = new char[50];
	}
	bytesread = read(file, &number, 1);
	for (int i = 0; i < num; i++) {
		char buffer[100];
		int x = 0, a = 0, count = 0;
		int b = 0;
		while (buffer[x] != '\n' || buffer[x] != '\0') {
			bytesread = read(file, &buffer[x], 1);

			if (buffer[x] == 47) {
				count++;
			}
			if (count >= 2 && (buffer[x] >= 48 && buffer[x] <= 57)) {
				b *= 10;
				char temp = buffer[x];
				b += atoi(&temp);
			}

			if ((buffer[x] >= 65 && buffer[x] <= 90) || (buffer[x] >= 97 && buffer[x] <= 122) || buffer[x] == 32) {
				names[i][a++] = buffer[x];
			}

			if (buffer[x] == '\n' || buffer[x] == '\0') {
				break;
			}
			x++;
		}
		sizes[i] = a;
		tickets[i] = b;
	}
	close(file);
}

void updatemoviesfile(char** names, int* tickets, int total, int total2, int*& sizes) {
	int file = open("movieTicketsInfo.txt", O_WRONLY | O_CREAT);
	if (file == -1) {
		perror("File not found!");
		exit(0);
	}
	truncate("movieTicketsInfo.txt", 0);
	lseek(file, 0, SEEK_SET);
	cin.clear();
	
	ostringstream oss1;
	oss1 << total2 << "\n";
	string line = oss1.str();
	write(file, line.c_str(), line.size());
	int linenum = 1;
	for (int i = 0; i < total; i++) {
		if (tickets[i] > 0) {
			ostringstream oss;
			oss << linenum << "/"  << names[i] << "/" << tickets[i] << "\n";
			string line = oss.str();
			write(file, line.c_str(), line.size());
			linenum++;
		}
	}
	close(file);
}

void bookingfile(int cnic, int movien, int ticn) {
	char bf[2048];
	int file = open("booking.txt", O_APPEND| O_RDWR| O_CREAT);
	ostringstream oss;
	
	if (file == -1) {
		perror("File not found!");
		exit(0);
	}
	int top =  0;
	char buffer[2];
	
	off_t position = lseek(file, 0, SEEK_END);
	lseek(file, 0, SEEK_SET);
	
	close(file);
	
	file = open("booking.txt", O_APPEND| O_RDWR| O_CREAT);
    	if (position == 0){
    		oss << "0\n";
    		string line = oss.str();
		write(file, line.c_str(), line.size());
    	}
    	
	oss.str("");
	oss << cnic << "/"  << movien << "/" << ticn << " \n";
	string line = oss.str();
	write(file, line.c_str(), line.size());
	lseek(file, 0, SEEK_SET);
	read(file, buffer, 1);
	top = atoi(buffer);
	top++;
	close(file);
	
	
	file = open("booking.txt", O_RDWR| O_CREAT);
	oss.str("");
	oss << top << "\n";
	line = oss.str();
	write(file, line.c_str(), line.size());
	close(file);
}

int main(int argc, char* args[]) {

	Buffer* obj;
	Buffer2* obj2;

	int id_sem1 = shmget(1069, 1024, 0666 | IPC_CREAT | IPC_EXCL);
	int id_sem2 = shmget(1079, 1024, 0666 | IPC_CREAT | IPC_EXCL);
	int id_sem3 = shmget(1089, 1024, 0666 | IPC_CREAT | IPC_EXCL);
	int id_cs = shmget(1099, 4096, 0666 | IPC_CREAT | IPC_EXCL);
	int id_cs2 = shmget(1109, 4096, 0666 | IPC_CREAT | IPC_EXCL);
	if (id_sem1 == -1 || id_cs == -1 || id_sem2 == -1) {
		perror("Error creating shared memory!");
		return 0;
	}
	sem_t* semaphore1;
	sem_t* semaphore2;
	sem_t* semaphore3;
	semaphore1 = (sem_t*)shmat(id_sem1, NULL, 0);
	semaphore2 = (sem_t*)shmat(id_sem2, NULL, 0);
	semaphore3 = (sem_t*)shmat(id_sem3, NULL, 0);
	obj = (Buffer*)shmat(id_cs, NULL, 0);
	obj2 = (Buffer2*)shmat(id_cs2, NULL, 0);
	sem_init(semaphore1, 1, 0);
	sem_init(semaphore2, 1, 0);
	sem_init(semaphore3, 1, 0);

	while(true){
		char** names;
		int* tickets;
		int* sizes;
		int total, x = 0;
		readmoviesfromfile(names, tickets, total, sizes);
		obj->totalmovies = total;
		for (int i = 0; i < total; i++) {
			int j = 0;
			obj->arr[x++] = '$';
			while (names[i][j] != '\0') {
				obj->arr[x++] = names[i][j++];
			}
		}
		obj->arr[x] = '@';


		sem_post(semaphore2);
		sem_wait(semaphore1);

		int mn = obj->movieNum - 1;
		if (obj->ticNum <= tickets[mn]) {
			obj2->check = true;
			string temp3 = "Your total bill for movie " + string(names[mn]) + " is: ";
			strcpy(obj2->arr, temp3.c_str());
		}
		else if (obj->ticNum > tickets[mn]) {
			obj2->check = false;
			string temp3 = "Insufficient tickets available :(";
			strcpy(obj2->arr, temp3.c_str());
			sem_post(semaphore3);
			sem_wait(semaphore1);
			continue;
		}
		sem_post(semaphore3);
		sem_wait(semaphore1);
		
		if (obj2->ans == 'n'){
			continue;
		}
		
		if (obj2->ans == 'y') {
			int tn = obj->ticNum;
			tickets[mn] -= tn;
			int total2 = total;
			for (int i = 0; i < total; i++) {
				if (tickets[i] <= 0) {
					total2--;
				}
			}
			updatemoviesfile(names, tickets, total, total2, sizes);
			int cnic1 = obj->cnic;
			int mv1 = obj->movieNum;
			int tc1 = obj->ticNum;
			bookingfile(cnic1, mv1, tc1);
		}
		sem_post(semaphore3);
		sem_wait(semaphore1);
	}

	sem_destroy(semaphore1);
	sem_destroy(semaphore2);
	sem_destroy(semaphore3);
	shmdt(semaphore1);
	shmdt(semaphore2);
	shmdt(semaphore2);
	shmdt(obj);
	shmdt(obj2);
	shmctl(id_sem1, IPC_RMID, NULL);
	shmctl(id_sem2, IPC_RMID, NULL);
	shmctl(id_sem3, IPC_RMID, NULL);
	shmctl(id_cs, IPC_RMID, NULL);
	shmctl(id_cs2, IPC_RMID, NULL);
	return 0;
}
