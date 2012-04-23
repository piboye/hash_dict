ALL= test_hash_dict write_key_id bobhash query_key_id

all: $(ALL)

	
test_hash_dict: test_hash_dict.cpp
	g++ -ggdb -Wall $< -o $@

write_key_id: write_key_id.cpp
	g++ -ggdb -Wall $< -o $@

bobhash: bobhash.cpp
	g++ -O2 -ggdb -Wall $< -o $@
	
query_key_id : query_key_id.cpp
	g++ -O2 -ggdb -Wall $< -o $@

clean: 
	rm -f *.o $(ALL)
