#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>







int create_db_file(char *filename) {
  int fd = open(filename, O_RDONLY);

  return fd;
}
