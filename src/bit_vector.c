/* @file Bit vector source
 * This is implementation for the API presented in the
 * corresponding header. 
 *
 * Author: Yash Gupta <yash_gupta12@live.com>
 * Copyright: Yash Gupta
 * License: MIT Public License
 */
#include "bit_vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

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
bit_vector_create(bit_vector_type type, uint64_t length)
{
    bit_vector_t *vector;
    uint64_t temp_length;

    /*
     * Rounding up to power of 2 for length as caller may be giving
     * hint at required size for streams.
     */
    if (type == BIT_VECTOR_TYPE_STREAM) {
        temp_length = 1 << ((int)log2(length) + 1);
    } else {
        temp_length = length;
    }

    vector = malloc(sizeof *vector);
    if (!vector) {
        return NULL;
    }

    vector->array = calloc(BIT_VECTOR_BITS_TO_BYTES(temp_length), sizeof *(vector->array));
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
        errno = EINVAL;
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
bit_vector_clear(bit_vector_t *vector, uint64_t index)
{
    uint8_t and_bits;

    if (!vector || index >= vector->length) {
        errno = EINVAL;
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
bit_vector_get(bit_vector_t *vector, uint64_t index)
{
    int8_t return_bits;
    int8_t and_bits;
    
    if (!vector || index >= vector->length) {
        errno = EINVAL;
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
    uint8_t *temp_array;

    if (!vector) {
        errno = EINVAL;
        return NULL;
    }

    temp_array = realloc(vector->array, BIT_VECTOR_BITS_TO_BYTES(length));
    if (!temp_array) {
        return NULL;
    } else {
        vector->array = temp_array;
        vector->length = length;
    }

    return vector;
}

/**
 * Detach a bit from a bit vector. Used when working with a
 * vector of type BIT_VECTOR_TYPE_STREAM.
 * 
 * @param   vector      The vector to detach bit from.
 * 
 * @return  0/1
 * @return  -1          Invalid parameter.
 * @return  -1          Nothing to detach (ENOSR).
 */
int8_t 
bit_vector_detach(bit_vector_t *vector)
{
    uint8_t bit;

    if (!vector) {
        errno = EINVAL;
        return -1;
    } else if (vector->vector_type == BIT_VECTOR_TYPE_ARRAY) {
        errno = EINVAL;
        return -1;
    } else if (vector->index == 0) {
        errno = ENOSR;
        return -1;
    }

    vector->index--;
    bit = bit_vector_get(vector, vector->index);

    return bit;
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
        errno = EINVAL;
        return NULL;
    } else if (bit > BIT_VECTOR_STATE_SET) {
        errno = EINVAL;
        return NULL;
    } else if (vector->vector_type == BIT_VECTOR_TYPE_ARRAY) {
        errno = EINVAL;
        return NULL;
    }

    if (vector->index == vector->length) {
        if (bit_vector_resize(vector, vector->length*2) == NULL) {
            return NULL;
        }
    }

    if (bit == BIT_VECTOR_STATE_CLEAR) {
        bit_vector_clear(vector, vector->index);
    } else {
        bit_vector_set(vector, vector->index);
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
        errno = EINVAL;
        return NULL;
    } else if (vector->vector_type == BIT_VECTOR_TYPE_ARRAY) {
        errno = EINVAL;
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
bit_vector_append_vector(bit_vector_t *dest, bit_vector_t *src, uint64_t size)
{
    uint64_t i, append_length;
    uint8_t bit;

    if (!dest || !src) {
        errno = EINVAL;
        return NULL;
    } else if (dest->vector_type == BIT_VECTOR_TYPE_ARRAY) {
        errno = EINVAL;
        return NULL;
    }

    if (size == 0) {    
        append_length = (src->vector_type == BIT_VECTOR_TYPE_ARRAY)? src->length: src->index;
    } else {
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
 * Convert a bit vector into a C-style string. It is the callers
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
        errno = EINVAL;
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
    uint64_t i, print_length;

    /*
     * Not performing error checking here as I have no 
     * way to report back an error. Better that a segmentation 
     * fault occurs rather than a silent return.
     */
    
    print_length = (vector->vector_type == BIT_VECTOR_TYPE_ARRAY)? vector->length: vector->index;
    for (i = 0; i < print_length; i++) {
        printf("%u", bit_vector_get(vector, i));
    }
    
    if (i > 0) {
        printf("\n");
    }
}

/**
 * Output a bit vector onto a file descriptor from 
 * an offset.
 * 
 * Format:
 * -> 1 byte for the bit vector type.
 * -> 8 bytes for the length.
 * -> 8 bytes for the index.
 * -> Length/Index bytes for the array.
 *
 * @param   vector  The bit vector you want to output.
 * @param   fd      The file you want to output to.
 * @param   offset  The offset in file to output to.
 *
 * @return  The next writing offset
 * @return  -1      Error with errno set.
 */
int64_t
bit_vector_file_output(bit_vector_t *vector, int fd, uint64_t offset)
{
    // TODO: Move to macros.
    const int type_size = 1;
    const int length_size = 8;
    const int index_size = 8;
    const int metadata_size = type_size + length_size + index_size;

    uint64_t *length, *index, length_of_array;
    uint8_t *type;
    uint8_t metadata[metadata_size];

    if (!vector) {
        errno = EINVAL;
        return -1;
    }

    // Micro optimization to save number of writes.
    type = metadata;
    length = (uint64_t*)(metadata + type_size);
    index = (uint64_t*)(metadata + type_size + length_size);

    *type = vector->vector_type;
    *length = vector->length;
    *index = vector->index;
    
    if (pwrite(fd, metadata, metadata_size, offset) < metadata_size) {
        return -1;
    } else {
        offset += metadata_size;
    }

    length_of_array = (vector->vector_type == BIT_VECTOR_TYPE_ARRAY)? vector->length: vector->index;
    length_of_array = BIT_VECTOR_BITS_TO_BYTES(length_of_array);
    if (pwrite(fd, vector->array, length_of_array, offset) < (int64_t)length_of_array) {
        return -1;
    } else {
        offset += length_of_array;
    }

    return offset;
}

/**
 * Input a bit vector from a file descriptor and 
 * an offset.
 *  
 * Format:
 * -> 1 byte for the bit vector type.
 * -> 8 bytes for the length.
 * -> 8 bytes for the index.
 * -> Length/Index bytes for the array.
 *
 * @param   fd      The file you want to input from.
 * @param   offset  The offset in file to input from. The
 *                  next offset to read from is returned
 *                  in this.
 *
 * @return  A bit vector.
 * @return  NULL    Error with errno set.
 */
bit_vector_t*
bit_vector_file_input(int fd, uint64_t *offset)
{
    // TODO: Move to macros.
    const int type_size = 1;
    const int length_size = 8;
    const int index_size = 8;
    const int metadata_size = type_size + length_size + index_size;

    bit_vector_t *vector;
    uint64_t *length, *index, length_of_array;
    uint8_t *type;
    uint8_t metadata[metadata_size];

    // Micro optimization to save number of reads.
    if (pread(fd, metadata, metadata_size, *offset) < metadata_size) {
        return NULL;
    } else {
        *offset += metadata_size;
    }
    type = metadata;
    length = (uint64_t*)(metadata + type_size);
    index = (uint64_t*)(metadata + type_size + length_size);

    vector = bit_vector_create(*type, *length);
    if (!vector) {
        return NULL;
    } else {
        vector->index = *index;
    }

    length_of_array = (vector->vector_type == BIT_VECTOR_TYPE_ARRAY)? vector->length: vector->index;
    length_of_array = BIT_VECTOR_BITS_TO_BYTES(length_of_array);
    if (pread(fd, vector->array, length_of_array, *offset) < (int64_t)length_of_array) {
        bit_vector_free(vector);
        return NULL;
    } else {
        *offset += length_of_array;
    }

    return vector;
}