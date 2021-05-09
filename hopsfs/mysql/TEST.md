# 

## Supply your own create and query SQL statements, with 50 clients querying and 200 selects for each (enter the command on a
       single line):

           mysqlslap --delimiter=";"
             --create="CREATE TABLE a (b int);INSERT INTO a VALUES (23)"
             --query="SELECT * FROM a" --concurrency=50 --iterations=200


```bash
mysqlslap -h 11.160.43.168 -p 3306 -u root -pLaLa123! --auto-generate-sql --auto-generate-sql-execute-number=1000 --auto-generate-sql-load-type=write


load type: mixed, update, write, key, or read; default is mixed.
```

+--------------+-----------+------+-------+---------+----------+-------------------+-------------+------------+-------------+----------------+------------------+--------+---------+--------------------+------+---------------+--------------+--------+--------------------+----------------+-------------------+--------------+----------------+--------------+----------+-----------------+----------------+
| partition_id | parent_id | name | id    | user_id | group_id | modification_time | access_time | permission | client_name | client_machine | generation_stamp | header | symlink | subtree_lock_owner | size | quota_enabled | meta_enabled | is_dir | under_construction | subtree_locked | file_stored_in_db | logical_time | storage_policy | children_num | num_aces | num_user_xattrs | num_sys_xattrs |
+--------------+-----------+------+-------+---------+----------+-------------------+-------------+------------+-------------+----------------+------------------+--------+---------+--------------------+------+---------------+--------------+--------+--------------------+----------------+-------------------+--------------+----------------+--------------+----------+-----------------+----------------+
|    102736839 |         1 | lala | 25002 |      31 |       11 |     1611293551249 |           0 |        493 | NULL        | NULL           |                0 |      0 | NULL    |                  0 |    0 |             0 |            0 |      1 |                  0 |              0 |                 0 |            0 |                |            0 |        0 |               0 |              0 |
|            0 |         0 |      |     1 |      31 |       11 |     1611294214972 |           0 |        493 | NULL        | NULL           |                0 |      0 | NULL    |                  0 |    0 |             1 |            0 |      1 |                  0 |              0 |                 0 |            0 |                |            1 |        0 |               0 |              0 |
+--------------+-----------+------+-------+---------+----------+-------------------+-------------+------------+-------------+----------------+------------------+--------+---------+--------------------+------+---------------+--------------+--------+--------------------+----------------+-------------------+--------------+----------------+--------------+----------+-----------------+----------------+

```bash
mysqlslap -h 11.160.43.168 -p 3306 -u root -pLaLa123! --concurrency=1 --iterations=1 --number-int-cols=23 --number-char-cols=4 --auto-generate-sql --auto-generate-sql-execute-number=1000 --auto-generate-sql-load-type=write

mysqlslap: [Warning] Using a password on the command line interface can be insecure.
Benchmark
	Average number of seconds to run all queries: 8.753 seconds
	Minimum number of seconds to run all queries: 8.753 seconds
	Maximum number of seconds to run all queries: 8.753 seconds
	Number of clients running queries: 1
	Average number of queries per client: 1000
```



