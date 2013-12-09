
include ../Makefile.config

libnames = guampsio gmx_t gmxutil output
# guampsio gmxutil types
libobjs = $(addsuffix .o, ${libnames})

prognames = get set test2
progobjs = $(addsuffix .o, ${prognames})

allobjs = $(join ${libobjs}, ${progobjs})

.PHONY: all
all : ${libobjs} ${prognames}

# get : ${objects}
# 	${cc} -lgmx $^ -o $@

get : ${libobjs} get.o
	${cc} ${ldflags} $^ -o $@

set : ${libobjs} set.o
	${cc} ${ldflags} $^ -o $@


test2 : ${libobjs} test2.o
	${cc} ${ldflags} $^ -o $@


%.o : %.c ${myinc}/*.h ../Makefile.config
	${cc} ${cflags} ${includes} -c $< -o $@


.PHONY: clean
clean :
	rm -f *.o ${objects} ${prognames}
