general: read_noncanonical.c write_noncanonical.c
	gcc -Wall read_noncanonical.c -o read
	gcc -Wall write_noncanonical.c -o write

clean:
	rm -f read
	rm -f write