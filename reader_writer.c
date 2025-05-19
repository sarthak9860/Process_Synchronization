#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

typedef struct {
	pthread_mutex_t mtx;
	pthread_cond_t can_read;
	pthread_cond_t can_write;
	int readers;
	int writers;
	int waiting_writers;
} rw_lock_t;

void rw_lock_init(rw_lock_t* lck) {
	pthread_mutex_init(&lck->mtx, NULL);
	pthread_cond_init(&lck->can_read, NULL);
	pthread_cond_init(&lck->can_write, NULL);
	lck->readers = 0;
	lck->writers = 0;
	lck->waiting_writers = 0;
}

void reader_lock(rw_lock_t* lck) {
	pthread_mutex_lock(&lck->mtx);
	while (lck->writers > 0 || lck->waiting_writers > 0) {
		pthread_cond_wait(&lck->can_read, &lck->mtx);
	}
	lck->readers++;
	pthread_mutex_unlock(&lck->mtx);
}

void writer_lock(rw_lock_t* lck) {
	pthread_mutex_lock(&lck->mtx);
	lck->waiting_writers++;
	while (lck->readers > 0 || lck->writers > 0) {
		pthread_cond_wait(&lck->can_write, &lck->mtx);
	}
	lck->waiting_writers--;
	lck->writers = 1;
	pthread_mutex_unlock(&lck->mtx);
}

void reader_unlock(rw_lock_t* lck) {
	pthread_mutex_lock(&lck->mtx);
	lck->readers--;
	if (lck->readers == 0 && lck->waiting_writers > 0) {
		pthread_cond_signal(&lck->can_write);
	}
	pthread_mutex_unlock(&lck->mtx);
}

void writer_unlock(rw_lock_t* lck) {
	pthread_mutex_lock(&lck->mtx);
	lck->writers = 0;
	if (lck->waiting_writers > 0) {
		pthread_cond_signal(&lck->can_write);
	} else {
		pthread_cond_broadcast(&lck->can_read);
	}
	pthread_mutex_unlock(&lck->mtx);
}

rw_lock_t lock;

void* reader(void* arg) {
	int id = *(int*)arg;
	reader_lock(&lock);
	printf("Reader %d: reading...\n", id);
	sleep(1);
	printf("Reader %d: done.\n", id);
	reader_unlock(&lock);
	return NULL;
}

void* writer(void* arg) {
	int id = *(int*)arg;
	writer_lock(&lock);
	printf("Writer %d: writing...\n", id);
	sleep(2);
	printf("Writer %d: done.\n", id);
	writer_unlock(&lock);
	return NULL;
}

int main() {
	pthread_t r1, r2, w1, r3;
	int id1 = 1, id2 = 2, id3 = 3, wid = 10;

	rw_lock_init(&lock);

	pthread_create(&r1, NULL, reader, &id1);
	pthread_create(&r2, NULL, reader, &id2);
	sleep(1);
	pthread_create(&w1, NULL, writer, &wid);
	sleep(1);
	pthread_create(&r3, NULL, reader, &id3);

	pthread_join(r1, NULL);
	pthread_join(r2, NULL);
	pthread_join(w1, NULL);
	pthread_join(r3, NULL);
	return 0;
}

