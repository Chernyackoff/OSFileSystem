all:
	gcc main.c -o fs

clean:
	rm -f fs

re: clean all

.PHONY: all clean re