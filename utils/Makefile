utilobj = utils.o logfile.o queue.o
utils.so: $(utilobj)
	gcc -shared $(utilobj) -o utils.so
utils.o: utils.c utils.h
	gcc -c utils.c -o utils.o -O3
logfile.o: logfile.c logfile.h
	gcc -c logfile.c -o logfile.o -O3
queue.o: queue.h queue.c
	gcc -c queue.c -o queue.o -O3
.PHONY: clean install rmprv
install:
	-mkdir /usr/lib/sptcpsv
	cp utils.so /usr/lib/sptcpsv/
	cp *.h /usr/lib/sptcpsv/
rmprv:
	-rm /usr/lib/sptcpsv/queue.h /usr/lib/sptcpsv/utils.h \
	/usr/lib/sptcpsv/logfile.h /usr/lib/sptcpsv/exitv.h \
	/usr/lib/sptcpsv/utils.so
clean:
	-rm utils.o logfile.o queue.o utils.so
