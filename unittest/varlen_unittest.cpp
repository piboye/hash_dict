/*
 * =====================================================================================
 *
 *       Filename:  kv_uint.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012/03/14 10时55分37秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "varlen.h"

TEST(varlen, encode_len) {
    size_t len = 2;
    char buf[100];
    size_t e_len = encode_len(buf, len);
    ASSERT_EQ(e_len, 1);
    len = 1000;
    e_len = encode_len(buf, len);
    ASSERT_EQ(e_len, 5);
}

TEST(varlen, decode_len) {
    size_t len = 2;
    char buf[100];
    size_t e_len = 0;
    encode_len(buf, len);
    decode_len(buf, e_len);
    ASSERT_EQ(e_len, len);
    len = 1000;
    e_len = encode_len(buf, len);
    decode_len(buf, e_len);
    ASSERT_EQ(e_len, len);
}

