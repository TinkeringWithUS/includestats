build: linecount.c 
	gcc -o linecount linecount.c 

run: linecount.c
	build
	./linecount

clean: linecount
	rm linecount
