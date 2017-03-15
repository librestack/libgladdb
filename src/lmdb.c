/* 
 * lmdb.c - code to handle lmdb connections
 *
 * this file is part of GLADDB
 *
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
#define _GNU_SOURCE
#include "db.h"
#include "lmdb.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

/*  These 3 defines are taken from mtest.c in libmdb and are under 
 *  OpenLDAP Public License <http://www.OpenLDAP.org/license.html> 
 *  However, I've changed them to syslog.
 *
 *  https://github.com/LMDB/lmdb/tree/mdb.master/libraries/liblmdb */
#define E(expr) CHECK((rc = (expr)) == MDB_SUCCESS, #expr)
#define RES(err, expr) ((rc = expr) == (err) || (CHECK(!rc, #expr), 0))
#define CHECK(test, msg) ((test) ? (void)0 : ((void)syslog(LOG_DEBUG, \
       "%s:%d: %s: %s\n", __FILE__, __LINE__, msg, mdb_strerror(rc)), abort()))


/* connect to lmdb */
int db_connect_lmdb(db_t *db)
{

        int rc;
        MDB_env *env = NULL;

        /*  Create an LMDB environment handle.  */
        E(mdb_env_create(&env));
        syslog(LOG_DEBUG, "mdb_env_create() successful in: %s\n", __func__);

        /*  Set the maximum number of threads/reader slots for the environment. 
         *  Default is 126, but set to make sure */
        E(mdb_env_set_maxreaders(env, 126));
        syslog(LOG_DEBUG, "mdb_env_set_maxreaders() successful in: %s\n", __func__);

        /*  We should set sane defaults as the default db size is small, and allow 
         *  args for them too. We will re-grow anyway.
         *  We set the same as what the default is to be sure. */
        E(mdb_env_set_mapsize(env, 10485760)); 
        syslog(LOG_DEBUG, "mdb_env_set_mapsize() successful in: %s\n", __func__);

        /*  MDB_NOSUBDIR is used so db->db is the *actual* name of the db
         *  not the db directory to save the db file in. */
        E(mdb_env_open(env, db->db, MDB_NOSUBDIR, 0664));
        syslog(LOG_DEBUG, "mdb_env_open() successful in: %s\n", __func__);

        /*  Save the handle */
        db->conn = env;

        return EXIT_SUCCESS;
}

/* disconnect from a lmdb db */
int db_disconnect_lmdb(db_t *db)
{
        if (db->conn != NULL) {
		mdb_env_close(db->conn); // void return type
                db->conn = NULL;
	}
	else {
                syslog(LOG_ERR, "mdb_env_close(): Attempt to use a null pointer to close db: %s\n",
                __func__);
	}
        return EXIT_SUCCESS;
}

int db_fetch_all_lmdb(db_t *db, char *query, field_t *filter, row_t **rows,
        int *rowc)
{
        return EXIT_SUCCESS;
}

/* lmdb get */
int db_get_lmdb(db_t *db, char *resource, keyval_t *db_data)
{
        int rc;
        MDB_val key, data;

        MDB_env *env = db->conn;

        MDB_txn *txn = NULL;
        MDB_dbi dbi = NULL;

        /*  Convert our key. */
        key.mv_size = sizeof(db_data->key);
        key.mv_data = db_data->key;

        /*  Make sure readonly */
        E(mdb_txn_begin(env, NULL, MDB_RDONLY, &txn));
        E(mdb_dbi_open(txn, NULL, 0, &dbi));

        E(mdb_get(txn, dbi, &key, &data));
        mdb_txn_abort(txn);

        /*  all good, save data. */
        db_data->value = data.mv_data;

        return EXIT_SUCCESS;
}

/* lmdb put */
int db_insert_lmdb(db_t *db, char *resource, keyval_t *db_data)
{
       /*  We don't use *resource yet, we can support multiple databases or nested transactions,
        *  so that's something to consider later. */
       
        /*  Maintain our stats */
        MDB_stat mst;

        /*  We need to convert/set keyval_t to MDB_val */
        MDB_val key, data;

        key.mv_size = sizeof(db_data->key);
        key.mv_data = db_data->key;

        data.mv_size = sizeof(db_data->value);
        data.mv_data = db_data->value;
        
        /*  Get ready */
        int rc;
        MDB_txn *txn = NULL;
        MDB_dbi dbi;
        MDB_env *env = db->conn;

        /*  Once the environment is open via db_connect_lmdb(), a transaction can be created within it 
         *  using mdb_txn_begin(). Transactions may be read-write or read-only, and read-write 
         *  transactions may be nested. We always read-write here.
         *
         *  A transaction must only be used by one thread at a time. Transactions are always required,
         *  even for read-only access. The transaction provides a consistent view of the
         *  data. */
        E(mdb_txn_begin(db->conn, NULL, 0, &txn)); /* We don't support nested transactions yet. */
        E(mdb_dbi_open(txn, NULL, 0, &dbi));

        /*  We don't do anything complicated yet or allow storing multiple values for a key.
         *  We could use *resource as a flag to indicate that and switch to mdb_cursor_put(). 
         *  We also don't use and of the flexible flags. See mdb_put() docs. */
        E(mdb_put(txn, dbi, &key, &data, 0));
        E(mdb_txn_commit(txn));
        E(mdb_env_stat(env, &mst));

        return EXIT_SUCCESS;
}

/* lmdb delete */
int db_delete_lmdb(db_t *db, char *resource, keyval_t *db_data)
{
        /*  Maintain our stats */
        MDB_stat mst;

        int rc;
        MDB_txn *txn = NULL;
        MDB_val key;
        MDB_dbi dbi = NULL;
        MDB_env *env = db->conn;

        /*  Convert */        
        key.mv_size = sizeof(db_data->key);
        key.mv_data = db_data->key;

        E(mdb_txn_begin(db->conn, NULL, 0, &txn)); /* We don't support nested transactions yet. */
        E(mdb_dbi_open(txn, NULL, 0, &dbi));

        /*  Again, basic delete, no flags. As with db_insert_mdb() we could use
         *  *resource and switch to mdb_cursor_del() */
        if (RES(MDB_NOTFOUND, mdb_del(txn, dbi, &key, NULL))) {
               mdb_txn_abort(txn);      
        } 
        else {
               E(mdb_txn_commit(txn));
        }
        E(mdb_env_stat(env, &mst));

        return EXIT_SUCCESS;
}
