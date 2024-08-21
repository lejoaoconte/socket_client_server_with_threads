#include<stdio.h> 
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

#define thread_count 5 
int p[thread_count]={0,1,2,3,4}; 

void *process(void *);

int main(int argc, char** argv) {


    void *status;
    pthread_t threads[thread_count];

    for(int i = 0; i < thread_count; i++) {
        pthread_create( &threads[i], 0, process, &p[i]);
    }

    for(int i = 0; i < thread_count; i++) {
        pthread_join( threads[i], &status);
    }

    return 0;
}

void *process(void *arg) {

    int id = *(int*)arg;
    for(int i = 0; i < 5; i++) {
        printf("Alô da thread %d\n", id);
        sleep(1);
    }
}
