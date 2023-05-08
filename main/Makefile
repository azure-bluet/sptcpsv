sptcpsv: sptcpsv.o
	gcc sptcpsv.o /usr/lib/sptcpsv/utils.so -o sptcpsv
sptcpsv.o: sptcpsv.c sptcpsv.h
	gcc -c sptcpsv.c -o sptcpsv.o -O3 -I/usr/lib/sptcpsv -Wpointer-to-int-cast \
		-Wint-to-pointer-cast
.PHONY: clean install rmprv
install:
	cp sptcpsv /usr/lib/sptcpsv/
	cp sptcpsv.h /usr/lib/sptcpsv/
rmprv:
	-rm /usr/lib/sptcpsv/sptcpsv /usr/lib/sptcpsv/sptcpsv.h
clean:
	-rm sptcpsv.o sptcpsv
