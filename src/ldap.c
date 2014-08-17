/* 
 * ldap.c - code to handle database connections etc.
 *
 * this file is part of GLADDB
 *
 * Copyright (c) 2012, 2013, 2014 Brett Sheffield <brett@gladserv.com>
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
#define _GNU_SOURCE
#define LDAP_DEPRECATED 1
#include "db.h"
#include "ldap.h"
#include <ldap.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

/* connect to ldap */
int db_connect_ldap(db_t *db)
{
        LDAP *l;
        int rc;
        int protocol = LDAP_VERSION3;

        rc = ldap_initialize(&l, db->host);
        if (rc != LDAP_SUCCESS) {
                syslog(LOG_DEBUG,
                  "Could not create LDAP session handle for URI=%s (%d): %s\n",
                  db->host, rc, ldap_err2string(rc));
                return -1;
        }
        syslog(LOG_DEBUG, "ldap_initialise() successful");

        /* Ensure we use LDAP v3 */
        rc = ldap_set_option(l, LDAP_OPT_PROTOCOL_VERSION, &protocol);

        db->conn = l;

        return 0;
}

/* disconnect from a mysql db */
int db_disconnect_ldap(db_t *db)
{
        if (db->conn != NULL) {
		int rc = ldap_unbind(db->conn);
		if (rc != LDAP_SUCCESS) {
                        syslog(LOG_ERR, "LDAP unbind error: %s (%d)",
                        ldap_err2string(rc), rc);
		}
                db->conn = NULL;
	}
	else {
                syslog(LOG_ERR, "Null pointer passed to LDAP unbind");
	}
        return 0;
}

int db_fetch_all_ldap(db_t *db, char *query, field_t *filter, row_t **rows,
        int *rowc)
{
        BerElement *ber;
        LDAPMessage *msg;
        LDAPMessage *res = NULL;
        char *a;
        char *lfilter = NULL;
        char *search;
        char **vals;
        int i;
        int rc;
        field_t *f;
        field_t *ftmp = NULL;
        row_t *r;
        row_t *rtmp = NULL;

        asprintf(&search, "%s,%s", query, db->db);
        rc = ldap_search_ext_s(db->conn, search, LDAP_SCOPE_SUBTREE,
                lfilter, NULL, 0, NULL, NULL, LDAP_NO_LIMIT,
                LDAP_NO_LIMIT, &res);
        free(search);

        if (rc != LDAP_SUCCESS) {
                syslog(LOG_DEBUG, "search error: %s (%d)",
                                ldap_err2string(rc), rc);
                return -1;
        }
        syslog(LOG_DEBUG, "ldap_search_ext_s successful");

        *rowc = ldap_count_messages(db->conn, res);
        syslog(LOG_DEBUG, "Messages: %i", *rowc);

        /* populate rows and fields */
        for (msg = ldap_first_entry(db->conn, res); msg != NULL;
        msg = ldap_next_entry(db->conn, msg))
        {
                r = malloc(sizeof(row_t));
                r->fields = NULL;
                r->next = NULL;
                for (a = ldap_first_attribute(db->conn, msg, &ber); a != NULL;
                a = ldap_next_attribute(db->conn, msg, ber))
                {
                        /* attributes may have more than one value - here
                           we list them all as separate fields */
                        if ((vals = ldap_get_values(db->conn, msg, a)) != NULL)
                        {
                                for (i = 0; vals[i] != NULL; i++) {
                                        f = malloc(sizeof(field_t));
                                        f->fname = strdup(a);
                                        f->fvalue = strdup(vals[i]);
                                        f->next = NULL;
                                        if (r->fields == NULL) {
                                                r->fields = f;
                                        }
                                        if (ftmp != NULL) {
                                                ftmp->next = f;
                                        }
                                        ftmp = f;
                                }
                                ldap_value_free(vals);
                        }
                        ldap_memfree(a);
                }
                ldap_memfree(ber);
                if (rtmp == NULL) {
                        /* as this is our first row, update the ptr */
                        *rows = r;
                }
                else {
                        rtmp->next = r;
                }
                ftmp = NULL;
                rtmp = r;
        }
        ldap_msgfree(res);
        return 0;
}

/* ldap add */
int db_insert_ldap(db_t *db, char *resource, keyval_t *data)
{
        /* TODO */
        return 0;
}

/* test credentials against ldap */
int db_test_bind(db_t *db, char *bindstr, char *bindattr,
        char *user, char *pass)
{
        char *binddn;
        int rc;

        if (db == NULL) {
                syslog(LOG_DEBUG, "NULL db passed to db_test_bind()");
                return -1;
        }

        db_connect_ldap(db);
        asprintf(&binddn, "%s=%s,%s,%s", bindattr, user, bindstr, db->db);
        fprintf(stderr, "%s\n", binddn);
        rc = ldap_simple_bind_s(db->conn, binddn, pass);
        free(binddn);
        if (rc != LDAP_SUCCESS) {
                syslog(LOG_DEBUG, "Bind error: %s (%d)",
                        ldap_err2string(rc), rc);
                db_disconnect_ldap(db);
                return -2;
        }
        db_disconnect_ldap(db);

        return 0;
}
