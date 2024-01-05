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

TEST_DEPS := testList.c
TEST_DEPS += arrayList.h arrayList.c

test_list: $(TEST_DEPS) 
	gcc -g -o test_list $(TEST_DEPS) 
	./test_list

clean_list: test_list
	rm test_list

