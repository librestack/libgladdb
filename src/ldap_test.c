/* 
 * ldap_test.c - tests for ldap functions
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

#include "db.h"
#include "ldap.h"
#include <ldap.h>
#include <stdio.h>
#include <stdlib.h>

int ldap_test_db_connect_ldap(db_t *db);
int ldap_test_db_disconnect_ldap(db_t *db);
int ldap_test_db_test_bind_good(db_t *db);
int ldap_test_db_test_bind_bad(db_t *db);
int main();

int main()
{
        int rc;
        struct db_t *db = malloc(sizeof (db_t));

        db->alias = "ldapdb";
        db->type = "ldap";
        db->host = "ldap://ldaptestserver";
        db->db = "dc=gladserv,dc=com";
        db->user = "betty";
        db->pass = "ie5a8P40";
        db->conn = NULL;
        db->next = NULL;

        printf("ldap_test_db_connect_ldap...");
        rc = ldap_test_db_connect_ldap(db);
        if (rc != 0) goto cleanup_main;
        printf("OK\n");

        printf("ldap_test_db_disconnect_ldap...");
        rc = ldap_test_db_disconnect_ldap(db);
        if (rc != 0) goto cleanup_main;
        printf("OK\n");

        printf("ldap_test_db_test_bind_good...");
        rc = ldap_test_db_test_bind_good(db);
        if (rc != 0) goto cleanup_main;
        printf("OK\n");

        printf("ldap_test_db_test_bind_bad...");
        rc = ldap_test_db_test_bind_bad(db);
        if (rc != 0) goto cleanup_main;
        printf("OK\n");

cleanup_main:
        printf("\n");
        free(db);
        return rc;
}

/* test connect to ldap */
int ldap_test_db_connect_ldap(db_t *db)
{
        int rc;
        rc = db_connect_ldap(db);
        return rc;
}

/* test disconnect from ldap */
int ldap_test_db_disconnect_ldap(db_t *db)
{
        int rc;
        rc = db_disconnect_ldap(db);
        return rc;
}

/* test ldap bind with bad credentials - MUST succeed */
int ldap_test_db_test_bind_good(db_t *db)
{
        int rc;
        rc = db_test_bind(db, "ou=people", "uid", "betty", "ie5a8P40");
        return rc;
}

/* test ldap bind with bad credentials - MUST fail */
int ldap_test_db_test_bind_bad(db_t *db)
{
        int rc;
        rc = db_test_bind(db, "ou=people", "uid", "betty", "badpass");
        return (rc == 0) ? 1 : 0;
}
