/* @file Bit vector test array
 * Units tests for bit_vector data structure.
 *
 * Author: Yash Gupta <yash_gupta12@live.com>
 * Copyright: Yash Gupta
 * License: MIT Public License
 */
#include "../src/bit_vector.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

#define VEC_SIZE    64

#define err_print(format, ...)  \
    fprintf(stderr, "[%d:%s:%d]" format, errno, strerror(errno), __LINE__, ##__VA_ARGS__)

int 
main(void)
{
    int fd;
    uint64_t read_offset = 0;
    bit_vector_t *array_vec, *read_vec;
    array_vec = read_vec = NULL;

    array_vec = bit_vector_create(BIT_VECTOR_TYPE_ARRAY, VEC_SIZE);
    if (!array_vec) {
        err_print("create failed\n");
        return errno;
    }

    printf("init\n");
    bit_vector_print(array_vec);

    bit_vector_set(array_vec, 0);
    bit_vector_set(array_vec, 2);    
    bit_vector_set(array_vec, 10);
    bit_vector_set(array_vec, 12);
    bit_vector_set(array_vec, 63);

    printf("set 5 entries\n");
    bit_vector_print(array_vec);

    bit_vector_clear(array_vec, 0);
    bit_vector_clear(array_vec, 63);

    printf("clear 2 entries\n");
    bit_vector_print(array_vec);

    bit_vector_resize(array_vec, 8);
    printf("resize\n");
    bit_vector_print(array_vec);
    printf("Index 2: %d\n", bit_vector_get(array_vec, 2));

    fd = open("array_test", O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1) {
        err_print("open failed {write}\n");
        return errno;
    }
    bit_vector_file_output(array_vec, fd, 0);
    close(fd);

    fd = open("array_test", O_RDONLY);
    if (fd == -1) {
        err_print("open failed {read}\n");
        return errno;
    }
    read_vec = bit_vector_file_input(fd, &read_offset);
    close(fd);

    printf("read_vec\n");
    bit_vector_print(read_vec);

    printf("array_vec\n");
    bit_vector_print(array_vec);

    bit_vector_free(read_vec);    
    bit_vector_free(array_vec);
    return 0;
}