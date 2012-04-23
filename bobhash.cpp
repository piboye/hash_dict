#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "bobhash.h"

int main(int argc, char **argv) 
{ 
    char line[1024]; 
    int init =0; 
    uint32_t mod = 0;
    if(argc >= 2) { 
        init = atoi(argv[1]); 
    } 
    if(argc >= 3) { 
        mod = atoi(argv[2]); 
    } 
    while(fgets(line, sizeof(line), stdin)){ 
        int len = 0;
        char *name=line;
        while(*name != '\0' && *name != '\t') { 
            ++name;
            ++len;
        }
        uint32_t hash_id = bob_hash(line, len, 251); 
        if (mod) {
            hash_id %=mod;
        }

        printf("%u\t%s", hash_id, line); 
    } 
    return 0; 
}
