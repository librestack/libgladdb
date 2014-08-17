/* 
 * pg.c - code to handle database connections etc.
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
#include "pg.h"
#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

/* connect to a postgresql database */
int db_connect_pg(db_t *db)
{
        char *conninfo;
        PGconn *conn;
        int retval = 0;

        if (asprintf(&conninfo,
                "host = %s dbname = %s", db->host, db->db) == -1)
        {
                fprintf(stderr,
                        "asprintf failed for conninfo in db_connect_pg()\n");
                return -2;
        }
        conn = PQconnectdb(conninfo);
        free(conninfo);
        if (PQstatus(conn) != CONNECTION_OK)
        {
                syslog(LOG_ERR, "connection to database failed.\n");
                syslog(LOG_DEBUG, "%s", PQerrorMessage(conn));
                fprintf(stderr, "%s", PQerrorMessage(conn));
                retval = PQstatus(conn);
                PQfinish(conn);
                return retval;
        }
        db->conn = conn;
        return 0;
}

/* TODO: db_create_pg() */
int db_create_pg(db_t *db)
{
        return 0;
}

/* disconnect from a postgresql db */
int db_disconnect_pg(db_t *db)
{
        PQfinish(db->conn);
        db->conn = NULL;

        return 0;
}

/* execute sql against a postgresql db */
int db_exec_sql_pg(db_t *db, char *sql)
{
        PGresult *res;

        res = PQexec(db->conn, sql);
        int status = PQresultStatus(res);
        if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
                syslog(LOG_ERR,
                       "SQL exec failed: %s", PQerrorMessage(db->conn));
                PQclear(res);
                return -1;
        }
        PQclear(res);

        return 0;
}

/* return all results from a SELECT - postgres */
int db_fetch_all_pg(db_t *db, char *sql, field_t *filter, row_t **rows,
        int *rowc)
{
        PGresult *res;
        int i, j;
        int nFields;
        field_t *f;
        field_t *ftmp = NULL;
        row_t *r;
        row_t *rtmp = NULL;
        char *sqltmp;

        if (filter != NULL) {
                sqltmp = strdup(sql);
                asprintf(&sql, "%s WHERE %s='%s'", sqltmp, filter->fname,
                        filter->fvalue);
                free(sqltmp);
        }

        res = PQexec(db->conn, sql);
        int status = PQresultStatus(res);
        if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
                syslog(LOG_ERR, "query failed: %s",
                        PQerrorMessage(db->conn));
                return -1;
        }

        /* populate rows and fields */
        nFields = PQnfields(res);
        for (j = 0; j < PQntuples(res); j++) {
                r = malloc(sizeof(row_t));
                r->fields = NULL;
                r->next = NULL;
                for (i = 0; i < nFields; i++) {
                        f = malloc(sizeof(field_t));
                        f->fname = strdup(PQfname(res, i));
                        f->fvalue = strdup(PQgetvalue(res, j, i));
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
        *rowc = PQntuples(res);

        PQclear(res);

        return 0;
}
