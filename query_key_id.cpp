/*
 * =====================================================================================
 *
 *       Filename:  query_key_id.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2012/03/15 18时49分14秒
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


void strip(char *line) {
    while(*line!='\0') {
        if (*line == '\r') { *line= '\0';}
        if (*line == '\n') { *line='\0';}
        ++line;
    }
}

std::string to_lower(std::string const &str) {
    std::string ret;
    for(size_t i=0, len = str.size(); i<len; ++i) {
        ret.push_back(tolower(str[i]));
    }
    return ret;
}

int main(int argc, char **argv) 
{ 
    char line[10240]; 
    if(argc < 2) { 
        fprintf(stderr, "usage:%s file\n", argv[0]);
	return 1;
    } 
    

    char const * file=argv[1];
    
    HashDict<uint64_t> dict;
    int ret = 0;
    ret = dict.open(file);
    if (ret) {
        fprintf(stderr, "open file failed! err:%d\n", ret);
	return 1;
    }

    std::vector<std::string> words;
    KVGroup group;
    while(fgets(line, sizeof(line), stdin)){ 
        words.clear();
        strip(line);
        split(line, words);
        if (words.empty()) continue;
        std::string key = words[0];
        
        char const *v =NULL;
        size_t vlen = 0;
        uint32_t hash_id = bob_hash(key.c_str(), key.size(), 251);
        ret = dict.lookup(hash_id, key.c_str(), key.size(), v, vlen);
        if (ret == 1) {
            printf("no found, hash_id:%u, key:%s\n", hash_id, key.c_str());
        } 
        else if (ret == 0) {
            printf("found, key:%s, vlen:%ld, value:[", key.c_str(), vlen);
            std::vector<int32_t> ids;
            ids.resize(vlen/sizeof(int32_t),0);
            memcpy(&ids[0], v, vlen);
            for(size_t i=0; i< ids.size(); ++i) {
                if (i>0) printf(",");
                printf("%d", ids[i]);
            }
            printf("]\n");
        }
    }
    return 0; 
}

