#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

#include "parse.h"
#include "common.h"


/**
 * @brief Take a file descriptor and allocate memory for it to create a dbheader_t type file
 * */
int create_db_header(int fd, struct dbheader_t **headerOut) {
  //Allocate memory and initialize with '0'
  struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));

  if (header == NULL) {
    printf("Calloc failed to create db header\n");
    return STATUS_ERROR;
  }

  header->version = 0x1;
  header->count = 0;
  header->magic = HEADER_MAGIC;
  header->filesize = sizeof(struct dbheader_t);

  *headerOut = header;

  return STATUS_SUCCESS;
}


/**
 * @brief Take a file desciptor and validate if the file is already exist (type dbheader_t)
 * */
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

  *headerOut = header;

  return STATUS_SUCCESS;
}



/** 
 * @brief Take an output on disk file descriptor and write the content of the file
 *
 * */
int output_file(int fd, struct dbheader_t *db_hd, struct employee_t *employees) {
  if (fd < 0) {
    printf("Got a bad file descriptor from user\n");
    return STATUS_ERROR;
  }
  int realcount = db_hd->count; //reserve the host data byte sequence

  db_hd->magic = htonl(db_hd->magic);
  db_hd->filesize = htonl(sizeof(struct dbheader_t) + sizeof(struct employee_t)*realcount);
  db_hd->version = htons(db_hd->version);
  db_hd->count = htons(db_hd->count);

  lseek(fd, 0, SEEK_SET);

  write(fd, db_hd, sizeof(struct dbheader_t));

  int i = 0;
  for (; i < realcount; i++) {
    employees[i].hours = htonl(employees[i].hours);
    write(fd, &employees[i], sizeof(struct employee_t));
  }

  return STATUS_SUCCESS;

}


/**
 * @brief Read employee structure data
 * */
int read_employees(int fd, struct dbheader_t *db_hd, struct employee_t **employeesOut) {
  if (fd < 0) {
    printf("Got a bad file descriptor from user \n");
    return STATUS_ERROR;
  }

  int count = db_hd->count;

  struct employee_t *employees = calloc(count, sizeof(struct employee_t));
  if (employees == NULL) {
    printf("Calloc failed \n");
    return STATUS_ERROR;
  }

  read(fd, employees, count*sizeof(struct employee_t));

  int i = 0;
  for (; i < count; i++) {
    employees[i].hours = ntohl(employees[i].hours);
  }

  *employeesOut = employees;
   return STATUS_SUCCESS;

}

/**
 * @brief Function to add employees to db 
 * */
int add_employee(struct dbheader_t *db_hd, struct employee_t *employees, char *addstring) {

  char *name = strtok(addstring, ",");
  char *address = strtok(NULL, ",");
  char *hours = strtok(NULL, ",");

  printf("%s | %s | %s\n", name, address, hours);

  strncpy(employees[db_hd->count - 1].name, name, sizeof(employees[db_hd->count - 1].name));
  strncpy(employees[db_hd->count - 1].address, address, sizeof(employees[db_hd->count - 1].address));
  employees[db_hd->count - 1].hours = atoi(hours);


  return STATUS_SUCCESS;
}



/**
 * @brief List employees to stdin
 * */
void list_employees(struct dbheader_t *db_hd, struct employee_t *employees) {
  int i = 0;
  for (; i < db_hd->count; i++) {
    printf("\nEmployee: %d\n", i);
    printf("Name: %s\n", employees[i].name);
    printf("Address: %s\n", employees[i].address);
    printf("Hours: %d\n", employees[i].hours);
  }
}
