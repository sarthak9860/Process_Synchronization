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
		printf("%d: Thread id = %d\n", i, *(int*)arg);
		sleep(2);
	}
	unlock();
	return NULL;
}

int main() {
    	sem_init(&sem, 0, 1);
	pthread_t t1, t2, t3;
	int id1 = 1, id2 = 2, id3 = 3;
	pthread_create(&t1, NULL, func, &id1);
	pthread_create(&t2, NULL, func, &id2);
	pthread_create(&t3, NULL, func, &id3);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(t3, NULL);
	return 0;
}
