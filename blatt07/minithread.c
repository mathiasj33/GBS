#define _GNU_SOURCE

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

void *thread_func(void *arg) {
    printf("Hello, my name is %s!\n", (char *) arg);
    pthread_exit(NULL);
}

int main(void) {
    const char *names[5] = {"up", "you", "give", "gonna", "never"};
    pthread_t tid[5]; //oder: [sizeof(names)/sizeof(*names)]
    int i;
    int ret;

    for (i = 0; i < 5; ++i) {
        ret = pthread_create(&tid[i], NULL, thread_func, (void *) names[4 - i]);
        if (ret != 0) {
            printf("Could not create thread. Exiting.");
            exit(1);
        }
    }

    pthread_yield();

    for (i = 0; i < 5; ++i) {
        ret = pthread_join(tid[i], NULL);
        if (ret != 0) {
            printf("Could not wait for thread. Exiting.");
            exit(1);
        }
    }

    return 0;
}