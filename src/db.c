/* 
 * db.c - code to handle database connections etc.
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

#ifndef D_NLDAP
#include "ldap.h"
#endif

#ifndef D_NMY
#include "my.h"
#endif

#ifndef D_NPG
#include "pg.h"
#endif

#ifndef D_NTDS
#include "tds.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

/* connect to specified database 
 * pointer to the connection is stored in db_t struct 
 * a wrapper for the database-specific functions
 */
int db_connect(db_t *db)
{
        if (db == NULL) {
                syslog(LOG_ERR, "No database info supplied to db_connect()\n");
                return -1;
        }
#ifndef _NPG
        if (strcmp(db->type, "pg") == 0) {
                return db_connect_pg(db);
        }
#endif
#ifndef _NMY
        if (strcmp(db->type, "my") == 0) {
                return db_connect_my(db);
        }
#endif
#ifndef _NTDS
        if (strcmp(db->type, "tds") == 0) {
                return db_connect_tds(db);
        }
#endif
#ifndef _NLDAP
        if (strcmp(db->type, "ldap") == 0) {
                return db_connect_ldap(db);
        }
#endif
        syslog(LOG_ERR,
                "Invalid database type '%s' passed to db_connect()\n",
                db->type);
        return -1;
}

/* wrapper for the database-specific db creation functions */
int db_create(db_t *db)
{
        if (db == NULL) {
                fprintf(stderr,
                        "No database info supplied to db_create()\n");
                return -1;
        }
        if (strcmp(db->type, "pg") == 0) {
                return db_create_pg(db);
        }
        else {
                fprintf(stderr,
                    "Invalid database type '%s' passed to db_create()\n",
                    db->type);
        }
        return 0;
}

/* wrapper for the database-specific disconnect functions */
int db_disconnect(db_t *db)
{
        if (db == NULL) {
                fprintf(stderr,
                        "No database info supplied to db_disconnect()\n");
                return -1;
        }
        if (db->conn == NULL) {
                return 0;
        }
#ifndef _NPG
        if (strcmp(db->type, "pg") == 0) {
                return db_disconnect_pg(db);
        }
#endif
#ifndef _NMY
        if (strcmp(db->type, "my") == 0) {
                return db_disconnect_my(db);
        }
#endif
#ifndef _NTDS
        if (strcmp(db->type, "tds") == 0) {
                return db_disconnect_tds(db);
        }
#endif
#ifndef _NLDAP
        if (strcmp(db->type, "ldap") == 0) {
                return db_disconnect_ldap(db);
        }
#endif
        fprintf(stderr,
            "Invalid database type '%s' passed to db_disconnect()\n",
            db->type);
        return -1;
}

/* execute some sql on a database
 * wrapper for db-specific functions */
int db_exec_sql(db_t *db, char *sql)
{
        int isconn = 0;

        if (db == NULL) {
                fprintf(stderr,
                        "No database info supplied to db_exec_sql()\n");
                return -1;
        }

        /* connect if we aren't already */
        if (db->conn == NULL) {
                if (db_connect(db) != 0) {
                        syslog(LOG_ERR, "Failed to connect to db on %s",
                                db->host);
                        return -1;
                }
                isconn = 1;
        }
#ifndef _NPG
        if (strcmp(db->type, "pg") == 0) {
                return db_exec_sql_pg(db, sql);
        }
#endif
#ifndef _NMY
        if (strcmp(db->type, "my") == 0) {
                return db_exec_sql_my(db, sql);
        }
#endif
#ifndef _NTDS
        if (strcmp(db->type, "tds") == 0) {
                return db_exec_sql_tds(db, sql);
        }
#endif
        fprintf(stderr,
            "Invalid database type '%s' passed to db_exec_sql()\n",
            db->type);

        /* leave the connection how we found it */
        if (isconn == 1)
                db_disconnect(db);

        return 0;
}

/* return all results from a SELECT
 * wrapper for db-specific functions */
int db_fetch_all(db_t *db, char *sql, field_t *filter, row_t **rows, int *rowc)
{
        if (db == NULL) {
                syslog(LOG_ERR,
                        "No database info supplied to db_fetch_all()\n");
                return -1;
        }
#ifndef _NPG
        if (strcmp(db->type, "pg") == 0) {
                return db_fetch_all_pg(db, sql, filter, rows, rowc);
        }
#endif
#ifndef _NMY
        if (strcmp(db->type, "my") == 0) {
                return db_fetch_all_my(db, sql, filter, rows, rowc);
        }
#endif
#ifndef _NTDS
        if (strcmp(db->type, "tds") == 0) {
                return db_fetch_all_tds(db, sql, filter, rows, rowc);
        }
#endif
#ifndef _NLDAP
        if (strcmp(db->type, "ldap") == 0) {
                return db_fetch_all_ldap(db, sql, filter, rows, rowc);
        }
#endif
        syslog(LOG_ERR,
            "Invalid database type '%s' passed to db_fetch_all()\n",
            db->type);
        return -1;
}

