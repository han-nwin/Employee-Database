#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <string.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
  printf("Usage: %s -n -f <database file>\n", argv[0]);
  printf("\t -n - create new database file\n");
  printf("\t -f - (require) path to database file\n");
  return;
}

int main (int argc, char *argv[]) {

  char *filepath = NULL;
  char *addstring = NULL;
  bool newfile = false;
  int c;

  int db_fd = -1;
  struct employee_t *employees = NULL;
  struct dbheader_t *db_hd = NULL;
  bool list = false;

  while ((c = getopt(argc, argv, "nf:a:l")) != -1) {
    switch (c) {
      case 'n':
        newfile = true;
        break;
      case 'f':
        filepath = optarg;
        break;
      case 'a':
        addstring = optarg;
        break;
      case 'l':
        list = true;
        break;
      case '?':
        printf("Unknown option -%c\n", c);
        break;
      default:
        return -1;
    }
  }

  if (filepath == NULL) {
    printf("Filepath is a required argument\n");
    print_usage(argv);

    return 0;
  }

  if (newfile) {
    
    db_fd = create_db_file(filepath);
    if (db_fd == STATUS_ERROR) {
      printf("Unable to create database file\n");
      return -1;
    }
   
    if (create_db_header(db_fd, &db_hd) == -1) {
      printf("Failed to create database header\n");
      return -1;

    }


  } else {
    db_fd = open_db_file(filepath);
    if (db_fd == STATUS_ERROR) {
      printf("Unable to open database file\n");
      return -1;
    }

    if (validate_db_header(db_fd, &db_hd) == STATUS_ERROR) {
      printf("Failed to validate database header\n");
      return -1;
    }
  }

  if (read_employees(db_fd, db_hd, &employees) != STATUS_SUCCESS) {
    printf("Failed to read employees\n");
    return -1;
  }

  if (addstring) {
    db_hd->count++;
    employees = realloc(employees, db_hd->count*sizeof(struct employee_t));
    if (add_employee(db_hd, employees, addstring) != STATUS_SUCCESS) {
      return -1;
    };
  }

  if (list) {
    list_employees(db_hd, employees);
  }

  printf("------------------------\n");
  printf("Newfile: %d\n", newfile);
  printf("Filepath: %s\n", filepath);

  //Write to db file
  output_file(db_fd, db_hd, employees);

  return 0;

}
