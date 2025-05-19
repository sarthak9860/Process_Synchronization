#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

#define MAXVAL 100

static int glob = 0;

void subtract(unsigned int n){
	pthread_mutex_lock(&mtx);
	glob -= n;
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mtx);
}

void add(unsigned int n){
	pthread_mutex_lock(&mtx);
	while(glob + n > MAXVAL){
		pthread_cond_wait(&cond, &mtx);
	}
	glob += n;
	pthread_mutex_unlock(&mtx);
}

void* adder(void* arg){
	for(int i=0; i<20; i++){
		add(15);
		printf("Added 15 and Global Value is %d\n", glob);
		sleep(1);
	}
	return NULL;
}

void* subtractor(void* arg){
	for(int i=0; i<20; i++){
		subtract(10);
		printf("Subtracted 10 and global value is %d\n", glob);
		sleep(1);
	}
	return NULL;
}

int main(){
	pthread_t t1, t2;
	pthread_create(&t1, NULL, adder, NULL);
	pthread_create(&t2, NULL, subtractor, NULL);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	return 0;
}
