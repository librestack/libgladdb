/* 
 * ldif.h - process ldif data
 *
 * this file is part of GLADDB
 *
 * Copyright (c) 2014 Brett Sheffield <brett@gladserv.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program (see the file COPYING in the distribution).
 * If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __GLADDB_LDIF_H__
#define __GLADDB_LDIF_H__ 1

#include "db.h"
#include <stdio.h>
#include <ldap.h>
#include <ldif.h>

LDAP_LDIF_F(LDIFFP *) ldif_open_mem(char *ldif, size_t size, const char *mode);

int process_ldif(db_t *db, LDIFFP *fp);

#endif /* __GLADDB_LDIF_H__ */
