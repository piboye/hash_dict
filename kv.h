/*
 * =====================================================================================
 *
 *       Filename:  kv.h
 *
 *    Description:  
 *        Version:  1.0
 *        Created:  2012/03/13 23时46分42秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef KV_H_INC
#define KV_H_INC
#include <string>
#include <vector>
#include <assert.h>
#include "varlen.h"

#define KV_INSERT_TO_VECTOR(v, value)  \
        v.insert(v.end(), (char *)&value, ( char *) &value + sizeof(value))

template <typename T>
inline
void push_back_vc(std::vector<char> & vc, T value) {
        vc.insert(vc.end(), (char *)&value, ( char *) &value + sizeof(value));
}

inline
void push_back_vc(std::vector<char> & vc, std::string const & value) {
        vc.insert(vc.end(), value.c_str(), value.c_str() + value.size());
}


struct KV
{
    std::string key;
    std::string value;

    void write_raw(std::vector<char> & raw) const {
        size_t key_len = key.size();
        push_back_vc(raw, ENCODE_LEN(key_len));
        size_t value_len = value.size();
        push_back_vc(raw, ENCODE_LEN(value_len));
        push_back_vc(raw, key);
        push_back_vc(raw, value);
        return ;
    }

    size_t raw_size() const {
        return raw_size(key.size(), value.size());  
    }

    static 
    size_t raw_size(size_t k, size_t v) {
        return  encode_len_size(k) + encode_len_size(v)\
                + k +v;
    }

    int read_raw(char const *src, size_t len) {
        assert(src);
        assert(len>0);
        char const *p = src;

        // key_len
        size_t key_len = 0;
        p+=decode_len(p, key_len); 

        //value_len
        size_t value_len = 0;
        p+=decode_len(p, value_len); 
        
        if(len < raw_size(key_len, value_len)) {
                return -__LINE__;
        }
        
        key.assign(p, key_len);
        p+=key_len;
        value.assign(p, value_len);

        return raw_size();
    }

    // return: >0 no found; =0 found; <=0 error;
    static int lookup_in_raw(char const *key, size_t klen, char const *raw, size_t len, char const *&value, size_t & vlen) {
        char const *p = raw;

        size_t key_len = 0;
        p+=decode_len(p, key_len); 
        
        //value_len
        size_t value_len = 0;
        p+=decode_len(p, value_len); 

        if (key_len == klen && 0==memcmp(p, key, klen)) {
            p+=key_len;
            value = p;
            vlen = value_len; 
            return 0;
        }
        return  raw_size(key_len,  value_len);
    }
};

struct KVGroup
{
    std::vector<KV> group;
    
    size_t content_size() const  {
        size_t len = 0;
        for(typeof(group.begin()) it = group.begin(), iend=group.end(); it != iend; ++it) {
            len +=it->raw_size(); 
        }
        return len;
    }
    size_t raw_size() const {
        size_t len = content_size();
        len+=encode_len_size(group.size());
        len+=encode_len_size(len);
        return len;
    }

    void write_raw(std::vector<char> & raw) const {
        size_t len = content_size(); 
        size_t num = group.size();
         
        //raw.reserve(len);
        
        push_back_vc(raw, ENCODE_LEN(len));
        push_back_vc(raw, ENCODE_LEN(num));
        for(typeof(group.begin()) it = group.begin(), iend=group.end(); it != iend; ++it) {
            it->write_raw(raw);
        }
        return ;
    }

    size_t read_raw(char const *src) {
        assert(src);
        char const *p = src;

        size_t ret = 0;
        size_t len = 0;
        p+=decode_len(p, len);


        size_t num = 0;
        p+=decode_len(p, num);

        ret = encode_len_size(len) + encode_len_size(num) + len;

        size_t rlen = len;
        for(size_t i = 0; i<num; ++i) {
            KV kv;
            size_t has_read = kv.read_raw(p, rlen);
            if (has_read < 0) {
                return -2;
            }
            p+=has_read;
            rlen-=has_read;
            group.push_back(kv);
            if (rlen <0) break;
        }
       
        if (group.size() != num) return -3;
        return ret;
    }


    // return: 0 found; 1 not found; <0 error;
    static 
    int lookup_in_raw(char const *key, size_t klen, char const * raw, char const *&value, size_t &vlen) {
        assert(key);
        assert(klen>0);
        assert(raw);

        char const *p = raw;

        size_t len = 0;
        p+=decode_len(p, len);

        size_t num = 0;
        p+=decode_len(p, num);

        size_t rest_len= len;
        for(size_t i = 0; i<num; ++i) {
            KV kv;
            size_t has_read = KV::lookup_in_raw(key, klen, p, rest_len, value, vlen);
            if (has_read == 0) {
                return 0;
            }
            p += has_read;
            rest_len -= has_read;
            if (rest_len < 0) break;
        }
        return 1;
}
    
};

#endif 