/* database agnostic resource insertion */
int db_insert(db_t *db, char *resource, keyval_t *data)
{
        if (db == NULL) {
                syslog(LOG_ERR,
                        "No database info supplied to db_insert()\n");
                return -1;
        }
#if !defined(_NPG) || !defined(_NMY) || !defined(_NTDS)
        if ((strcmp(db->type, "pg") == 0) || (strcmp(db->type, "my") == 0) ||
            (strcmp(db->type, "tds") == 0))
        {
                return db_insert_sql(db, resource, data);
        }
#endif
#ifndef _NLDAP
        if (strcmp(db->type, "ldap") == 0) {
                return db_insert_ldap(db, resource, data);
        }
#endif
        syslog(LOG_ERR, "Invalid database type '%s' passed to db_insert()\n",
                db->type);
        return -1;
}

/* INSERT into sql database */
int db_insert_sql(db_t *db, char *resource, keyval_t *data)
{
        char *flds = NULL;
        char *sql;
        char *vals = NULL;
        char *tmpflds = NULL;
        char *tmpvals = NULL;
        char quot = '\'';
        int rval;
        int isconn = 0;

        /* use backticks to quote mysql */
        if (strcmp(db->type, "my") == 0)
                quot = '"';

        /* build INSERT sql from supplied data */
        while (data != NULL) {
                fprintf(stderr, "%s = %s\n", data->key, data->value);
                if (flds == NULL) {
                        asprintf(&flds, "%s", data->key);
                        asprintf(&vals, "%1$c%2$s%1$c", quot, data->value);
                }
                else {
                        tmpflds = strdup(flds);
                        tmpvals = strdup(vals);
                        free(flds); free(vals);
                        asprintf(&flds, "%s,%s", tmpflds, data->key);
                        asprintf(&vals, "%2$s,%1$c%3$s%1$c",
                                quot, tmpvals, data->value);
                        free(tmpflds); free(tmpvals);
                }
                data = data->next;
        }

        asprintf(&sql,"INSERT INTO %s (%s) VALUES (%s)", resource, flds, vals);
        free(flds); free(vals);
        syslog(LOG_DEBUG, "%s", sql);

        if (db->conn == NULL) {
                if (db_connect(db) != 0) {
                        syslog(LOG_ERR, "Failed to connect to db on %s",
                                db->host);
                        return -1;
                }
                isconn = 1;
        }

        rval = db_exec_sql(db, sql);
        free(sql);

        /* leave the connection how we found it */
        if (isconn == 1)
                db_disconnect(db);

        return rval;
}

/* return field with name fname from provided row */
field_t * db_field(row_t *row, char *fname)
{
        field_t *f;

        f = row->fields;
        while (f != NULL) {
                if (strcmp(fname, f->fname) == 0)
                        return f;
                f = f->next;
        }

        return '\0';
}

/* free database struct */
void db_free(db_t *dbs)
{
        db_t *d;
        db_t *tmp;

        d = dbs;
        while (d != NULL) {
                free(d->alias);
                free(d->type);
                free(d->host);
                free(d->db);
                free(d->user);
                free(d->pass);
                tmp = d;
                d = d->next;
                free(tmp);
        }
        dbs = NULL;
}

/* return the db_t pointer for this db alias */
db_t *db_get(db_t *dbs, char *alias)
{
        db_t *db;

        db = dbs;
        while (db != NULL) {
                if (strcmp(alias, db->alias) == 0)
                        return db;
                db = db->next;
        }

        return NULL; /* db not found */
}

/* free field_t struct */
void free_fields(field_t *f)
{
        field_t *next_f = NULL;
        while (f != NULL) {
                free(f->fname);
                free(f->fvalue);
                next_f = f->next;
                free(f);
                f = next_f;
        }
}

/* free row_t struct */
void liberate_rows(row_t *r)
{
        row_t *next_r = NULL;
        while (r != NULL) {
                free_fields(r->fields);
                next_r = r->next;
                free(r);
                r = next_r;
        }
}
