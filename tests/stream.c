/* @file Bit vector test stream
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
#include <time.h>

#define SIZE    64

#define err_print(format, ...)  \
    fprintf(stderr, "[%d:%s:%d]" format, errno, strerror(errno), __LINE__, ##__VA_ARGS__)

int 
main(void)
{
    int fd, i;
    uint64_t read_offset = 0;
    bit_vector_t *stream_vec, *read_vec;
    char *str_vec;
    stream_vec = read_vec = NULL;

    stream_vec = bit_vector_create(BIT_VECTOR_TYPE_STREAM, VEC_SIZE);
    if (!stream_vec) {
        err_print("create failed\n");
        return errno;
    }

    printf("init\n");
    bit_vector_print(stream_vec);
    srand(time(0));

    for (i = 0; i < SIZE; i++) {
        bit_vector_append_bit(stream_vec, rand() % 2);
    }

    printf("64 random appends\n");
    bit_vector_print(stream_vec);

    printf("Detach: %d\n", bit_vector_detach(stream_vec));
    printf("Detach: %d\n", bit_vector_detach(stream_vec));
    printf("Detach: %d\n", bit_vector_detach(stream_vec));
    printf("Detach: %d\n", bit_vector_detach(stream_vec));
    bit_vector_print(stream_vec);

    bit_vector_free(stream_vec);
    stream_vec = bit_vector_string_to_vector("1001100");

    printf("str to vec\n");
    bit_vector_print(stream_vec);

    str_vec = bit_vector_vector_to_string(stream_vec);
    printf("String: %s\n", str_vec);

    bit_vector_append_string(stream_vec, str_vec);
    printf("str append\n");
    bit_vector_print(stream_vec);

    bit_vector_append_vector(stream_vec, stream_vec, 0);
    printf("vec append\n");    
    bit_vector_print(stream_vec);

    fd = open("stream_test", O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (fd == -1) {
        err_print("open failed {write}\n");
        return errno;
    }
    bit_vector_file_output(stream_vec, fd, 0);
    close(fd);

    fd = open("stream_test", O_RDONLY);
    if (fd == -1) {
        err_print("open failed {read}\n");
        return errno;
    }
    read_vec = bit_vector_file_input(fd, &read_offset);
    close(fd);

    printf("read_vec\n");
    bit_vector_print(read_vec);

    printf("stream_vec\n");
    bit_vector_print(stream_vec);

    bit_vector_free(read_vec);
    bit_vector_free(stream_vec);
    return 0;
}