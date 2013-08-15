# Makefile for gladdb
# 
# this file is part of GLADDB
# 
# Copyright (c) 2012, 2013 Brett Sheffield <brett@gladserv.com>
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program (see the file COPYING in the distribution).
# If not, see <http://www.gnu.org/licenses/>.

CC=gcc

# the following are valid in CFLAGS:
# -D_NPG   => compile without postgres support
# -D_NMY   => compile without mysql support
# -D_NTDS  => compile without freetds support
# -D_NLDAP => compile without ldap support
CFLAGS=-g -Wall -Werror

INCLUDES=-I/usr/include/postgresql
MYSQLFLAGS=`mysql_config --cflags --libs`
LIBS=-L/usr/lib/x86_64-linux-gnu -lpq ${MYSQLFLAGS} -lsybdb -lmysqlclient -lldap
LIBDIR=/usr/local/lib
INCLUDEDIR=/usr/local/include/${LIBNAME}/
LIBNAME=gladdb
LIBFILE=lib${LIBNAME}.so

${LIBFILE}.1.0: tds.o db.o
	${CC} ${CFLAGS} -shared -W1,-soname,${LIBFILE}.1 -o ${LIBFILE}.1.0 tds.o db.o

db.o: db.c db.h
	${CC} ${CFLAGS} -fpic ${INCLUDES} -c db.c ${LIBS}

tds.o: tds.c tds.h
	${CC} ${CFLAGS} -fpic ${INCLUDES} -c tds.c ${LIBS}

.PHONY: clean

clean:
	rm *.so*
	rm *.o

${INCLUDEDIR}:
	mkdir ${INCLUDEDIR}

install: ${LIBFILE}.1.0 | ${INCLUDEDIR}
	cp ${LIBFILE}.1.0 ${LIBDIR}
	ln -sf ${LIBDIR}/${LIBFILE}.1.0 ${LIBDIR}/${LIBFILE}.1
	ln -sf ${LIBDIR}/${LIBFILE}.1 ${LIBDIR}/${LIBFILE}
	cp db.h ${INCLUDEDIR}
	cp tds.h ${INCLUDEDIR}
