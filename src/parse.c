#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "parse.h"
#include "common.h"

int validate_db_header(int fd, struct dbheader_t **headerOut) {
  
  if(fd < 0) {
    printf("Bad file desciptor from user\n");
    return STATUS_ERROR;
  }

  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
  if (header == NULL) {
    printf("Calloc failed\n");
    return STATUS_ERROR;
  }

  if (read(fd, header, sizeof(struct dbheader_t)) != sizeof(struct dbheader_t)) {
    perror("read");
    free(header);
    return STATUS_SUCCESS;
  }

  header->version = ntohs(header->version);
  header->count = ntohs(header->count);
  header->magic = ntohl(header->magic);
  header->filesize = ntohl(header->filesize);

  if (header->magic != HEADER_MAGIC) {
    printf("Improper header magic \n");
    free(header);
    return STATUS_ERROR;
  }

  if (header->version != 1) {
    printf("Improper header magic\n");
    free(header);
    return STATUS_ERROR;
  }

//Check if the header filesize
  struct stat dbstat = {0};
  fstat(fd, &dbstat);

  if (header->filesize != dbstat.st_size) {
    printf("Corrupted database\n");
    free(header);
    return STATUS_ERROR;
  }


  return STATUS_SUCCESS;
}


int create_db_header(int fd, struct dbheader_t **headerOut) {
  //Allocate memory and initialize with '0'
  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));

  if (header == NULL) {
    printf("Calloc failed\n");
    return STATUS_ERROR;
  }

  header->version = 0x1;
  header->count = 0;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(struct dbheader_t);

  *headerOut = header;

  return STATUS_SUCCESS;
}
