#ifndef TUCKY_UTILS_H
#define TUCKY_UTILS_H

#include <stdbool.h>

static inline bool isSpace(const char c) {
    return (
        c==' '  ||
        c=='\t' ||
        c=='\n' ||
        c=='\r'
    );
}

static inline bool isAlpha(const char c) {
    return (
        (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z')
    );
}

static inline bool isDecDigit(const char c) {
    return (
        (c>='0' && c<='9')
    );
}
static inline bool isHexDigit(const char c) {
    return (
        (c>='0' && c<='9') ||
        (c>='a' && c<='f') ||
        (c>='A' && c<='F')
    );
}

static inline bool isAlphaNum(const char c) {
    return (
        (c >= 'a' && c <= 'z') ||
        (c >= 'A' && c <= 'Z') ||
        isDecDigit(c)
    );
}

#endif /* TUCKY_UTILS_H */