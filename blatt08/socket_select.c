#define _GNU_SOURCE

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

int create_unix_socket() {
  // create unix socket
  struct sockaddr_un addr;
  int ret = 0, s = socket (PF_UNIX, SOCK_STREAM, 0);
  int set = 1;
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &set, sizeof(set));
  memset(&addr, 0, sizeof (addr));
  addr.sun_family = AF_UNIX;
  strcpy (addr.sun_path, "/tmp/my-unix-socket");
  unlink (addr.sun_path);
  socklen_t alen = strlen(addr.sun_path) + sizeof(addr.sun_family);
  ret = bind (s, (struct sockaddr*) &addr, alen);
  if (0 > ret) {
    perror ("bind() failed with");
    exit (EXIT_FAILURE);
  }
  ret = listen(s, 10);
  if (0 > ret) {
    perror ("listen() failed with");
    exit (EXIT_FAILURE);
  }
  return s;
}

int create_inet_socket() {
  // create inet socket
  struct sockaddr_in addr;
  int ret, s = socket (AF_INET, SOCK_STREAM, 0);
  memset (&addr, 0, sizeof (addr));
  //Prepare the sockaddr_in structure
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons( 10000 );
  socklen_t alen = sizeof(struct sockaddr_in);
  ret = bind (s, (struct sockaddr*) &addr, alen);
  if (0 > ret) {
    perror ("bind() failed with");
    exit (EXIT_FAILURE);
  }
  ret = listen(s, 10);
  if (0 > ret) {
    perror ("listen() failed with");
    exit (EXIT_FAILURE);
  }
  return s;
}

void add_fd_to_list(int fd, int** fds, int* nrfds){
  if (fd > 0) {
    int i;
    // go through the list and find a closed socket
    for(i = 0; i < *nrfds ; i++){
      if((*fds)[i] == -1) break;
    }
    if(i == *nrfds){
      // We need to create new space for the file descriptor
      *fds = (int*)realloc(*fds, ++(*nrfds)*sizeof(int));
    }
    (*fds)[i] = fd;
  }
}

#define MAX(a,b) ((a) > (b) ? (a) : (b))

int main (void){
  int nrfds = 0;
  int *fds = 0;

  int s = create_unix_socket();
  int s_ip = create_inet_socket();

  char c;
  int ret;
  while (1) {

    fd_set rs;
    FD_ZERO(&rs);
    FD_SET(s,&rs);
    FD_SET(s_ip,&rs);
    int max = (s > s_ip) ? (s) : (s_ip);
    int i = 0;
    for(i = 0; i < nrfds ; i++){
      if(fds[i] > 0) {
        FD_SET(fds[i],&rs);
        if(max < fds[i]) max = fds[i];
      }
    }
    select(max + 1, &rs, NULL, NULL, NULL);
    if(FD_ISSET(s, &rs)){
      int fd = accept(s, NULL, NULL);
      add_fd_to_list(fd, &fds, &nrfds);
    } else if(FD_ISSET(s_ip, &rs)){
      int fd = accept(s_ip, NULL, NULL);
      add_fd_to_list(fd, &fds, &nrfds);
    } else {
      for(i = 0; i < nrfds ; i++){
        if(fds[i] > 0 && FD_ISSET(fds[i], &rs)){
          // socket -> stdout
          ret = read(fds[i], &c, 1);
          if(ret > 0) {
            printf("%c", c);
            fflush(stdout);
          } else if (ret < 1 && errno != EAGAIN){
            close(fds[i]);
            fds[i] = -1;
          }
        }
      }
    }
  }
  close (s);
  close (s_ip);
}
