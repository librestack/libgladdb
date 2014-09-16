/* 
 * ldif.c - process ldif data
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

#include "ldif.h"
#include <stdio.h>
#include <syslog.h>

LDAP_LDIF_F(LDIFFP *) ldif_open_mem(char *ldif, size_t size, const char *mode)
{
	FILE *fp;
	LDIFFP *lfp = NULL;

	fp = fmemopen(ldif, size, mode);
	if (fp) {
		lfp = ber_memalloc(sizeof(LDIFFP));
		lfp->fp = fp;
		lfp->prev = NULL;
	}

	return lfp;
}

int process_ldif(db_t *db, LDIFFP *fp)
{
	int rc = 0;
	char *bufp = NULL;
	int buflen = 0;
	struct berval rbuf;
	unsigned long linenum = 0;
	LDIFRecord lr;
	unsigned int flags = 0;

	while (rc == 0) {
		rc = ldif_read_record(fp, &linenum, &bufp, &buflen);
		if (rc == 0) {
			syslog(LOG_DEBUG, "EOF reached for ldif data");
			return 0;
		}
		else if (rc == -1) {
			syslog(LOG_ERR, "error reading ldif record");
			return 0;
		}
		rbuf.bv_val = bufp;
		rbuf.bv_len = 0;
		rc = ldap_parse_ldif_record(&rbuf, linenum, &lr, "libgladdb",
				flags);
		if (rc != 0) {
			syslog(LOG_DEBUG, "failed to parse ldif record");
			return 0;
		}
		if (lr.lr_op == LDAP_REQ_DELETE) {
			syslog(LOG_DEBUG, "DELETE resulting from ldif record");
			rc = ldap_delete_ext_s(db->conn, lr.lr_dn.bv_val,
				lr.lr_ctrls, NULL);
		}
		else if (lr.lr_op == LDAP_REQ_RENAME) {
			syslog(LOG_DEBUG, "RENAME resulting from ldif record");
			char *newparent = NULL;
			if (lr.lrop_newsup.bv_val) {
				newparent = lr.lrop_newsup.bv_val;
			}
			rc = ldap_rename_s(db->conn, lr.lr_dn.bv_val,
				lr.lrop_newrdn.bv_val, newparent,
				lr.lrop_delold, lr.lr_ctrls, NULL);
		}
		else if (lr.lr_op == LDAP_REQ_ADD) {
			syslog(LOG_DEBUG, "ADD resulting from ldif record");
			rc = ldap_add_ext_s(db->conn, lr.lr_dn.bv_val,
				lr.lrop_mods, lr.lr_ctrls, NULL);
		}
		else if (lr.lr_op == LDAP_REQ_MODIFY) {
			syslog(LOG_DEBUG, "MODIFY resulting from ldif record");
			rc = ldap_modify_ext_s(db->conn, lr.lr_dn.bv_val,
				lr.lrop_mods, lr.lr_ctrls, NULL);
		}
		else {
			syslog(LOG_DEBUG, "ldif record skipped");
		}
	}

	return 1;
}
