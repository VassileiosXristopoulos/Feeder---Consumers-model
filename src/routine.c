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

 key_t key=9876;
void consumers(int num,double begin){
	
	int *my_array;
	double dt=0,eval,time_diff;
	my_array=malloc(num*sizeof(int));
	int size=sizeof(int)+sizeof(double)+1;
	int sem_1,sem_2,sem_3;
	sem_1 = semget(KEY1, 1, 0666 );
	sem_2 = semget(KEY2, 1, 0666 );
	sem_3 = semget(KEY3, 1, 0666 );
	int ids[3]={sem_1,sem_2,sem_3};
	
	struct sembuf sb;
	int retval,shmid,i,j;
	char *data;
	if ((shmid = shmget(key, size, 0644 | IPC_CREAT)) == -1) { 
	        perror("shmget");
	        exit(1);
	    }
 	 data = shmat(shmid, (void *)0, 0);
	  if (data == (char *)(-1)) {
	      perror("shmat");
	      exit(1);
	  }
	for(i=0;i<num;i++){
		sb.sem_flg=0;
	 	sb.sem_num=0;
		sb.sem_op=-1;
		retval=semop(ids[i%2], &sb, 1); //this is to block the consumers
	
		time_diff=my_clock()-begin;
		my_array[i]=*(int *)data; //read 
		eval=*((double *)(data+sizeof(int)));
		dt+=(time_diff-eval)/(i+1); /// dt(n)= dt(n-1)+x(n)/n where x(n) is the time the process is executed 
		sb.sem_op=1;
		retval=semop(ids[2],&sb,1); 
	} 
	
	FILE *f = fopen("log_file.txt", "a");
	fprintf(f, "Process pid: %d | processes data: ",getpid() );
	for(i=0;i<num;i++){
		fprintf(f, "%d ", my_array[i]);
	}
	fprintf(f, "| average time: %f secs\n", dt);
	printf("%f\n",dt );

	fclose(f);
	shmctl(shmid , IPC_RMID , NULL);
	shmdt(data);
	for(i=0;i<3;i++){
		semctl(ids[i], 0, IPC_RMID);
	}
	free(my_array);

}

void feeder(int num,int processes,double begin){
	int sem_1,sem_2,sem_3,i,j;
	sem_1 = semget(KEY1, 1, 0666 );
	sem_2 = semget(KEY2, 1, 0666 );
	sem_3 = semget(KEY3, 1, 0666 );
	int ids[3]={sem_1,sem_2,sem_3};
	int size=sizeof(int)+sizeof(double)+1;


	struct sembuf sb;
	int retval,shmid;
	char *data;
	key_t key=9876;

	if ((shmid = shmget(key, size, 0644 | IPC_CREAT)) == -1) {
	        perror("shmget");
	        exit(1);
	    }
 	 data = shmat(shmid, (void *)0, 0);
	  if (data == (char *)(-1)) {
	      perror("shmat");
	      exit(1);
	    }
	time_t t;
	srand((unsigned) time(&t));
	int *my_array=malloc(num*sizeof(int));
		for(i=0;i<num;i++){
			my_array[i]=rand()%500;
		}
		for(i=0;i<num;i++){
				memcpy(data,&(my_array[i]),sizeof(int));
				double now=my_clock()-begin;
				memcpy(data+sizeof(int),&now,sizeof(double)); 	
				for(j=0;j<processes;j++){	
					sb.sem_flg=0;
					sb.sem_num=0;
					sb.sem_op=1;
					retval=semop(ids[i%2],&sb,1);//let a consumer operate 
					if((i==(num-1))&&(j==(processes-1))) break; //all consumers read all numbers, I will not down my semaphore
					sb.sem_flg=0;
					sb.sem_num=0;
					sb.sem_op=-1;
					retval=semop(ids[2],&sb,1); //get stuck at the queue		
				}	
		 	
		}
		shmdt(data);
		shmctl(shmid , IPC_RMID , NULL);
		for(i=0;i<3;i++){
			semctl(ids[i], 0, IPC_RMID);
		}
}

double my_clock(void) {
  struct timeval t;
  gettimeofday(&t, NULL);
  return (1.0e-6*t.tv_usec + t.tv_sec);
}
