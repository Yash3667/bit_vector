/* @file Bit vector source
 * This is implementation for the API presented in the
 * corresponding header. 
 *
 * Author: Yash Gupta <ygupta@ucsc.edu>
 * Copyright: Yash Gupta
 */
#include "bit_vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/**
 * Create and initialize a bit vector with its default values
 * depending on the type. The length is ignored in case type is
 * of BIT_VECTOR_TYPE_STREAM.
 *
 * @param   type        The type of the bit vector.
 * @param   length      The length of the bit vector we need.
 *
 * @return  A bit vector
 * @return  NULL        Not enough memory.
 */
bit_vector_t* 
bit_vector_create(bit_vector_type type, uint64_t length);
{
    const uint64_t stream_vector_size = 4;
    bit_vector_t *vector;
    uint64_t temp_length;

    if (length == 0) {
        return NULL;
    }

    vector = malloc(sizeof *vector);
    if (!vector) {
        return NULL;
    }

    /*
     * When working with a stream vector, we do not care about
     * the length as we work with a four byte vector and increase
     * the length as required.
     */
    
    if (type == BIT_VECTOR_TYPE_STREAM) {
        temp_length = stream_vector_size;
    } else {
        temp_length = length;
    }

    vector->array = calloc(BIT_VECTOR_BITS_TO_BYTES(temp_length) + 1, sizeof *(vector->array));
    if (!(vector->array)) {
        free(vector);
        return NULL;
    }

    vector->length = temp_length;
    vector->index = 0;
    vector->vector_type = type;

    return vector;
}

/**
 * Clear all the memory being utilized by the bit vector.
 *
 * @param   vector      The bit vector to free
 */
void
bit_vector_free(bit_vector_t *vector)
{
    /*
     * Not performing error checking here as I have no 
     * way to report back an error. Better that a segmentation 
     * fault occurs rather than a silent return.
     */ 

    free(vector->array);
    free(vector);
}

/**
 * Set a specific bit in the bit vector. Like any array, the index
 * begins from 0.
 *
 * @param   vector      The vector to set bit for.
 * @param   index       The index of the bit to set.
 *
 * @return   0          No error.
 * @return  -1          Error with errno set.
 *  EINVAL: vector is NULL.
 *  EINVAL: index is beyond vector length.
 */
int 
bit_vector_set(bit_vector_t *vector, uint64_t index)
{
    uint8_t or_bits;
    
    if (!vector || index >= vector->length) {
        errno = -EINVAL;
        return -1;
    }
    
    or_bits = 1 << BIT_VECTOR_GET_BIT_INDEX(index);
    vector->array[BIT_VECTOR_GET_BYTE_INDEX(index)] |= or_bits;

    return 0;
}

/**
 * Clear a specific bit in the bit vector. Like any array, the index
 * begins from 0.
 *
 * @param   vector      The vector to clear bit for.
 * @param   index       The index of the bit to clear.
 *
 * @return   0          No error.
 * @return  -1          Error with errno set.
 *  EINVAL: vector is NULL.
 *  EINVAL: index is beyond vector length.
 */
int 
bit_vector_clear(bit_vector_t *vector, uint64_t index);
{
    uint8_t and_bits;

    if (!vector || index >= vector->length) {
        errno = -EINVAL;
        return -1;
    }

    and_bits = ~(1 << BIT_VECTOR_GET_BIT_INDEX(index));
    vector->array[BIT_VECTOR_GET_BYTE_INDEX(index)] &= and_bits;

    return 0;
}

/**
 * Acquire the state of a bit in the bit vector. Like any array, the index
 * begins from 0.
 *
 * @param   vector      The vector to acquire bit for.
 * @param   index       The index of the bit to acquire.
 *
 * @return   1          Bit is set.
 * @return   0          Bit is clear.
 * @return  -1          Error with errno set.
 *  EINVAL: vector is NULL.
 *  EINVAL: index is beyond vector length.
 */
int 
bit_vector_get(bit_vector_t *vector, uint64_t index);
{
    int8_t return_bits;
    int8_t and_bits;
    
    if (!vector || index >= vector->length) {
        errno = -EINVAL;
        return -1;
    }

    and_bits = 1 << BIT_VECTOR_GET_BIT_INDEX(index);
    return_bits = vector->array[BIT_VECTOR_GET_BYTE_INDEX(index)] & and_bits;
    return !!return_bits;
}

/**
 * Resize a vector so it may hold more or fewer bits
 * (Usually more).
 *
 * @param   vector      The bit vector to resize.
 * @param   length      The new length of the bit vector.
 *
 * @return  Resized bit vector.
 * @return  NULL        Resize failed.
 */
