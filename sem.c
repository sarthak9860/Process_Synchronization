#include<semaphore.h>
#include<pthread.h>
#include<assert.h>
#include<stdio.h>
#include<unistd.h>

sem_t sem;
__thread int owns_lock = 0;

void lock() {
	sem_wait(&sem);
	owns_lock = 1;
}

void unlock() {
	assert(owns_lock == 1); 
	owns_lock = 0;
	sem_post(&sem);
}

void* func(void* arg){
	lock();
	for(int i=0; i<5; i++){
		printf("Thread id = %d\n", *(int*)arg);
		sleep(2);
	}
	unlock();
	return NULL;
}

int main() {
    	sem_init(&sem, 0, 1);
	pthread_t t1, t2;
	int id1 = 1, id2 = 2;
	pthread_create(&t1, NULL, func, &id1);
	pthread_create(&t2, NULL, func, &id2);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	return 0;
}
