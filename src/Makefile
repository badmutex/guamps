
include ../Makefile.config

libnames = io gmxutil
libobjs = $(addsuffix .o, ${libnames})

prognames = get test
progobjs = $(addsuffix .o, ${prognames})

allobjs = $(join ${libobjs}, ${progobjs})

.phony: all
all : ${libobjs} ${prognames}

# get : ${objects}
# 	${cc} -lgmx $^ -o $@

get : ${libobjs} get.o
	${cc} ${ldflags} $^ -o $@


test : ${libobjs} test.o
	${cc} ${ldflags} $^ -o $@


%.o : %.c ${myinc}/*.h ../Makefile.config
	${cc} ${cflags} ${includes} -c $< -o $@


.phony: clean
clean :
	rm -f *.o ${objects} ${prognames}
