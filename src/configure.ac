# configure.ac for libgladdb
# 
# this file is part of GLADDB
# 
# Copyright (c) 2015-2016 Brett Sheffield <brett@gladserv.com>
# Copyright (c) 2017 Gavin Henry <ghenry@suretec.co.uk>, Suretec 
#  Systems Ltd. T/A SureVoIP
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

# To build configure script:
#   autoconf configure.ac > configure

AC_INIT([libgladdb], [0.2.11], [brett@gladserv.com])
AC_PROG_CC
AC_LANG(C)
AC_PREFIX_DEFAULT([/usr/local/bin])
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_FUNC_MALLOC

AC_CHECK_HEADERS([stdio.h stdlib.h string.h syslog.h])

AC_ARG_WITH(ldif, [ --without-ldif disable ldif])
AC_ARG_WITH(mysql, [ --without-mysql disable mysql])
AC_ARG_WITH(openldap, [ --without-openldap disable openldap])
AC_ARG_WITH(postgres, [ --without-postgres disable postgres])
AC_ARG_WITH(tds, [ --without-tds disable tds support])
AC_ARG_WITH(lmdb, [ --without-lmdb disable lmdb support])

ldap=0
AC_CHECK_LIB([ldap], [ldap_initialize], [ldap=1] , [ldap=0])
if test "$with_openldap" == "no"
then
        ldap=0
fi
AC_SUBST(ldap, $ldap)
if test "$ldap" == "0"
then
        AC_MSG_NOTICE([disabling openldap support])
fi

ldif=0
AC_CHECK_LIB([ldap], [ldap_parse_ldif_record], [ldif=1] , [ldif=0])
if test "$with_ldif" == "no"
then
        ldif=0
fi
AC_SUBST(ldif, $ldif)
if test "$ldif" == "0"
then
        AC_MSG_NOTICE([disabling ldif support])
fi

mysql=0
AC_CHECK_LIB([mysqlclient], [mysql_init], [mysql=1] , [mysql=0])
if test "$with_mysql" == "no"
then
        mysql=0
fi
AC_SUBST(mysql, $mysql)
if test "$mysql" == "0"
then
        AC_MSG_NOTICE([disabling mysql/mariadb support])
fi

postgres=0
AC_CHECK_LIB([pq], [PQconnectdb], [postgres=1] , [postgres=0])
if test "$with_postgres" == "no"
then
        postgres=0
fi
AC_SUBST(postgres, $postgres)
if test "$postgres" == "0"
then
        AC_MSG_NOTICE([disabling postgresql support])
fi

freetds=0
AC_CHECK_LIB([sybdb], [dbinit], [freetds=1] , [freetds=0])
if test "$with_freetds" == "no"
then
        freetds=0
fi
AC_SUBST(freetds, $freetds)
if test "$freetds" == "0"
then
        AC_MSG_NOTICE([disabling freetds support])
fi

lmdb=0
AC_CHECK_LIB([lmdb], [mdb_env_create], [lmdb=1] , [lmdb=0])
if test "$with_lmdb" == "no"
then
        lmdb=0
fi
AC_SUBST(lmdb, $lmdb)
if test "$lmdb" == "0"
then
        AC_MSG_NOTICE([disabling lmdb support])
fi


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
