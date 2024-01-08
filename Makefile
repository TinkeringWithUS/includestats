run: linecount.c
	make build
	./linecount

BUILD_DEPS := linecount.c 
BUILD_DEPS += arrayList.h arrayList.c
BUILD_DEPS += parseFile.h parseFile.c
BUILD_DEPS += colors.h 
BUILD_DEPS += stringHelp.h stringHelp.c

build: $(BUILD_DEPS)
	gcc -g -o linecount $(BUILD_DEPS)

clean: linecount
	rm linecount

TEST_LIST_DEPS := arrayList.h arrayList.c

test_list: $(TEST_LIST_DEPS) 
	gcc -g -o test_list testList.c $(TEST_LIST_DEPS)
	./test_list

clean_list: $(TEST_LIST_DEPS)
	rm test_list

TEST_SET_DEPS := arraySet.c arraySet.h $(TEST_LIST_DEPS)

test_set: $(TEST_SET_DEPS)
	gcc -g -o test_set testSet.c $(TEST_SET_DEPS)
	./test_set

clean_set: $(TEST_SET_DEPS) 
	rm test_set
