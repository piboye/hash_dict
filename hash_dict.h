/*
 * =====================================================================================
 *
 *       Filename:  fix_hdb.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012/03/13 23时42分23秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#ifndef HASH_DICT_H_INC
#define HASH_DICT_H_INC

#include "file_map.h"
#include "kv.h"
#include <vector>

struct HashIndexHeader
{
    uint64_t magic;
    uint32_t version;
    uint32_t head_len; 
    uint32_t index_num;
    uint32_t index_start;
    uint32_t index_size;
    uint32_t index_expand;
};


template<typename index_t>
struct HashIndex
{
typedef index_t index_type;
index_type *index;
size_t num;
};


template<typename index_t>
struct HashDictIndex {
    typedef index_t index_type;
    HashIndexHeader header;
    HashIndex<index_type> indexes;
    
    FileMap file_map;
        
    ~HashDictIndex() {
        close();
    }

    int close() {
        file_map.close();
        return 0;
    }

    int open(char const * file) {
        int ret = 0;
        size_t len = 0;
        ret = file_map.init(file, len); 
        if (ret !=1) return ret;

        memcpy(&header, file_map.addr(), sizeof(header));

        indexes.index=(index_t *)((char *)file_map.addr()+header.index_start);
        indexes.num = header.index_num;
        return 1;
    }

    int set(size_t idx, size_t value) {
        idx %= indexes.num;
        indexes.index[idx] = value>>header.index_expand;
        return 0; 
    }

    int get(size_t idx, size_t &value) const { 
        idx %= indexes.num;
        value =  indexes.index[idx];
        value <<=header.index_expand;
        
        return 0;
    }

    int init(char const * file, size_t index_num, int index_expand=0) {
        
        memcpy((char *)&header.magic, "hdi.pb", sizeof("hdi.pb"));
        header.version = 1<<16 & 0;
        header.head_len =  sizeof(header);
        header.index_num = index_num;
        header.index_size = sizeof(index_type);
        header.index_start = (sizeof(header) +15)>>4<<4;
        header.index_expand = index_expand;

        size_t len = index_num*sizeof(index_type)+ header.index_start;

        int ret = 0;
        ret = file_map.init(file, len); 
        if (ret !=0) return ret;
        
        memcpy(file_map.addr(), &header, sizeof(header));

        indexes.index=(index_t *)((char *)file_map.addr()+header.index_start);
        indexes.num = header.index_num;
        memset(indexes.index, 0,  sizeof(index_type)*indexes.num);
        return 0;
    }
};


struct HashDataHeader
{
    uint64_t magic;
    uint32_t version;
    uint32_t head_len; 
    uint32_t data_align;
    uint32_t data_start;
};

struct HashDictData
{
    HashDataHeader header;
    FileMap file_map;
    char * data;
    size_t data_len;
    int data_align;

    int open(char const * file) {
        int ret = 0;
        size_t len = 0;
        ret = file_map.init(file, len); 
        if (ret !=1) return ret;

        memcpy(&header, file_map.addr(), sizeof(header));

        data = (char *)file_map.addr() + header.data_start;
        data_len = file_map.size() - header.data_start;
        data_align = header.data_align;
        return 1;
    }

    char const * get(size_t index)  const {
        return data+index;
    }

    static int create(char const *file_name, int align) {
            int fd = -1; 
            fd = ::open( file_name , O_WRONLY | O_CREAT, 0666);
            if (fd < 0) {
                    return -1;
            }
            HashDataHeader header;

            memcpy((char *)&header.magic, "hdd.pb", sizeof("hdd.pb"));
            header.version = 1<<16 & 0;
            header.head_len =  sizeof(header);
            header.data_align = (uint32_t) align;
            header.data_start = ((sizeof(header) +15)>>4<<4+ 1<<align)>>align<<align;
            write(fd, (char *) &header, sizeof(header)); 
            lseek(fd, header.data_start, SEEK_SET);
            return fd;
    }
};

template <typename index_t>
struct CreateHashDict
{
    int data_fd;
    size_t cur_pos;
    int index_expand;
    HashDictIndex<index_t> dict_idx;
    int data_align;

    int create(char const * file_name_pre, size_t index_num, int a_index_expand=0, int align=0) {
        std::string idx_file_name = file_name_pre;
        idx_file_name+=".idx";
        int ret = 0;
        ret = dict_idx.init(idx_file_name.c_str(), index_num, a_index_expand);
        if (ret != 0) return -1;

        std::string data_file_name = file_name_pre;
        data_file_name+=".data";
        data_fd = HashDictData::create(data_file_name.c_str(), align);
        if (data_fd<0) return -2;
        cur_pos = 0;
        index_expand = a_index_expand;
        data_align = align;
        return 0;
    }

    int write_data(size_t idx, KVGroup & datas) {
        std::vector<char> raw;
        datas.write_raw(raw);
        if (raw.empty()) return -1;
        write(data_fd, &raw[0], raw.size());
        dict_idx.set(idx,  cur_pos);
        cur_pos = cur_pos + raw.size();
        //fprintf(stderr, "cur_pos:%ld, idx:%ld\n", cur_pos, idx);
        if (data_align >0) {
            size_t new_pos = (cur_pos+ 1<<data_align-1)>>data_align<<data_align;
            if (new_pos - cur_pos > 0) {
                lseek(data_fd, new_pos-cur_pos, SEEK_CUR);
                cur_pos = new_pos;
            }
        }
        return 0;
    }
    int close() {
        dict_idx.close();
        ::close(data_fd);
        return 0;
    }
    ~CreateHashDict() {
        this->close();
    }
};

template <typename index_t>
struct HashDict
{
    HashDictIndex<index_t> dict_idx;
    HashDictData dict_data;

    int open(char const * file_name_pre) {
        std::string idx_file_name = file_name_pre;
        idx_file_name+=".idx";
        int ret = 0;
        ret = dict_idx.open(idx_file_name.c_str());
        if (ret != 1) return -1;

        std::string data_file_name = file_name_pre;
        data_file_name+=".data";
        ret = dict_data.open(data_file_name.c_str());
        if (ret != 1) return -2; 
        return 0;
    }

    int lookup(size_t idx, char const *key, size_t klen, char const *&v, size_t &vlen) {
        size_t index = 0;
        int ret = 0;
        ret = dict_idx.get(idx, index);
        if (ret <0) return -1;
        char const * data = dict_data.get(index);
        return KVGroup::lookup_in_raw(key, klen, data, v, vlen);
    }
};

#endif 
