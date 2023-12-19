build: linecount.c arrayList.h arrayList.c  
	gcc -g -o linecount linecount.c arrayList.c arrayList.h 

run: linecount.c
	build
	./linecount

clean: linecount
	rm linecount

test_list: arrayList.h arrayList.c testList.c
	gcc arrayList.c testList.c -o test_list 
	./test_list

clean_list: test_list
	rm test_list

