#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

static unsigned int tiks;
static unsigned int poweroff;

void signal_handler(int sig) {
    switch(sig) {
        case SIGTERM:
        case SIGINT:
            poweroff = 1;
            break;
        case SIGALRM:
            tiks++;
            break;
        default:
            break;
    }
}

int setup_signal(int sig) {
    int ret = 0;
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = signal_handler;

    ret = sigaction(sig, &sa, NULL);
    if(ret)
        perror("sigaction");

    return ret;
}

int setup_timer(void) {
    struct itimerval itv;

    itv.it_value.tv_sec = 1;
    itv.it_value.tv_usec = 0;

    itv.it_interval.tv_sec = 1;
    itv.it_interval.tv_usec = 0;

    return setitimer(ITIMER_REAL, &itv, NULL);
}

int main(void) {
    setup_signal(SIGINT);
    setup_signal(SIGTERM);
    setup_signal(SIGALRM);
    setup_timer();

    while(1) {
        sleep(1);
        if(poweroff) {
            printf("Tiks: %u\n", tiks);
            break;
        }
    }

    return EXIT_SUCCESS;
}