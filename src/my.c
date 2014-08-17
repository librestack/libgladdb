/* 
 * my.c - code to handle database connections etc.
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
#include "db.h"
#include "my.h"
#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

/* connect to mysql database */
int db_connect_my(db_t *db)
{
        MYSQL *conn;

        conn = mysql_init(NULL);
        if (conn == NULL) {
                syslog(LOG_ERR, "%u: %s\n", mysql_errno(conn),
                                            mysql_error(conn));
                return -1;
        }

        if (mysql_real_connect(conn, db->host, db->user, db->pass, db->db,
                                                        0, NULL, 0) == NULL)
        {
                syslog(LOG_ERR, "%u: %s\n", mysql_errno(conn),
                                            mysql_error(conn));
                return -1;
        }
        db->conn = conn;

        return 0;
}

/* disconnect from a mysql db */
int db_disconnect_my(db_t *db)
{
        mysql_close(db->conn);
        mysql_library_end();
        db->conn = NULL;

        return 0;
}

/* execute sql against a mysql db */
int db_exec_sql_my(db_t *db, char *sql)
{
        if (mysql_query(db->conn, sql) != 0) {
                syslog(LOG_ERR, "%u: %s\n", mysql_errno(db->conn), 
                                              mysql_error(db->conn));
                return -1;
        }
        return 0;
}

/* return all results from a SELECT - mysql */
int db_fetch_all_my(db_t *db, char *sql, field_t *filter, row_t **rows,
        int *rowc)
{
        MYSQL_RES *res;
        MYSQL_ROW row;
        MYSQL_FIELD *fields;
        int i;
        int nFields;
        field_t *f;
        field_t *ftmp = NULL;
        row_t *r;
        row_t *rtmp = NULL;
        char *sqltmp;
        char *join;

        *rowc = 0;

        if (filter != NULL) {
                join = (strcasestr(sql, "WHERE") == NULL) ? "WHERE" : "AND";
                sqltmp = strdup(sql);
                asprintf(&sql, "%s %s %s=\"%s\"", sqltmp, join, filter->fname,
                        filter->fvalue);
                free(sqltmp);
        }

        if (mysql_query(db->conn, sql) != 0) {
                syslog(LOG_ERR, "%u: %s\n%s", mysql_errno(db->conn), 
                                              mysql_error(db->conn), sql);
                return -1;
        }

        res = mysql_store_result(db->conn);
        *rowc = mysql_num_rows(res);

        /* populate rows and fields */
        fields = mysql_fetch_fields(res);
        nFields = mysql_num_fields(res);
        while ((row = mysql_fetch_row(res))) {
                r = malloc(sizeof(row_t));
                r->fields = NULL;
                r->next = NULL;
                for (i = 0; i < nFields; i++) {
                        f = malloc(sizeof(field_t));
                        f->fname = strdup(fields[i].name);
                        asprintf(&f->fvalue, "%s", row[i] ? row[i] : "NULL");
                        f->next = NULL;
                        if (r->fields == NULL) {
                                r->fields = f;
                        }
                        if (ftmp != NULL) {
                                ftmp->next = f;
                        }
                        ftmp = f;
                }
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
        mysql_free_result(res);

        return 0;
}
