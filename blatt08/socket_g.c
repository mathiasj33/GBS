#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>

int main(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server, client;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(10000); //htons macht richtige Netzwerkformatierung (Big Endian etc. abhängig vom Rechner) -> also mach abhängig vom netzwerk

    bind(fd, (struct sockaddr*) &server, sizeof(server));

    listen(fd, 10);

    int len = sizeof(struct sockaddr_in);
    int new_socket = accept(fd, (struct sockaddr*) &client, (socklen_t *)&len);

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