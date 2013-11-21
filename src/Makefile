
include ../Makefile.config

names = io gmxutil get
objects = $(addsuffix .o, ${names})

programs = get

.phony: all
all : ${programs} ${objects}

get : ${objects}
	${cc} -lgmx $^ -o $@

%.o : %.c ${myinc}/*.h ../Makefile.config
	${cc} ${cflags} ${includes} -c $< -o $@


.phony: clean
clean :
	rm -f *.o ${programs}
