security.a: security.o
	ar cr security.a security.o
security.o: security.c
	gcc -c security.c -o security.o -O3
.PHONY: mktest clean rmprv install
mktest:
	gcc security.c -o testserver -O3 -DMIAM -Dawdebg -DUDBG -lssl
	gcc testclient.c -o testclient -O3 -Dawdebg -DUDBG -lssl
rmprv:
	-rm /usr/lib/sptcpsv/security.a /usr/lib/sptcpsv/security.h
clean:
	-rm security.a security.o testserver testclient
install:
	cp security.h /usr/lib/sptcpsv/
	cp security.a /usr/lib/sptcpsv/
