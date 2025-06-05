#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

void *thread(void *vargp);

int main()
{
    pthread_t tid;
    printf("Thread[%lu]: Hello World da thread principal!\n", (long int) pthread_self());

    pthread_create(&tid, NULL, thread, NULL);
    pthread_join(tid, NULL);
    pthread_exit(NULL);
}

void *thread(void *vargp)
{
    printf("Thread[%lu]: Hello World da thread criada pela thread principal!\n", (long int) pthread_self());
    pthread_exit(NULL);
}
