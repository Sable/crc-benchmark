#ifndef CRC_FORMATS_H
#define CRC_FORMATS_H

#include<stdio.h>
#include<stdlib.h>
#include "common_args_serial.h"

unsigned int* read_crc(unsigned int* num_pages,unsigned int* page_size,const char* file_path);
void write_crc(const unsigned int** pages, const unsigned int num_pages, const unsigned int page_size,const char* file_path);
void free_crc(unsigned int** pages, const unsigned int num_pages);
unsigned int* rand_crc(unsigned int num_pages,unsigned int page_size);

#endif
