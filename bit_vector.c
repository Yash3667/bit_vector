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
 * @return  A bit vector
 * @return  NULL        Not enough memory.
 */
bit_vector_t* 
bit_vector_create(bit_vector_type type, uint64_t length);
{
    const uint64_t stream_vector_size = 1;
    bit_vector_t *vector;
    uint64_t temp;

    if (length == 0) {
        return NULL;
    }

    vector = malloc(sizeof(bit_vector_t));
    if (!vector) {
        return NULL;
    }

    /*
     * When working with a stream vector, we do not care about
     * the length as we work with a single byte vector and increase
     * the length as required.
     */
    
    if (type == BIT_VECTOR_TYPE_STREAM) {
        temp = stream_vector_size;
    } else {
        temp = length;
    }

    vector->array = calloc(BIT_VECTOR_BITS_TO_BYTES(tmep) + 1, sizeof(uint8_t));
    if (!(vector->array)) {
        free(vector);
        return NULL;
    }

    vector->length = temp;
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
    free(vector->array);
    free(vector);
}

/**
 * Set a specific bit in the bit vector. Like any array, the index
 * begins from 0.
 *
 * @param   vector      The vector to set bit for.
 * @param   index       The index of the bit to set.
 * @return  0           No error.
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
 * @return  0           No error.
 * @return  -1          Error with errno set.
 *  EINVAL: vector is NULL.
 *  EINVAL: index is beyond vector length.
 */
