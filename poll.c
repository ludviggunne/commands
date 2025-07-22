#include <stdio.h>
#include <stdlib.h>
#include <poll.h>
#include <string.h>

int main(int argc, char **argv)
{
  int nfds = 0;

  for (int i = 1; i < argc; ++i) {
    const char *arg = argv[i];

    if (strcmp(arg, "-i") == 0 || strcmp(arg, "-o") == 0)
      continue;

    nfds++;
  }

  if (nfds == 0) {
    fprintf(stderr, "%s: No file descriptors provided\n", argv[0]);
    exit(1);
  }

  struct pollfd *pollfds = malloc(nfds * sizeof(*pollfds));
  int flag = POLLIN;
  int pi = 0;

  for (int i = 1; i < argc; ++i) {
    const char *arg = argv[i];

    if (strcmp(arg, "-i") == 0) {
      flag = POLLIN;
      continue;
    }

    if (strcmp(arg, "-o") == 0) {
      flag = POLLOUT;
      continue;
    }

    pollfds[pi].fd = atoi(arg);
    pollfds[pi].events = flag;
    pi++;
  }

  int nready = poll(pollfds, nfds, -1);

  if (nready < 0) {
    perror("poll");
    exit(1);
  }

  if (nready == 0)
    exit(0);

  for (int i = 0; i < nfds; ++i) {
    if (pollfds[i].revents & POLLNVAL) {
      fprintf(stderr, "%s: Invalid file descriptor: %d\n", argv[0], pollfds[i].fd);
      exit(1);
    }

    if (pollfds[i].revents & POLLHUP) {
      fprintf(stderr, "%s: Broken pipe: %d\n", argv[0], pollfds[i].fd);
      exit(1);
    }

    if (pollfds[i].revents & POLLERR) {
      fprintf(stderr, "%s: An error ocurred on device or stream: %d\n", argv[0], pollfds[i].fd);
      exit(1);
    }

    if (pollfds[i].revents & (POLLIN|POLLOUT))
      printf("%d\n", pollfds[i].fd);
  }
}
