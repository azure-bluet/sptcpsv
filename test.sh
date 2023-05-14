rm -r /usr/lib/sptcpsv/
bash build
cd main
gcc -shared -fPIC test.c /usr/lib/sptcpsv/utils.so -o main.so -I/usr/lib/sptcpsv -O3
echo -e "Compilation finished. Start running."
/usr/lib/sptcpsv/sptcpsv 80
cd ..
