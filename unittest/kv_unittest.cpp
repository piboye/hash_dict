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
#include "kv.h"

TEST(KV, write_raw) {
    KV kv;
    std::vector<char> raw;
    kv.write_raw(raw);
    ASSERT_EQ(raw.size(), kv.raw_size());
    ASSERT_EQ(raw.size(), 2);

    raw.clear();
    kv.key="abc";
    kv.value="1";
    kv.write_raw(raw);
    ASSERT_EQ(raw.size(), kv.raw_size());
}

TEST(KV, read_raw) {
    KV kv, kv2;
    std::vector<char> raw;

    kv.key="abc";
    kv.value="1";
    kv.write_raw(raw);
   
    int len = kv2.read_raw(&raw[0], raw.size());
    ASSERT_GT(len, 0);
    ASSERT_EQ(kv.key, kv2.key);
    ASSERT_EQ(kv.value, kv2.value);
}

TEST(KV, find_in_raw_success) {
    KV kv, kv2;
    std::vector<char> raw;

    kv.key="abc";
    kv.value="1";
    kv.write_raw(raw);
    char const *v=NULL;
    size_t vlen=0;
    int r = kv.lookup_in_raw("abc", 3,  &raw[0], raw.size(), v, vlen);
    ASSERT_EQ(r, 0);
    ASSERT_STREQ(v, "1");
    ASSERT_EQ(vlen, 1);
}

TEST(KV, find_in_raw_failed) {
    KV kv, kv2;
    std::vector<char> raw;

    kv.key="abc";
    kv.value="1";
    kv.write_raw(raw);
    char const *v=NULL;
    size_t vlen=0;
    int r = kv.lookup_in_raw("a", 3,  &raw[0], raw.size(), v, vlen);
    ASSERT_NE(r, 0);
    ASSERT_STRNE(v, "1");
        ASSERT_NE(vlen, 1);
}

TEST(KVGroup, write_raw) {
    std::vector<char> raw;
    KVGroup group;
    KV kv;
    group.group.push_back(kv);
    group.write_raw(raw);    
    ASSERT_EQ(raw.size(), group.raw_size());
    ASSERT_EQ(raw.size(), 4);
}

TEST(KVGroup, read_raw) {
    std::vector<char> raw;
    KVGroup group, group2;
    KV kv;
    kv.key="abc";
    kv.value="123";
    group.group.push_back(kv);
    group.write_raw(raw);    

    int ret = 0;
    ret = group2.read_raw(&raw[0]);

    ASSERT_GT(ret, 0);

    ASSERT_EQ(group.group.size(), group2.group.size());
    ASSERT_EQ(group.group[0].key, group2.group[0].key);
    ASSERT_EQ(group.group[0].value, group2.group[0].value);
}

TEST(KVGroup, lookup_in_raw) {
    std::vector<char> raw;
    KVGroup group;
    KV kv, kv2;
    kv.key="abc";
    kv.value="1";

    kv2.key="abd";
    kv2.value="2";
    group.group.push_back(kv);
    group.group.push_back(kv2);
    group.write_raw(raw);

    char const *v=NULL;
    size_t vlen=0;
    int r = KVGroup::lookup_in_raw((char *)"a", 1,  &raw[0], v, vlen);
    ASSERT_NE(r, 0);
    ASSERT_STRNE(v, "1");
    ASSERT_NE(vlen, 1);

    r = KVGroup::lookup_in_raw("abd", 3,  &raw[0], v, vlen);
    ASSERT_EQ(r, 0);
    ASSERT_STREQ(v, "2");
    ASSERT_EQ(vlen, 1);
}  
