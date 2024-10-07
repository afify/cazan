# cazan - simple muslim prayers calculator
# print next prayer left duration or today's all prayers.
# See LICENSE file for copyright and license details.

include config.mk

SRC = cazan.c util.c
OBJ = ${SRC:.c=.o}

all: options cazan

options:
	@echo cazan build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "LDFLAGS  = ${LDFLAGS}"
	@echo "CC       = ${CC}"

.c.o:
	${CC} -c ${CFLAGS} $<

${OBJ}: config.mk

cazan: ${OBJ}
	${CC} ${LDFLAGS} -o $@ ${OBJ}

clean:
	rm -f cazan ${OBJ} cazan-${VERSION}.tar.gz

dist: clean
	mkdir -p cazan-${VERSION}
	cp -R LICENSE Makefile README.md config.def.h config.mk\
		cazan.1 cazan.png util.h ${SRC} cazan-${VERSION}
	tar -cf cazan-${VERSION}.tar cazan-${VERSION}
	gzip cazan-${VERSION}.tar
	rm -rf cazan-${VERSION}

install: cazan
	mkdir -p ${DESTDIR}${PREFIX}/bin
	cp -f cazan ${DESTDIR}${PREFIX}/bin
	chmod 755 ${DESTDIR}${PREFIX}/bin/cazan
	mkdir -p ${DESTDIR}${MANPREFIX}/man1
	sed "s/VERSION/${VERSION}/g" < cazan.1 > ${DESTDIR}${MANPREFIX}/man1/cazan.1
	chmod 644 ${DESTDIR}${MANPREFIX}/man1/cazan.1

uninstall:
	rm -f ${DESTDIR}${PREFIX}/bin/cazan\
		${DESTDIR}${MANPREFIX}/man1/cazan.1

.PHONY: all options clean dist install uninstall
