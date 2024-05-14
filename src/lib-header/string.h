#ifndef _STRING_H
#define _STRING_H

#include <stdint.h>
#include <stddef.h>

/**
 * C standard memset, check man memset or
 * https://man7.org/linux/man-pages/man3/memset.3.html for more details
 * 
 * @param s Pointer to memory area to set
 * @param c Constant byte value for filling memory area
 * @param n Memory area size in byte 
 * 
 * @return Pointer s
*/
void* memset(void *s, int c, size_t n);

/**
 * C standard memcpy, check man memcpy or
 * https://man7.org/linux/man-pages/man3/memcpy.3.html for more details
 * 
 * @param dest Starting location for memory area to set
 * @param src Pointer to source memory
 * @param n Memory area size in byte 
 * 
 * @return Pointer dest
*/
void* memcpy(void* restrict dest, const void* restrict src, size_t n);

/**
 * C standard memcmp, check man memcmp or
 * https://man7.org/linux/man-pages/man3/memcmp.3.html for more details
 * 
 * @param s1 Pointer to first memory area
 * @param s2 Pointer to second memory area
 * @param n Memory area size in byte 
 * 
 * @return Integer as error code, zero for equality, non-zero for inequality
*/
int memcmp(const void *s1, const void *s2, size_t n);

/**
 * C standard memmove, check man memmove or
 * https://man7.org/linux/man-pages/man3/memmove.3.html for more details
 * 
 * @param dest Pointer to destination memory
 * @param src Pointer to source memory
 * @param n Memory area size in byte 
 * 
 * @return Pointer dest
*/
void *memmove(void *dest, const void *src, size_t n);

/**
 * @param char *stringA kata pertama
 * @param char *stringB kata kedua
 * @param int size ukuran yang akan dicek
 * 
 * @return 0 jika sama, else beda
*/
int strcmp(const char *stringA, char *stringB, int size);

/**
 * @param int a
 * @param int b
 * @return maximum antara a b
*/
int max(uint16_t a, uint16_t b);

/**
 * @param char* string
 * @return length of string
*/
size_t strlen(char* string);

/**
 * Like strcmp
*/
int isEqual(const char *stringA, char *stringB, int size);

/**
 * @param char* string
 * @param int length of string to clear 
*/
void clear(char* string, int size);

/**
 * @brief string concat with "/" between the string
 * 
 */
void str_path_concat(char* dest, const char* src1, const char* src2);

#endif