#include <stdint.h>
#include <stddef.h>
#include "lib-header/string.h"

void* memset(void *s, int c, size_t n) {
    uint8_t *buf = (uint8_t*) s;
    for (size_t i = 0; i < n; i++)
        buf[i] = (uint8_t) c;
    return s;
}

void* memcpy(void* restrict dest, const void* restrict src, size_t n) {
    uint8_t *dstbuf       = (uint8_t*) dest;
    const uint8_t *srcbuf = (const uint8_t*) src;
    for (size_t i = 0; i < n; i++)
        dstbuf[i] = srcbuf[i];
    return dstbuf;
}

int memcmp(const void *s1, const void *s2, size_t n) {
    const uint8_t *buf1 = (const uint8_t*) s1;
    const uint8_t *buf2 = (const uint8_t*) s2;
    for (size_t i = 0; i < n; i++) {
        if (buf1[i] < buf2[i])
            return -1;
        else if (buf1[i] > buf2[i])
            return 1;
    }

    return 0;
}

void *memmove(void *dest, const void *src, size_t n) {
    uint8_t *dstbuf       = (uint8_t*) dest;
    const uint8_t *srcbuf = (const uint8_t*) src;
    if (dstbuf < srcbuf) {
        for (size_t i = 0; i < n; i++)
            dstbuf[i]   = srcbuf[i];
    } else {
        for (size_t i = n; i != 0; i--)
            dstbuf[i-1] = srcbuf[i-1];
    }

    return dest;
}

int strcmp(const char *stringA, char *stringB, int size) {
    int i;
    for (i = 0; i < size && stringA[i] == stringB[i]; i++) {
        if (stringA[i] == '\0') {
            return 0;
        }
    }
    if (i == size) {
        return 0;
    }
    return stringA[i] - stringB[i];
}

// int isEqual(char *stringA, char *stringB, int size) {
//     int i;
//     if (strlen(stringA) != strlen(stringB)) {
//         return 1;
//     } else {

//     }
//     for (i = 0; i < size; i++) {
//         if (stringA[i] != stringB[i]) {
//             return 0;
//         }
//     }
//     return 1;
// }

int max(uint16_t a, uint16_t b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

size_t strlen(char* string) {
    size_t len = 0;
    while (string[len] != '\0') {
        len++;
    }
    return len;
}

void clear(char* string, int size) {
    for (int i = 0; i < size; i++) {
        string[i] = 0x00;
    }
}

void str_path_concat(char* dest, const char* src1, const char* src2) {
    // Copy src1 to dest
    while (*src1) {
        *dest++ = *src1++;
    }
    // Add a '/' separator if not already present
    if (*(dest - 1) != '/') {
        *dest++ = '/';
    }
    // Copy src2 to dest
    while (*src2) {
        *dest++ = *src2++;
    }
    // Null terminate the destination string
    *dest = '\0';
}