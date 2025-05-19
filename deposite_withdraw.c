#include<stdio.h>
#include<pthread.h>
#include<unistd.h>

static int money=0;
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cv = PTHREAD_COND_INITIALIZER;

void deposit(int amount) {
	pthread_mutex_lock(&m);
	money += amount;
	pthread_cond_broadcast(&cv);
	pthread_mutex_unlock(&m);
}

void withdraw(int amount) {
	pthread_mutex_lock(&m);
	while (money < amount) {
		pthread_cond_wait(&cv, &m);
	}
	money -= amount;
	pthread_mutex_unlock(&m);
}

void* adder(void* arg){
        for(int i=0; i<20; i++){
                deposit(1500);
                printf("Deposit 1500 and Current money is %d\n", money);
                sleep(1);
        }
        return NULL;
}

void* subtractor(void* arg){
        for(int i=0; i<10; i++){
                withdraw(2000);
                printf("Withdraw 1000 and Current money is %d\n", money);
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
