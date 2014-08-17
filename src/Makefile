# Makefile for gladdb
# 
# this file is part of GLADDB
# 
# Copyright (c) 2012, 2013, 2014 Brett Sheffield <brett@gladserv.com>
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
CFLAGS := -g -Wall -Werror

# include if defined
DB_LDAP := 1 # ldap
DB_MY := 1   # mysql
DB_PG := 1   # postgresql
DB_TDS := 1  # freetds

INCLUDES :=
LIBS :=-L/usr/lib/x86_64-linux-gnu
OBJECTFILES :=db.o
HEADERFILES :=db.h

ifdef DB_LDAP
	LIBS := $(LIBS) -lldap
	OBJECTFILES := $(OBJECTFILES) ldap.o
	HEADERFILES := $(HEADERFILES) ldap.h
else
	CFLAGS := $(CFLAGS) -D_NLDAP
endif

ifdef DB_MY
	LIBS := $(LIBS) `mysql_config --cflags --libs` -lmysqlclient
	OBJECTFILES := $(OBJECTFILES) my.o
	HEADERFILES := $(HEADERFILES) my.h
else
	CFLAGS := $(CFLAGS) -D_NMY
endif

ifdef DB_PG
	INCLUDES := $(INCLUDES) -I/usr/include/postgresql
	LIBS := $(LIBS) -lpq
	OBJECTFILES := $(OBJECTFILES) pg.o
	HEADERFILES := $(HEADERFILES) pg.h
else
	CFLAGS := $(CFLAGS) -D_NPG
endif

ifdef DB_TDS
	LIBS := $(LIBS) -lsybdb
	OBJECTFILES := $(OBJECTFILES) tds.o
	HEADERFILES := $(HEADERFILES) tds.h
else
	CFLAGS := $(CFLAGS) -D_NTDS
endif

LIBDIR=/usr/local/lib
INCLUDEDIR=/usr/local/include/${LIBNAME}/
LIBNAME=gladdb
LIBFILE=lib${LIBNAME}.so

${LIBFILE}.1.0: $(OBJECTFILES)
	${CC} $(CFLAGS) -shared -Wl,-soname,${LIBFILE}.1 -o ${LIBFILE}.1.0 $(OBJECTFILES)

db.o: db.c db.h
	${CC} $(CFLAGS) -fpic $(INCLUDES) -c db.c $(LIBS)

ifdef DB_LDAP
ldap.o: ldap.c ldap.h
	${CC} $(CFLAGS) -fpic $(INCLUDES) -c ldap.c $(LIBS)
endif

ifdef DB_MY
my.o: my.c my.h
	${CC} $(CFLAGS) -fpic $(INCLUDES) -c my.c $(LIBS)
endif

ifdef DB_PG
pg.o: pg.c pg.h
	${CC} $(CFLAGS) -fpic $(INCLUDES) -c pg.c $(LIBS)
endif

ifdef DB_TDS
tds.o: tds.c tds.h
	${CC} $(CFLAGS) -fpic $(INCLUDES) -c tds.c $(LIBS)
endif

.PHONY: clean

clean:
	rm -f *.so*
	rm -f *.o

${INCLUDEDIR}:
	mkdir ${INCLUDEDIR}

install: ${LIBFILE}.1.0 | ${INCLUDEDIR}
	cp ${LIBFILE}.1.0 ${LIBDIR}
	ln -sf ${LIBDIR}/${LIBFILE}.1.0 ${LIBDIR}/${LIBFILE}.1
	ln -sf ${LIBDIR}/${LIBFILE}.1 ${LIBDIR}/${LIBFILE}
	cp $(HEADERFILES) ${INCLUDEDIR}
