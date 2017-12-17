#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/un.h>
#define MY_FILE "/tmp/my-unix-socket"

int main(void) {
    int fd = socket(PF_UNIX, SOCK_STREAM, 0);
    unlink(MY_FILE); //delete previous file

    struct sockaddr_un address; //sockaddr_un steht für Unix
    address.sun_family = AF_UNIX;
    strcpy(address.sun_path, MY_FILE);

    bind(fd, (struct sockaddr*) &address, sizeof(address));

    listen(fd, 10);
    int len = sizeof(struct sockaddr_un);  //oder _in???
    int new_socket = accept(fd, (struct sockaddr*) &address, (socklen_t *)&len);

    char c;
    int ret;
    while(1) {
        ret = read(new_socket, &c, 1);
        printf("%c", c);
        fflush(stdout);
        if(ret <= 0) break;
    }
    close(new_socket);
    close(fd);
    printf("done");
}