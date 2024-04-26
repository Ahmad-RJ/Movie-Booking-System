#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <semaphore.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
using namespace std;
struct Buffer{
	int totalmovies;
	int movieNum;
	int ticNum;
	int cnic;
	char arr[2048];
};
struct Buffer2{
	char ans;
	bool check;
	char arr[60];
};
int main(){
	Buffer* obj;
	Buffer2* obj2;
	int id_sem1 = shmget(1069, 1024, 0);
	int id_sem2 = shmget(1079, 1024, 0);
	int id_sem3 = shmget(1089, 1024, 0);
	int id_cs = shmget(1099, 4096,  0);
	int id_cs2 = shmget(1109, 4096,  0);
	if (id_sem1 == -1 || id_cs == -1 || id_sem2 == -1){
		perror("Error creating shared memory!");
		return 0;
	}
	sem_t *semaphore1;
	sem_t *semaphore2;
	sem_t *semaphore3;
	int* names;
	semaphore1 = (sem_t*) shmat(id_sem1, NULL, 1);
	semaphore2 = (sem_t*) shmat(id_sem2, NULL, 0);
	semaphore3 = (sem_t*) shmat(id_sem3, NULL, 0);
	obj = (Buffer*) shmat(id_cs, NULL, 0);
	obj2 = (Buffer2*) shmat(id_cs2, NULL, 0);
	
	sem_wait(semaphore2);
	cout << "********** Welcome To Movie World **********" << endl << endl;
	int num = obj->totalmovies;
	int x = 0;
	for (int i = 0; i < num; i++){
		if (obj->arr[x] == '$'){
			cout <<"[" << i + 1 << "] ";
			x++;
			while (obj->arr[x] != '$' || obj->arr[x] != '@'){
				cout << obj->arr[x++];
				if (obj->arr[x] == '$' || obj->arr[x] == '@'){
					break;
				}
			}
			cout << endl;
		}
		if (obj->arr[x] == '@'){
			break;
		}
	}
	int movienum, ticnum;
	cout << "Choose a movie: ";
	cin >> movienum;
	cout << "Enter number of tickets you want: ";
	cin >> ticnum;
	obj->ticNum = ticnum;
	obj->movieNum = movienum;
	
	
	sem_post(semaphore1);
	sem_wait(semaphore3);
	
	if (obj2->check == false){
		cout << obj2->arr;
		cout << endl;
		sem_post(semaphore1);

		shmdt(semaphore1);
		shmdt(semaphore2);
		shmdt(semaphore3);
		shmdt(obj);
		shmdt(obj2);
		return 0;
	}
	else {
		cout << obj2->arr;
		cout << ticnum * 500;
		cout << endl << "Press y to proceed or press n to cancel: ";
		char temp1;
		cin >> temp1;
		obj2->ans = temp1;
		if (temp1 == 'n'){
			sem_post(semaphore1);
		
			shmdt(semaphore1);
			shmdt(semaphore2);
			shmdt(semaphore3);
			shmdt(obj);
			shmdt(obj2);
			return 0;
		}
	}
	
	int cnicno;
	cout << "Enter CNIC: ";
	cin >> cnicno;
	obj->cnic = cnicno;
	sem_post(semaphore1);
	sem_wait(semaphore3);
	
	cout << "Reservation Done!" << endl;
	
	
	sem_post(semaphore1);
	
	shmdt(semaphore1);
	shmdt(semaphore2);
	shmdt(semaphore3);
	shmdt(obj);
	shmdt(obj2);
	return 0;
}