int 
bit_vector_clear(bit_vector_t *vector, uint64_t index);
{
    uint8_t and_bits;

    if (!vector || index >= vector->array) {
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
 * @return  1           Bit is set.
 * @return  0           Bit is clear.
 * @return  -1          Error with errno set.
 *  EINVAL: vector is NULL.
 *  EINVAL: index is beyond vector length.
 */
int 
bit_vector_get(bit_vector_t *vector, uint64_t index);
{
    int8_t return_bits;
    int8_t and_bits;
    
    if (!vector || index >= vector->vector_length) {
        errno = -EINVAL;
        return -1;
    }

    and_bits = 1 << BIT_VECTOR_GET_BIT_INDEX(index);
    return_bits = vector->array[BIT_VECTOR_GET_BYTE_INDEX(index)] & and_bits;
    return !!return_bits;
}


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
 * Get the index of the bit vector. This function is mostly useless
 * for type of BIT_VECTOR_TYPE_ARRAY but is useful for when using
 * as a stream.
 *
 * @param   vector      The vector whose size we want.
 * @param   index       Pointer to index variable.
 * @return  0           Size succesfully returned.
 * @return  -1          Error with errno set.
 *  EINVAL: vector is NULL.
 *  EINVAL: vector is of type BIT_VECTOR_TYPE_ARRAY.
 *  EINVAL: index is NULL
 */
int 
bit_vector_index(bit_vector_t *vector, uint64_t *index);
{
    if (!vector || !index || vector->type == BIT_VECTOR_TYPE_ARRAY) {
        errno = -EINVAL;
        return -1;
    }

    *index = vector->index;
    return 0;
}

/**
 * This function is used to resize the bit vector so that
 * now it can hold the specified amount of bits in it.
 *
 * @param vector The bit vector to resize.
 * @param length The new length of the bit vector.
 */
bit_vector_t*
bit_vector_resize(bit_vector_t *vector, uint64_t length)
{
    uint8_t *temp_vector;

    /* Confirm valid vector */
    if (!vector) {
        return NULL;
    }

    /* Resize the length and call realloc */
    vector->vector_length = length;
    temp_vector = realloc(vector->vector, (length / VECTOR_BYTE_SIZE) + 1);
    if (!temp_vector) {
        bit_vector_free(vector);
        return NULL;
    } else {
        vector->vector = temp_vector;
    }

    return vector;
}

/**
 * This function is used to append a bit to the bit vector.
 * In most case this function is used when working with a stream
 * of bits in which case it cab be useful to have a function
 * which treats this vector as a stream of bits.
 *
 * @param vector The vector to which we want to append a bit.
 * @param bit The bit to append.
 * @return The bit vector or NULL.
 */
bit_vector_t*
bit_vector_append_bit(bit_vector_t* vector, uint8_t bit)
{
    if (!vector) {
        return NULL;
    } else if (bit > 1) {
        return NULL;
    }

    if (vector->working_index == vector->vector_length) {
        bit_vector_resize(vector, vector->vector_length * 2);
    }

    if (bit == 0) {
        bit_vector_clear_bit(vector, vector->working_index);
    } else {
        bit_vector_set_bit(vector, vector->working_index);
    }
    vector->working_index += 1;

    return vector;
}

/**
 * This function is used to append one vector to another vector
 * and is really helpful when working with multiple bit vectors.
 *
 * @param vector The vector to which we append.
 * @param copy_vector The vector which we copy.
 * @param flag The flag of how to copy.
 * @return vector or NULL.
 */
bit_vector_t*
bit_vector_append_vector(bit_vector_t *vector, bit_vector_t *copy_vector, uint8_t flag)
{
    uint8_t bit;
    uint64_t i, append_length;

    if (!vector || !copy_vector) {
        return NULL;
    }

    /* Set length according to flag */
    append_length = bit_vector_get_size(copy_vector, flag);

    /* Append all bits of the copy_vector */
    for (i = 0; i < append_length; i++) {
        bit = bit_vector_check_bit(copy_vector, i);
        bit_vector_append_bit(vector, bit);
    }

    return vector;
}

/**
 * This function is used to print the bit vector depending
 * on the flag which is passed.
 *
 * @param vector The vector to print
 * @param flag Style of printing
 */
void
bit_vector_print(bit_vector_t *vector, uint8_t flag)
{
    uint8_t bit;
    uint64_t i, print_length;

    if (!vector) {
        return;
    }

    /* Print vector either as a stream or as the full vector */
    print_length = bit_vector_get_size(vector, flag);

    for (i = 0; i < print_length; i++) {
        if (i % (VECTOR_BYTE_SIZE / 2) == 0 && i > 0) {
            printf(" ");
        }
        bit = bit_vector_check_bit(vector, i);
        printf("%u", bit);
    }
    printf("\n");
}

/**
 * This function is used to output a bit vector onto a file
 * after a specified offset. This is helpful when you want to
 * save a bit vector in a binary format.
 *
 * @param vector The bit vector you want to output
 * @param fd The file you want to output to
 * @param offset The offset in file to output to
 * @param flags Style of output.
 * @return 0 on success or error code
 */
ssize_t
bit_vector_output(bit_vector_t *vector, int fd, uint64_t offset, uint8_t flag)
{
    uint64_t length;
    ssize_t size_to_write;
    ssize_t bytes_written;

    if (!vector) {
        return -1;
    }

    /* Get which length to write depending on flag */
    length = bit_vector_get_size(vector, flag);

    /* Get length depending on flag and write it out */
    size_to_write = sizeof(uint64_t);
    bytes_written = pwrite(fd, &length, size_to_write, offset);
    if (bytes_written < size_to_write) {
        return -2;
    } else {
        offset += bytes_written;
    }

    /* Write out the entire vector */
    length = (length / VECTOR_BYTE_SIZE) + 1;
    size_to_write = length;
    bytes_written = pwrite(fd, vector->vector, size_to_write, offset);
    if (bytes_written < size_to_write) {
        return -4;
    } else {
        offset += bytes_written;
    }

    return offset;
}

/**
 * This function is used to input a bit vector from a file.
 * This function expects the file to be formatted the way
 * bit_vector_output emits. This is helpful for saving and restoring
 * state of the bit vector.
 *
 * @param fd The file you want to output to.
 * @param offset The offset in file to output to.
 * @return A bit vector or null
 */
bit_vector_t*
bit_vector_input(int fd, uint64_t offset)
{
    bit_vector_t *vector;
    uint64_t length;
    ssize_t size_to_read;
    ssize_t bytes_read;

    /* Read the length */
    size_to_read = sizeof(uint64_t);
    bytes_read = pread(fd, &length, size_to_read, offset);
    if (bytes_read < size_to_read) {
        return NULL;
    } else {
        offset += bytes_read;
    }

    /* Make the vector */
    vector = bit_vector_create(length);
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


/**
 * This function is used to convert a bit string which is
 * represented as a C-style string into a bit vector string.
 *
 * @param bit_string The bit string to convert
 * @return A bit vector or NULL
 */
bit_vector_t*
bit_vector_convert(char *bit_string)
{
    char bit_code;
    uint64_t i, length;
    bit_vector_t *temp;

    length = strlen(bit_string);
    temp = bit_vector_create(length);
    if (!temp) {
        return NULL;
    }

    /*
     * The following loop is powerful in that it can create
     * a bit vector even if the bit_string contains elements
     * which are not 0 or 1. If a string which has extra characters
     * is passed in, then those extra characters are ignored.
     */

    for (i = 0; i < length; i++) {
        bit_code = bit_string[i];
        if (bit_code == '0') {
            temp = bit_vector_append_bit(temp, 0);
        } else if (bit_code == '1') {
            temp = bit_vector_append_bit(temp, 1);
        }

        if (!temp) {
            return NULL;
        }
    }

    /*
     * We resize this bit vector in case extra invalid characters were
     * passed in the string so that our vector does not consume extra
     * memory. We simply return whatever we got, be it NULL or a valid
     * pointer.
     */
    temp = bit_vector_resize(temp, temp->working_index);
    return temp;
}