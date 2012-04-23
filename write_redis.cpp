#include <stdio.h>
#include <vector>
#include <string>
#include <assert.h>
#include "bobhash.h"
#include "hiredis.h"

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
        fprintf(stderr, "usage:%s  ip port pre_key\n", argv[0]);
	return 1;
    } 

    char * ip = argv[1];
    int port = atoi(argv[2]);
    std::string key_pre;
    if (argc >= 4) {
	key_pre = argv[3];
    }

    redisContext * redis_ctx = redisConnect(ip, port);

    if (redis_ctx->err) {
        fprintf(stderr, "Connection error: %s\n", redis_ctx->errstr);
	return 2;
    }

    
    std::vector<std::string> words;
    while(fgets(line, sizeof(line), stdin)){ 
	words.clear();
	split(line, words);
	if (words.size() < 2) {
		//fprintf(stderr, "words[0]:%s\n", words[0].c_str());
		fprintf(stderr, "error too less [%d]fields:%s\n", words.size(), line);
		continue;
	}	
	std::string key = key_pre+words[0];
	uint32_t hasid = bob_hash(key.c_str(), key.size(), 251);
	std::string value;
	for(size_t i=1; i<words.size(); ++i) {
		int32_t id = atoi(words[i].c_str());
		value.append((char *)&id, sizeof(id));
	}
    	void * reply = redisCommand(redis_ctx, "HSET song%u %s %b", 1, key.c_str(), value.c_str(), value.size());
    	assert(reply != NULL);
    	freeReplyObject(reply);
    } 
    return 0; 
}
