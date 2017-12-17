#define _GNU_SOURCE

#include <termios.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

int toggle_echo(void) {
    struct termios t;

    if (tcgetattr (1, &t) != 0) return -1;

    t.c_lflag ^= ECHO;
    t.c_lflag ^= ICANON;
    t.c_cc[VMIN] = 1;
    t.c_cc[VTIME] = 0;

    if(tcsetattr(1, TCSAFLUSH, &t) != 0) return -1;
    return 0;
}

void *thread_write(void *arg) {
    int fd = *(int*) arg;
    int ret;
    char c;
    while(1) {
        ret = read(0, &c, 1);
        if(ret < 0) return 0;
        write(fd, &c, 1);
    }
    return NULL;
}

void *thread_read(void *arg) {
    int fd = *(int*) arg;
    int ret;
    char c;
    while(1) {
        ret = read(fd, &c, 1);
        printf("%c", c);
        if(ret < 0) return 0;
    }
}

int main(void) {
    toggle_echo();
    int fds[2];
    pipe(fds);

    pthread_t writeThread;
    pthread_create(&writeThread, NULL, thread_write, (void*) &(fds[1]));
    pthread_t readThread;
    pthread_create(&readThread, NULL, thread_read, (void*) &(fds[0]));

    pthread_join(writeThread, NULL);
    pthread_join(readThread, NULL);
}