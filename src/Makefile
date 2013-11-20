
include ../Makefile.config

coords : coords.o io.o
	${cc} ${cflags} -lgmx $^ -o $@

%.o : %.c ../include/%.h
	$(cc) $(cflags) ${includes} -c $< -o $@