bit_vector_t*
bit_vector_resize(bit_vector_t *vector, uint64_t length)
{
    uint8_t *temp_vector;

    if (!vector) {
        errno = -EINVAL;
        return NULL;
    }

    temp_vector = realloc(vector->array, (length / BIT_VECTOR_BITS_IN_BYTE) + 1);
    if (!temp_vector) {
        return NULL;
    } else {
        vector->array = temp_vector;
        vector->length = length;
    }

    return vector;
}

/**
 * Append a single bit to a bit vector. Used when working with a
 * vector of type BIT_VECTOR_TYPE_STREAM.
 *
 * @param   vector      The vector to which we want to append a bit.
 * @param   bit         The bit to append.
 *
 * @return  A bit vector.
 * @return  NULL        Invalid parameter.
 * @return  NULL        Resize failed.
 */
bit_vector_t*
bit_vector_append_bit(bit_vector_t* vector, uint8_t bit)
{
    if (!vector) {
        errno = -EINVAL;
        return NULL;
    } else if (bit > BIT_VECTOR_STATE_SET) {
        errno = -EINVAL;
        return NULL;
    } else if (vector->vector_type == BIT_VECTOR_TYPE_ARRAY) {
        errno = -EINVAL;
        return NULL;
    }

    if (vector->index == vector->length) {
        if (bit_vector_resize(vector, vector->length*2) == NULL) {
            return NULL;
        }
    }

    if (bit == BIT_VECTOR_STATE_CLEAR) {
        bit_vector_clear_bit(vector, vector->index);
    } else {
        bit_vector_set_bit(vector, vector->index);
    }
    vector->index += 1;

    return vector;
}

/**
 * Append a bit string to a bit vector. Used when working with a
 * vector of type BIT_VECTOR_TYPE_STREAM.
 *
 * @param   vector      Vector to append string to.
 * @param   bit_string  A C-style string with 1's and 0's.
 *
 * @return  A bit vector.
 * @return  NULL        Invalid parameters.
 * @return  NULL        Append failed.       
 */
bit_vector_t* 
bit_vector_append_string(bit_vector_t *vector, char *bit_string)
{
    const uint8_t ascii_zero_value = 0x30;
    uint32_t i;

    if (!vector || !bit_string) {
        errno = -EINVAL;
        return NULL;
    } else if (vector->vector_type == BIT_VECTOR_TYPE_ARRAY) {
        errno = -EINVAL;
        return NULL;
    }

    for (i = 0; i < strlen(bit_string); i++) {
        if (bit_vector_append_bit(vector, bit_string[i] - ascii_zero_value) == NULL) {
            return NULL;
        }
    }

    return vector;
}

/**
 * Append a bit vector to a bit vector. Used when working with a
 * vector of type BIT_VECTOR_TYPE_STREAM. If the destination vector 
 * is of type BIT_VECTOR_TYPE_STREAM, only then is the source copied.
 *
 * @param   dest    The vector to which we append.
 * @param   src     The vector which we copy.
 * @param   size    The max size to copy. If this is zero, then
 *                  for a stream vector, everything until its index
 *                  is copied and for an array vector, its entire 
 *                  length is copied.
 *
 * @return  The destination vector.
 * @return  NULL    Invalid paramters.
 * @return  NULL    Append failed.
 */
bit_vector_t*
bit_vector_append_vector(bit_vector_t *src, bit_vector_t *src, uint64_t size)
{
    uint64_t i, append_length;
    uint8_t bit;

    if (!dest || !src) {
        errno = -EINVAL;
        return NULL;
    } else if (dest->vector_type == BIT_VECTOR_TYPE_ARRAY) {
        errno = -EINVAL;
        return NULL;
    }

    append_length = (src->vector_type == BIT_VECTOR_TYPE_ARRAY)? src->length: src->index;
    if (size < append_length) {
        append_length = size;
    }

    for (i = 0; i < append_length; i++) {
        bit = bit_vector_get(src, i);
        if (bit_vector_append_bit(dest, bit) == NULL) {
            return NULL;
        }
    }

    return dest;
}

/**
 * Convert a C-style bit string into a bit vector. The
 * resulting vector is of type BIT_VECTOR_TYPE_STREAM.
 *
 * @param   bit_string      The bit string to convert into a vector.
 *
 * @return  A bit vector.
 * @return  NULL            Memory allocation failed.
 * @return  NULL            String to vector copy failed.
 */
