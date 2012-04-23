/*
 * =====================================================================================
 *
 *       Filename:  write_key_id.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012/03/15 18时03分44秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *        Company:  
 *
 * =====================================================================================
 */
#include <stdio.h>
#include "bobhash.h"
#include "hash_dict.h"

int split(char const *line, std::vector<std::string> & result)
{
    char const *p = line;
    char const *pre = p;
    int num=0;
        while(*p != '\0') { 
        if (*p == '\t') {
        result.push_back(std::string(pre, p-pre));
        pre = p+1;
        ++num;
        }
        ++p;
        }
    if (pre < p) {
        result.push_back(std::string(pre, p-pre));
        ++num;
    }
    return num;
}

int main(int argc, char **argv) 
{ 
    char line[10240]; 
    if(argc < 3) { 
        fprintf(stderr, "usage:%s file index_num index_expand=0, align=0\n", argv[0]);
	return 1;
    } 
    

    char const * file=argv[1];
    int index_num=atoi(argv[2]);
    int index_expand = 0;
    int align = 0;
    if (argc > 3) {
        index_expand = atoi(argv[3]);
    }

    if (argc > 4) {
        align = atoi(argv[4]);
    }
    
    CreateHashDict<uint64_t> dict;
    dict.create(file, index_num, index_expand, align);

    long long int pre_hash_id=0;
    std::vector<std::string> words;
    KVGroup group;
    while(fgets(line, sizeof(line), stdin)){ 
        words.clear();
        split(line, words);
        if (words.size() < 3) {
            fprintf(stderr, "error too less [%ld]fields:%s\n", words.size(), line);
            continue;
        }	
        long long int hash_id = atoll(words[0].c_str());
        std::string value;
        std::vector<int32_t> ids;
        for(size_t i=2; i<words.size(); ++i) {
            int32_t id = atoi(words[i].c_str());
            //fprintf(stderr, "ids:%s, [%d]", words[i].c_str(), id);
            ids.push_back(id);
        }

        KV kv;
        kv.key = words[1];
        //kv.value.resize(ids.size()*sizeof(int32_t));
        kv.value= std::string((char const *) (&ids[0]), (char const *)(&ids[0])+ids.size()*sizeof(int32_t));
        if (pre_hash_id == hash_id) {
            group.group.push_back(kv); 
            continue;
        }
        if (!group.group.empty()) {
            dict.write_data(pre_hash_id, group);
            group.group.clear();
            //fprintf(stderr, "process hash_id:%lld\n", pre_hash_id);
        }
        pre_hash_id = hash_id;
        group.group.push_back(kv); 
    }

    if (!group.group.empty()) {
        dict.write_data(pre_hash_id, group);
        group.group.clear();
    }

    return 0; 
}
