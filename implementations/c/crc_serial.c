/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014, Erick Lavoie, Faiz Khan, Sujay Kathrotia, Vincent
 * Foley-Bourgon, Laurie Hendren
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */


#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <getopt.h>
#include "common.h"
#include "crc_formats.h"
#include "eth_crc32_lut.h"


// /////Slice-by-8 version of CRC///////////////////////
////////altered from the version posted online by Stephan Brumme////////
// Copyright (c) 2013 Stephan Brumme. All rights reserved.
// see http://create.stephan-brumme.com/disclaimer.html
//
uint32_t crc32_8bytes(const void* data, size_t length){
    uint32_t* current = (uint32_t*) data;
    uint32_t crc = 0xFFFFFFFF;
    while (length >= 8) // process eight bytes at once
    {
        uint32_t one = *current++ ^ crc;
        uint32_t two = *current++;
        crc = crc32Lookup[7][ one      & 0xFF] ^
            crc32Lookup[6][(one>> 8) & 0xFF] ^
            crc32Lookup[5][(one>>16) & 0xFF] ^
            crc32Lookup[4][ one>>24        ] ^
            crc32Lookup[3][ two      & 0xFF] ^
            crc32Lookup[2][(two>> 8) & 0xFF] ^
            crc32Lookup[1][(two>>16) & 0xFF] ^
            crc32Lookup[0][ two>>24        ];
        length -= 8;
    }

    unsigned char* currentChar = (unsigned char*) current;
    while (length--) { // remaining 1 to 7 bytes
        crc = (crc >> 8) ^ crc32Lookup[0][(crc & 0xFF) ^ *currentChar++];
    }
    return ~crc;
}

void usage()
{
    fprintf(stderr, "crc [-s <page_size>] [-n <num_pages>] [-r <num_execs>] \n");
    fprintf(stderr, "Common arguments:\n");
    fprintf(stderr, "Program-specific arguments:\n");
    fprintf(stderr, "\t-h | 'Print this help message'\n");
    fprintf(stderr, "\t-n: Random Generation: Create <num_pages> pages - Default is 1\n");
    fprintf(stderr, "\t-s: Random Generation: Set # of bytes with each page to <page_size> - Default is 1024\n");
    fprintf(stderr, "\t-r: Specify the number of times the benchmark should be run\n");
    exit(0);
}

int main(int argc, char** argv){
    unsigned int *h_num;
    unsigned int i,j,num_pages=1,num_execs=1;
    int c;
    unsigned int page_size=100, num_words;
    unsigned int* crcs;
    unsigned int final_crc, expected_crc;
    double cumulative_time=0;
    stopwatch sw;

    while((c = getopt(argc, argv, "hs:n:r:")) != -1)
    {
        switch(c)
        {
        case 'h':
            usage();
            exit(0);
            break;
        case 's':
            page_size = atoi(optarg);
            break;
        case 'n':
            num_pages = atoi(optarg);
            break;
        case 'r':
            num_execs = atoi(optarg);
            break;
        default:
            fprintf(stderr, "Invalid argument: '%s'\n\n",optarg);
            usage();
        }
    }

    if ((page_size % 8) != 0) {
        fprintf(stderr, "Unsupported page size of '%u', please choose a page size that is a multiple of 8\n", page_size);
        exit(1);
    }

    num_words = page_size / 4;
    h_num = rand_crc(num_pages, page_size);
    crcs = malloc(sizeof(*crcs)*num_pages);

    expected_crc = 2231263667;
    stopwatch_start(&sw);
    for (j=0; j<num_execs; j++)
    {
        for(i=0; i<num_pages; i++)
        {
            crcs[i] = crc32_8bytes(&h_num[i*num_words], page_size);
        }

        // Self-checking crc results
        final_crc = crc32_8bytes(crcs, sizeof(*crcs)*num_pages);
        if (page_size == 65536 && num_pages == 128) {
            if (final_crc != expected_crc)
            {
                fprintf(stderr, "Invalid crc check, received '%u' while expecting '%u'\n", final_crc, expected_crc);
                return 1;
            }
        }
    }
    stopwatch_stop(&sw);

    // Moved out of the loop to avoid slowing down the benchmark with IO in case
    // no self-checking is performed
    if (!(page_size == 65536 && num_pages == 128)){
        fprintf(stderr, "WARNING: no self-checking step for page_size '%u' and num_pages '%u'\n", page_size, num_pages);
    }

    cumulative_time += get_interval_by_sec(&sw);


    printf("{ \"status\": %d, \"options\": \"-n %d -s %d -r %d\", \"time\": %f }\n", 1, num_pages, page_size, num_execs, get_interval_by_sec(&sw));

    free(h_num);
    return 0;
}