bit_vector_t* 
bit_vector_string_to_vector(char *bit_string)
{
    bit_vector_t *this_vector;

    this_vector = bit_vector_create(BIT_VECTOR_TYPE_STREAM, 0);
    if (!this_vector) {
        return NULL;
    }

    return bit_vector_append_string(this_vector, bit_string);
}

/**
 * Convert a bit vector into a C-style string. It is the users
 * responsibility to free this string.
 *
 * @param   vector      The vector to convert into a string.
 *
 * @return  A C string.
 * @return  NULL        Invalid paraeters.
 * @return  NULL        Memory allocation failed.
 */
char* 
bit_vector_vector_to_string(bit_vector_t *vector)
{
    const uint8_t ascii_zero_value = 0x30;
    char *this_string;
    uint64_t i, string_length;

    if (!vector) {
        errno = -EINVAL;
        return NULL;
    }

    string_length = (vector->vector_type == BIT_VECTOR_TYPE_ARRAY)? vector->length: vector->index;
    this_string = calloc(string_length+1, sizeof *this_string);
    if (!this_string) {
        return NULL;
    }

    for (i = 0; i < string_length; i++) {
        this_string[i] = bit_vector_get(vector, i) + ascii_zero_value;
    }
    this_string[string_length] = '\0';

    return this_string;
}

/**
 * Print a vector to stdout.
 *
 * @param   vector  The vector to print.
 */
void
bit_vector_print(bit_vector_t *vector)
{
    const uint8_t ascii_zero_value = 0x30;
    uint64_t i, print_length;

    /*
     * Not performing error checking here as I have no 
     * way to report back an error. Better that a segmentation 
     * fault occurs rather than a silent return.
     */
    
    print_length = (vector->vector_type == BIT_VECTOR_TYPE_ARRAY)? vector->length: vector->index;
    for (i = 0; i < print_length; i++) {
        printf("%u", bit_vector_get(vector, i) + ascii_zero_value);
    }
    printf("\n");
}

/**
 * Output a bit vector onto a file descriptor. The semantics
 * are acquired from the type of the vector.
 *
 * @param   vector  The bit vector you want to output.
 * @param   fd      The file you want to output to.
 * @param   offset  The offset in file to output to.
 *
 * @return  The next writing offset
 * @return  -1      Error with errno set.
 */
ssize_t
bit_vector_file_output(bit_vector_t *vector, int fd, uint64_t offset)
{
    uint64_t length;
    ssize_t size_to_write;
    ssize_t bytes_written;

    if (!vector) {
        errno = -EINVAL;
        return -1;
    }

    /**
     * The format outputted to the file is simple. It is the
     * length (in bits) followed by the vector. In case of a STREAM vector,
     * the vector is outputted until the index.
     */
    length = (vector->vector_type == BIT_VECTOR_TYPE_ARRAY)? vector->length: vector->index;

    size_to_write = sizeof length;
    bytes_written = pwrite(fd, &length, size_to_write, offset);
    if (bytes_written < size_to_write) {
        return -1;
    } else {
        offset += bytes_written;
    }

    // TODO: Ouput vector type.

    length = (length / BIT_VECTOR_BITS_IN_BYTE) + 1;
    size_to_write = length;
    bytes_written = pwrite(fd, vector->array, size_to_write, offset);
    if (bytes_written < size_to_write) {
        return -1;
    } else {
        offset += bytes_written;
    }

    return offset;
}

/**
 * Input a bit vector from a file descriptor. The semantics
 * are acquired from the type of the vector.
 *
 * @param   fd      The file you want to output to.
 * @param   offset  The offset in file to output to. The
 *                  next offset to read from is returned
 *                  in this.
 *
 * @return  A bit vector.
 * @return  NULL    Error with errno set.
 */
bit_vector_t*
bit_vector_file_input(int fd, uint64_t *offset)
{
    bit_vector_t *vector;
    uint64_t length;
    ssize_t size_to_read;
    ssize_t bytes_read;

    /* The format is the length (in bits) followed by the vector */
    size_to_read = sizeof length;
    bytes_read = pread(fd, &length, size_to_read, offset);
    if (bytes_read < size_to_read) {
        return NULL;
    } else {
        *offset += bytes_read;
    }

    vector = bit_vector_create(length, BIT_);
    if (!vector) {
        return NULL;
    } else {
        vector->working_index = length;
    }

    /* Read in the entire vector */
    length = (length / VECTOR_BYTE_SIZE) + 1;
    size_to_read = length;
    bytes_read = pread(fd, vector->vector, size_to_read, offset);
    if (bytes_read < size_to_read) {
        return NULL;
    }

    return vector;
}