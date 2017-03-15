/* 
 * lmdb_test.c - tests for lmdb functions
 *
 * this file is part of GLADDB
 *
 * Copyright (c) 2014 Brett Sheffield <brett@gladserv.com>
 * Copyright (c) 2017 Gavin Henry <ghenry@suretec.co.uk>, Suretec 
 *  Systems Ltd. T/A SureVoIP
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

#include "db.h"
#include "lmdb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int lmdb_test_db_connect_lmdb(db_t *db);
int lmdb_test_db_disconnect_lmdb(db_t *db);
int lmdb_test_db_fetch_all_lmdb(db_t *db);
int lmdb_test_db_get_lmdb(db_t *db);
int lmdb_test_db_insert_lmdb(db_t *db);
int lmdb_test_db_delete_lmdb(db_t *db);

int main(void)
{
        int rc;
        struct db_t *db = calloc(1, sizeof (db_t));

        db->alias = "lmdbdb";
        db->type = "lmdb";
        db->db = "gladdb_test_lmdb_db";
        db->conn = NULL;
        db->next = NULL;

        /*  Messy. Loop all these? Consider Unity C test harness later. */
        printf("lmdb_test_db_connect_lmdb...");
        rc = lmdb_test_db_connect_lmdb(db);
        if (rc != 0) goto cleanup_main;
        printf("OK\n");

        printf("lmdb_test_db_insert_lmdb...");
        rc = lmdb_test_db_insert_lmdb(db);
        if (rc != 0) goto cleanup_main;
        printf("OK\n");

        printf("lmdb_test_db_get_lmdb...");
        rc = lmdb_test_db_get_lmdb(db);
        if (rc != 0) goto cleanup_main;
        printf("OK\n");

        printf("lmdb_test_db_delete_lmdb...");
        rc = lmdb_test_db_delete_lmdb(db);
        if (rc != 0) goto cleanup_main;
        printf("OK\n");

        printf("lmdb_test_db_disconnect_lmdb...");
        rc = lmdb_test_db_disconnect_lmdb(db);
        if (rc != 0) goto cleanup_main;
        printf("OK\n");

cleanup_main:
        printf("\n");
        free(db);
        return rc;
}

/* test connect to lmdb */
int lmdb_test_db_connect_lmdb(db_t *db)
{
        int rc;
        rc = db_connect_lmdb(db);
        return rc;
}

/* test disconnect from lmdb */
int lmdb_test_db_disconnect_lmdb(db_t *db)
{
        int rc;
        rc = db_disconnect_lmdb(db);
        return rc;
}

/* test lmdb fetch all */
int lmdb_test_db_fetch_all_lmdb(db_t *db)
{
        return EXIT_SUCCESS;
}

/* test lmdb get */
int lmdb_test_db_get_lmdb(db_t *db)
{
        int rc, ret;
        keyval_t orig_data = { "SureVoIP", "rules!", NULL };
        keyval_t req_data = { "SureVoIP", NULL, NULL };
        rc = db_get_lmdb(db, NULL, &req_data);
        printf("got value: %s for key: %s...",
                req_data.value, req_data.key);
        
        ret = strcmp(req_data.value,orig_data.value);
        if (ret == 0) {
            return ret;
        } 

        return rc;
}

/* test lmdb put */
int lmdb_test_db_insert_lmdb(db_t *db)
{
        int rc;
        keyval_t data = { "SureVoIP", "rules!", NULL };
        printf("inserting key: %s, value: %s...",
                data.key, data.value);
        rc = db_insert_lmdb(db, NULL, &data);
        return rc;
}

/* test lmdb delete */
int lmdb_test_db_delete_lmdb(db_t *db)
{
        int rc;
        keyval_t data = { "SureVoIP", NULL, NULL };
        rc = db_delete_lmdb(db, NULL, &data);
        return rc;
}
