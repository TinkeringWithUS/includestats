build: linecount.c 
	gcc -o linecount -g linecount.c 

run: linecount.c
	build
	./linecount

clean: linecount
	rm linecount
