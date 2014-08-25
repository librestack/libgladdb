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
#include <string.h>

int ldap_test_db_connect_ldap(db_t *db);
int ldap_test_db_disconnect_ldap(db_t *db);
int ldap_test_db_test_bind_good(db_t *db);
int ldap_test_db_test_bind_bad(db_t *db);
int ldap_test_keyval_to_LDAPMod();
int ldap_test_db_insert_ldap();
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

        printf("ldap_test_keyval_to_LDAPMod...");
        rc = ldap_test_keyval_to_LDAPMod();
        if (rc != 0) goto cleanup_main;
        printf("OK\n");

        printf("ldap_test_db_insert_ldap...");
        rc = ldap_test_db_insert_ldap();
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

/* test ldap add */
int ldap_test_db_insert_ldap()
{
        /*
        dn: uid=wilma,ou=People,dc=gladserv,dc=com
        uid: wilma
        objectClass: inetOrgPerson
        objectClass: posixAccount
        objectClass: shadowAccount
        objectClass: inetLocalMailRecipient
        uidNumber: 9646
        gidNumber: 1100
        cn: Wilma
        sn: Flintstone
        homeDirectory: /home/wilma
        loginShell: /sbin/nologin
        gecos: Wilma Flintstone
        mail: wilma@gladserv.com
        */
        struct db_t *db = malloc(sizeof (db_t));
        db->alias = "ldapdb";
        db->type = "ldap";
        db->host = "ldap://ldaptestserver";
        db->db = "dc=gladserv,dc=com";
        db->user = "cn=testadmin,dc=gladserv,dc=com";
        db->pass = "mytestpass";
        db->conn = NULL;
        db->next = NULL;
int rc;
        keyval_t *kv;
        keyval_t attr12 = { "mail", "wilma@gladserv.com", NULL };
        keyval_t attr11 = { "gecos", "Wilma", &attr12 };
        keyval_t attr10 = { "loginShell", "/sbin/nologin", &attr11 };
        keyval_t attr9 = { "homeDirectory", "/home/wilma", &attr10 };
        keyval_t attr8 = { "sn", "Flintstone", &attr9 };
        keyval_t attr7 = { "cn", "Wilma", &attr8 };
        keyval_t attr6 = { "gidNumber", "1100", &attr7 };
        keyval_t attr5 = { "uidNumber", "9646", &attr6 };
        keyval_t attr4 = { "objectClass", "inetLocalMailRecipient", &attr5 };
        keyval_t attr3 = { "objectClass", "shadowAccount", &attr4 };
        keyval_t attr2 = { "objectClass", "posixAccount", &attr3 };
        keyval_t attr1 = { "objectClass", "inetOrgPerson", &attr2 };
        keyval_t attr0 = { "uid", "wilma", &attr1 };
        kv = &attr0;
        rc = db_insert_ldap(db, "uid=wilma,ou=people,dc=gladserv,dc=com", kv);
        free(db);
        return rc;
}

/* test keyval to LDAPMod conversion function */
int ldap_test_keyval_to_LDAPMod()
{
        int c = 0;
        int rc = 0;
        LDAPMod **lm = NULL;

        /* keyval to feed into keyval_to_LDAPMod() */
        keyval_t *kv;
        keyval_t attr3 = { "objectClass", "widgets", NULL };
        keyval_t attr2 = { "objectClass", "people", &attr3 };
        keyval_t attr1 = { "objectClass", "top", &attr2 };
        keyval_t attr0 = { "uid", "ldapadduser", &attr1 };
        kv = &attr0;

        /* the LDAPMod we expect back */
        LDAPMod *l0[4];
        char *vals_uid[] = { "ldapadduser", NULL };
        char *vals_objectClass[] = { "top", "people", "widgets", NULL };
        LDAPMod lm_uid, lm_objectClass;
        lm_uid.mod_type = "uid";
        lm_uid.mod_values = vals_uid;
        lm_objectClass.mod_type = "objectClass";
        lm_objectClass.mod_values = vals_objectClass;
        l0[0] = &lm_uid;
        l0[1] = &lm_objectClass;
        l0[2] = NULL;

        /* call the conversion function */
        c = keyval_to_LDAPMod(kv, &lm);

        /* check return value */
        if (c != 2) return 1;

        /* compare LDAPMods */
        if (lm == NULL) {
                rc = 1;
                goto ldap_test_keyval_to_LDAPMod_cleanup;
        }
        rc = compare_LDAPMod(l0, lm);

ldap_test_keyval_to_LDAPMod_cleanup:
        ldap_mods_free(lm, 1);

        return rc;
}
