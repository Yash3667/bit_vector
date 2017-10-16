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

#define err_print(format, ...)  \
    fprintf(stderr, "[%d:%s:%d]" format, errno, strerror(errno), __LINE__, ##__VA_ARGS__)

int 
main(void)
{
    char *test_str = NULL;
    bit_vector_t *test_vec = NULL;
    bit_vector_t *array_vec, *stream_vec;
    array_vec = stream_vec = NULL;

    array_vec = bit_vector_create(BIT_VECTOR_TYPE_ARRAY, VEC_SIZE);
    if (!array_vec) {
        err_print("create failed {array}\n");
    }

    stream_vec = bit_vector_create(BIT_VECTOR_TYPE_STREAM, 0);
    if (!stream_vec) {
        err_print("create failed {stream}\n");
    }

    bit_vector_print(array_vec);
    bit_vector_print(stream_vec);

    bit_vector_set(array_vec, 0);
    bit_vector_set(array_vec, 2);
    bit_vector_set(array_vec, 3);
    bit_vector_set(array_vec, 10);
    bit_vector_set(array_vec, 2);    
    bit_vector_append_bit(stream_vec, 1);

    bit_vector_print(array_vec);
    bit_vector_print(stream_vec);

    bit_vector_clear(array_vec, 0);
    bit_vector_resize(array_vec, 4);
    bit_vector_print(array_vec);

    bit_vector_append_bit(stream_vec, 1);
    bit_vector_append_bit(stream_vec, 1);
    bit_vector_append_bit(stream_vec, 1);
    bit_vector_print(stream_vec);

    bit_vector_append_bit(stream_vec, 1);
    bit_vector_print(stream_vec);

    bit_vector_free(array_vec);
    bit_vector_free(stream_vec);

    test_vec = bit_vector_string_to_vector("111000");
    test_str = bit_vector_vector_to_string(test_vec);

    printf("%s\n", test_str);

    return 0;
}
