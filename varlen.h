/*
 * =====================================================================================
 *
 *       Filename:  varint.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012年03月31日 15时22分26秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef VARLEN_H_INC_
#define VARLEN_H_INC_

#include <stdlib.h>
#include <string>

static size_t decode_len(char const *a_p, size_t &a_len) {
    unsigned char const *p = (unsigned char const *)(a_p);
    unsigned int len = (unsigned char)(*p);
    if (len < 254) {
        a_len = len;
        return 1;
    }
    memcpy(&len, p+1,sizeof(uint32_t));
    a_len = len;
    return sizeof(uint32_t)+1;
}

static size_t encode_len(char *a_p, size_t a_len) {
    assert(a_len < 1UL<<(sizeof(uint32_t)*8));
    unsigned char *p = (unsigned char *)(a_p);
    uint32_t len = (uint32_t)(a_len);
    if (p == NULL) {
        return 0;
    } else {
        if (len < 254) {
            p[0] = len;
            return 1;
        } else {
            p[0] = 254;
            memcpy(p+1,&len,sizeof(len));
            return 1+sizeof(len);
        }
    }
    return 0;
}

inline
size_t encode_len_size(size_t a_len) {
    if (a_len < 254) return 1; 
    return sizeof(uint32_t)+1;
}

inline 
std::string ENCODE_LEN(size_t a_len) { char buffer[10]; size_t l = encode_len(buffer, a_len); return std::string(buffer, l);}

#endif //VARLEN_H_INC_
