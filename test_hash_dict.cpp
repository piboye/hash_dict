/*
 * =====================================================================================
 *
 *       Filename:  test_hash_dict.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012/03/14 16时46分19秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include "hash_dict.h"
#include "kv.h"
#include <stdio.h>
#include <string>

int main() {
    CreateHashDict<int32_t> hash_dict;
    int ret = 0;
    ret = hash_dict.create("test", 100);
    if (ret) {
        fprintf(stderr, " create dict failed, ret:%d\n", ret);
        return 1;
    }
    
    KVGroup group;
    KV kv1, kv2;
    kv1.key="a";
    kv1.value="1234";
    kv2.key="b";
    kv2.value="2";
    group.group.push_back(kv1);
    group.group.push_back(kv2);
    ret = hash_dict.write_data(1, group);
    if (ret) {
        fprintf(stderr, "write dict failed, ret:%d\n", ret);
        return 1;
    }

    ret = hash_dict.write_data(2, group);
    if (ret) {
        fprintf(stderr, "write dict failed, ret:%d\n", ret);
        return 1;
    }

    HashDict<int32_t> dict;
    ret = dict.open("test");
    if (ret) {
        fprintf(stderr, "open dict failed, ret:%d\n", ret);
        return 1;
    }

    char const *v = NULL;
    size_t vlen;
    ret = dict.lookup(2, "a", 1, v, vlen);
    if (ret<0) {
        fprintf(stderr, "lookup dict failed, ret:%d\n", ret);
        return 1;
    }
    else if (ret > 0) {
        printf("lookup dict no found, ret:%d\n", ret);
    } else if (ret == 0){
        std::string value(v, v+vlen);
        printf("lookup dict key:%s, value:%s, vlen:%ld\n", "a", value.c_str(), vlen);
    }
    return 0;
}
