NAME = awm

X11INC = /usr/include/X11
X11LIB = /usr/lib/X11

CFLAGS = -I${X11INC} -Os
LDFLAGS = -L${X11LIB} -lX11

PREFIX = /usr/local

all: ${NAME}

${NAME}: awm.c config.h
	gcc -o ${NAME} awm.c ${CFLAGS} ${LDFLAGS}

clean:
	rm -f ${NAME}

install: all
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f ${NAME} ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/${NAME}

uninstall: 
	rm -rf ${DESTDIR}${PREFIX}/bin/${NAME}

.PHONY: all clean install uninstall
