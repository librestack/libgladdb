/* 
 * db.h - code to handle database connections etc.
 *
 * this file is part of GLADDB
 *
 * Copyright (c) 2012, 2013 Brett Sheffield <brett@gladserv.com>
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

#ifndef __GLADDB_DB_H__
#define __GLADDB_DB_H__ 1

typedef struct db_t {
        char *alias; /* a convenient handle to refer to this db by */
        char *type;  /* type of database: pg, my, ldap, tds */
        char *host;  /* hostname or ip for this database eg. "localhost" */
        char *db;    /* name of the database */
        char *user;  /* username (mysql) */
        char *pass;  /* password (mysql) */
        void *conn;  /* pointer to open db connection */
        struct db_t *next; /* pointer to next db so we can loop through them */
} db_t;

typedef struct field_t {
        char *fname;
        char *fvalue;
        struct field_t *next;
} field_t;

typedef struct keyval_t {
        char *key;
        char *value;
        struct keyval_t *next;
} keyval_t;

typedef struct row_t {
        struct field_t *fields;
        struct row_t *next;
} row_t;

int count_keyvals(keyval_t *kv, int *total, int *unique);
int db_connect(db_t *db);
int db_create(db_t *db);
int db_disconnect(db_t *db);
int db_exec_sql(db_t *db, char *sql);
int db_fetch_all(db_t *db, char *sql, field_t *filter, row_t **rows,
        int *rowc);
int db_insert(db_t *db, char *resource, keyval_t *data);
int db_insert_sql(db_t *db, char *resource, keyval_t *data);
field_t * db_field(row_t *row, char *fname);
void db_free();
db_t *db_get(db_t *dbs, char *alias);
void free_fields(field_t *f);
void liberate_rows(row_t *r);

#endif /* __GLADDB_DB_H__ */
