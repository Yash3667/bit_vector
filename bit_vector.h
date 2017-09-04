/* @file Bit vector header
 * This is the public header for the bit vector data structure
 * and presents the data structure interface.
 *
 * Author: Yash Gupta <ygupta@ucsc.edu>
 * Copyright: Yash Gupta
 */
#include <stdint.h>

#ifndef _BIT_VECTOR_H_
#define _BIT_VECTOR_H_

/* Macros */
// Unit conversion.
#define BIT_VECTOR_BITS_IN_BYTE             8
#define BIT_VECTOR_BITS_TO_BYTES(b)         (b / BIT_VECTOR_BITS_IN_BYTE)
#define BIT_VECTOR_BYTES_TO_BITS(b)         (b * BIT_VECTOR_BITS_TO_BYTES)

// Unit indexes for byte arrays.
//  Eg: For i==17, byte = 2 and bit = 1
#define BIT_VECTOR_GET_BYTE_INDEX(i)        (i / BIT_VECTOR_BITS_IN_BYTE)
#define BIT_VECTOR_GET_BIT_INDEX(i)         (i & 0x7)

#define VECTOR_SET_STRING(index)          (0x1 << VECTOR_BIT_INDEX(index))
#define VECTOR_CLEAR_STRING(index)        (~VECTOR_SET_STRING(index))
#define VECTOR_CHECK_STRING(index)        (VECTOR_SET_STRING(index))

/* Enumerations */
// Bit vector type.
typedef enum _bit_vector_type {
    BIT_VECTOR_TYPE_STREAM = 0,
    BIT_VECTOR_TYPE_ARRAY
} bit_vector_type;

// Bit vector state.
enum {
    BIT_VECTOR_STATE_CLEAR = 0,
    BIT_VECTOR_STATE_SET
};

/* Structures */
// Main structure for data structure.
typedef struct _bit_vector_t {
    /* 
     * We have used 8 bit unsigned integers as the source
     * of keeping the bits. This array has a respective field
     * which holds its size and another which holds its
     * stream index for when the vector is used as a stream and
     * not an array.
     */

    uint8_t *array;
    uint64_t length;     
    uint64_t index;
    bit_vector_type vector_type;
} bit_vector_t;

/**
 * Create and initialize a bit vector with its default values
 * depending on the type. The length is ignored in case type is
 * of BIT_VECTOR_TYPE_STREAM.
 */
bit_vector_t* bit_vector_create(bit_vector_type type, uint64_t length);

/**
 * Clear all the memory being utilized by the bit vector.
 */
void bit_vector_free(bit_vector_t *vector);

/**
 * Set a specific bit in the bit vector. Like any array, the index
 * begins from 0.
 */
int bit_vector_set(bit_vector_t *vector, uint64_t index);

/**
 * Clear a specific bit in the bit vector. Like any array, the index
 * begins from 0.
 */
int bit_vector_clear(bit_vector_t *vector, uint64_t index);

/**
 * Acquire the state of a bit in the bit vector. Like any array, the index
 * begins from 0.
 */
int bit_vector_get(bit_vector_t *vector, uint64_t index);

/**
 * Get the index of the bit vector. This function is mostly useless
 * for type of BIT_VECTOR_TYPE_ARRAY but is useful for when using
 * as a stream.
 */
int bit_vector_index(bit_vector_t *vector, uint64_t *size);

/**
 * Resize a vector so it may hold more or fewer bits.
 */
bit_vector_t* bit_vector_resize(bit_vector_t *vector, uint64_t length);

/**
 * Append a single bit to a bit vector. Used when working with a
 * vector of type BIT_VECTOR_TYPE_STREAM.
 */
bit_vector_t* bit_vector_append_bit(bit_vector_t *vector, uint8_t bit);

/**
 * Append a bit string to a bit vector. Used when working with a
 * vector of type BIT_VECTOR_TYPE_STREAM.
 */
bit_vector_t* bit_vector_append_string(bit_vector_t *vector, char *bit_string);

/**
 * Append a bit vector to a bit vector. Used when working with a
 * vector of type BIT_VECTOR_TYPE_STREAM. If the source vector 
 * is of type BIT_VECTOR_TYPE_STREAM. then only the length of the
 * stream is copied.
 */
bit_vector_t* bit_vector_append_vector(bit_vector_t *source, bit_vector_t *dest);

/**
 * Convert a C-style bit string into a bit vector.
 */
bit_vector_t* bit_vector_to_string(char *bit_string);

/**
 * Convert a bit vector into a C-style string.
 */
char* bit_vector_to_vector(bit_vector_t *vector);

/**
 * This function is used to print the bit vector depending
 * on the flag which is passed.
 */
void bit_vector_print(bit_vector_t*, uint8_t);

/**
 * This function is used to output a bit vector onto a file
 * after a specified offset. This is helpful when you want to
 * save a bit vector in a binary format.
 */
ssize_t bit_vector_output(bit_vector_t*, int, uint64_t, uint8_t);

/**
 * This function is used to input a bit vector from a file.
 * This function expects the file to be formatted the way
 * bit_vector_output emits. This is helpful for saving and restoring
 * state of the bit vector.
 */
bit_vector_t* bit_vector_input(int, uint64_t);

#endif