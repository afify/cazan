# cazan version
VERSION = 0.1

# paths
PREFIX    = /usr/local
MANPREFIX = ${PREFIX}/share/man

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=200112L
CFLAGS   = -ansi -pedantic -Wextra -Wall -O0 ${CPPFLAGS}
LDFLAGS  = -s -lm

# compiler and linker
CC = cc
