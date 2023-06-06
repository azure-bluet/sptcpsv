security.a: security.o
	ar cr security.a security.o
security.o: security.c
	gcc -c security.c -o security.o -O3
.PHONY: mktest
mktest:
	gcc security.c -o testserver -O3 -DMIAM -Dawdebg -DUDBG -lssl
	gcc testclient.c -o testclient -O3 -Dawdebg -DUDBG -lssl
