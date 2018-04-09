#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "../header/routine.h"
#define KEY1 (1492)
#define KEY2 (1493)
#define KEY3 (1494)


int main(int argv, char *argc[]){
	double begin=my_clock();
	key_t key=9876;
	remove("log_file.txt"); //if it exists create new, otherwise do nothing
	union semun {
			int val;
			struct semid_ds *buf;
			ushort * array;
		} argument;

	   argument.val = 0;

	int size=sizeof(int)+sizeof(double)+1;
	int sem_1,sem_2,sem_3,i,j,status;
	sem_1 = semget(KEY1, 1, 0666 |IPC_CREAT );
	sem_2 = semget(KEY2, 1, 0666 |IPC_CREAT );
	sem_3 = semget(KEY3, 1, 0666 |IPC_CREAT);
	int ids[3]={sem_1,sem_2,sem_3};
	for(i=0;i<3;i++){
		if(ids[i] < 0)
		{
		    fprintf(stderr, "Unable to obtain semaphore.\n");
		    exit(0);
		} 
		if( semctl(ids[i], 0, SETVAL, argument) < 0)
		   {
		      fprintf( stderr, "Cannot set semaphore value.\n");
		   }
	}
	struct sembuf sb;
	int retval,shmid;
	char *data;
	if ((shmid = shmget(key, size, 0644 | IPC_CREAT)) == -1) { //i created the shared memory
	        perror("shmget");
	        exit(1);
	    }
 	 data = shmat(shmid, (void *)0, 0);
	  if (data == (char *)(-1)) {
	      perror("shmat");
	      exit(1);
	  }


	    /*      we got the shared memory and atached a pointer to it      */

	    int process_num=atoi(argc[1]);
	    int elements=atoi(argc[2]);
	    pid_t pid;
	    pid=fork();
	    if(pid==0){
		   for(i=0;i<process_num;i++){ 
		   		pid=fork();  
		   		if(pid==0){ //child  
			 		consumers(elements,begin);
			 		pid=wait(&status);
			 		exit(0);
		   		}
			}
				while(pid=wait(&status)>0);
				exit(0);
	   	}
	   	else{
	   		feeder(elements,process_num,begin);
	   		while(pid=wait(&status)>0);
	   	}
	   	shmdt(data);
		return 0;
		
}