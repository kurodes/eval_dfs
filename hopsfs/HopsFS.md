# HopsFS Deployment

## install
- Manually Install 
  - deps
    - install java JDK, JRE; set JAVA_HOME to location of JRE
      - sudo yum install java-1.8.0-openjdk-devel-1.8.0.275.b01
        - 同时会安装 sudo yum install java-1.8.0-openjdk-1.8.0.275.b01
      - export JAVA_HOME=/usr/lib/jvm/java-1.8.0-openjdk-1.8.0.275xxxxx (to bashrc)
    - NDB client library
    - protobuffer 2.5
      - wget https://github.com/google/protobuf/releases/download/v2.5.0/protobuf-2.5.0.tar.gz 
      - tar -xzvf protobuf-2.5.0.tar.gz
      - ./configure; make -j; sudo make install
      - export LD_LIBRARY_PATH=/usr/local/lib 
    - install maven
      - mkdir ~/deps
      - cd ~/deps; wget https://mirrors.tuna.tsinghua.edu.cn/apache/maven/maven-3/3.6.3/binaries/apache-maven-3.6.3-bin.tar.gz
      - cd ~/deps; tar -xzvf apache-maven-3.6.3-bin.tar.gz
      - add .../apache-maven-3.6.3/bin/mvn/bin to PATH 
      - maven ali mirror
        -  Maven国内阿里镜像（Maven下载慢的解决方法） - D调的华丽呢 - 博客园 
  - hops DAL
    - git clone https://github.com/hopshadoop/hops-metadata-dal
    - cd hops-metadata-dal; mvn clean install -DskipTests
  - hops DAL impl ndb
    - git clone https://github.com/hopshadoop/hops-metadata-dal-impl-ndb
    - cd hops-metadata-dal-impl-ndb/; mvn clean install -DskipTests
    - libndbclient.so
      - wget https://archiva.hops.works/repository/Hops/com/mysql/ndb/clusterj-native/7.6.10/clusterj-native-7.6.10-natives-linux.jar
      - unzip clusterj-native-7.6.10-natives-linux.jar
      - sudo cp libndbclient.so /usr/lib
  - Building Hops Hadoop
    - git clone https://github.com/hopshadoop/hops
    - git checkout master
      - mvn package generate-sources -Pdist,native -DskipTests -Dtar
        - cmake
        - zlib-devel
        - openssl-devel
        - Maven is not working in Java 8 when Javadoc tags are incomplete - Stack Overflow 
        - unable to build maven project due to javadoc error? - Stack Overflow 
      - mvn clean generate-sources install -Pndb -DskipTests
      - mvn clean test-compile
  - 


  - MySQL NDB
    - install
      - standard server install
        - sudo yum localinstall mysql-cluster-community-{server,client,common,libs}-*
      - client install 
        - sudo yum localinstall mysql-community-{client,common,libs}-*
      - start mysql
        - systemctl start mysqld
      - configuration file
    - 
    - Install Binary
      - ref: MySQL :: MySQL 8.0 Reference Manual :: 23.2.1.2 Installing NDB Cluster from RPM 
      - download RPM Bundle from MySQL :: Download MySQL Cluster 
      - sudo yum install https://dl.fedoraproject.org/pub/epel/epel-release-latest-7.noarch.rpm -y
        -  MySQL Bugs: #83476: SQL Node installation fails with Missing Perl Classes 
      - Management nodes require the management server daemon (ndb_mgmd);
        - !!! management node shall not on the same server as data nodes !!!
        - sudo yum install mysql-cluster-community-{management-server,client,common,libs}-* -y
      - Data nodes require the data node daemon (ndbd or ndbmtd).
        - sudo yum install mysql-cluster-community-data-node-* -y
      - A SQL node must have installed on it a MySQL Server binary (mysqld). 
        - sudo yum localinstall mysql-cluster-community-{server,client,common,libs}-* -y

## mysql

### init Mysql NDB cluster
> https://dev.mysql.com/doc/refman/8.0/en/mysql-cluster-install-configuration.html

1. Management Node
- initial configuration: 
    - `/var/lib/mysql-cluster/config.ini`
    - diskless
        - It is possible to specify NDB Cluster tables as diskless, meaning that tables are not checkpointed to disk and that no logging occurs.
> https://github.com/hopshadoop/hops-metadata-dal-impl-ndb/blob/master/NDB/sample-ndb-config/config.ini
>
> https://dev.mysql.com/doc/refman/5.7/en/mysql-cluster-ndbd-definition.html#ndbparam-ndbd-diskless

- start ndb manager
    - `sudo ndb_mgmd -f /var/lib/mysql-cluster/config.ini --ndb-nodeid=1 --initial`

2. Data Node
- 数据节点配置文件 `/etc/my.cnf`
```ini
[mysqld]
# Options for mysqld process:
# run NDB storage engine
ndbcluster

[mysql_cluster]
# Options for NDB Cluster processes:
# location of management server
ndb-connectstring=11.160.43.143
```
- 启动多线程NDB
    - sudo ndbd / ndbmtd
- 使用ramdisk，作为ndb的数据目录
```bash
sudo mkdir -p /usr/local/mysql/data
mkfs.ext4 /dev/ram1
mount /dev/ram1 /usr/local/mysql/data
```
3. Mysql Server
> https://dev.mysql.com/doc/refman/8.0/en/linux-installation-rpm.html
- 同数据节点一样，配置相同的`/etc/my.cof`
- `systemctl start mysqld`
- 获取默认密码，并设置新密码
    - sudo grep 'temporary password' /var/log/mysqld.log
    - mysql -uroot -p...
    - ALTER USER 'root'@'localhost' IDENTIFIED BY 'LaLa123!';
    - 重制密码: https://www.cnblogs.com/zgqbky/p/11720406.html
- 创建 database
    - CREATE DATABASE hopsfs;
    - SHOW DATABASES;
    - USE hopsfs;
    - SHOW TABLES;
    - DESCRIBE my_table;
    - mysql -u root -pLaLa123! -D hopsfs -e "DROP database hopsfs" 
    - mysql -u root -pLaLa123! -D hopsfs -e "CREATE DATABASE hopsfs" 
    - select * from hdfs_metadata_log;
- 设置远程访问root账户权限
    - use mysql; select host,user,password_last_changed from user;
    - update user set host='%' where user ='root';
    - flush privileges;

4. 重启 ndb 集群

> https://blog.csdn.net/cuantangnie3915/article/details/100491082?utm_medium=distribute.pc_relevant.none-task-blog-baidujs_title-2&spm=1001.2101.3001.4242
>
> https://dev.mysql.com/doc/refman/5.7/en/mysql-cluster-rolling-restart.html
>
> https://dev.mysql.com/doc/refman/5.7/en/mysql-cluster-ndbinfo-nodes.html

- Rolling Restart of NDB Cluster
    - ndb_mgm -e shutdown
    - update config.ini
    - sudo ndb_mgmd -f /var/lib/mysql-cluster/config.ini --ndb-nodeid= --reload
    - sudo ndbd / ndbmtd
    - systemctl start mysqld

### 启动 HopsFS NameNodes
- 检查端口占用
    - `sudo lsof -Pi :50070 -sTCP:LISTEN -t`
- 在数据库创建需要的 table
    - `/hdfs namenode -dropAndCreateDB`
    - `hops-metadata-dal-impl-ndb/schema/create-tables.sh`
- 设置配置信息
    - etc/hadoop/hadoop-env.sh
    - etc/hadoop/core-site.xml
    - etc/hadoop/hdfs-site.xml
    - etc/hadoop/ndb-config.properties
- 设置环境变量
```bash
export HADOOP_HOME=/home/heifeng.lwh/hopsfs/hadoop-3.2.0.0
export JAVA_HOME=$HADOOP_HOME/java-1.8.0-openjdk-1.8.0.275.b01-0.el7_9.x86_64
export JRE_HOME=$JAVA_HOME/jre
export CLASSPATH=$JAVA_HOME/lib:$JRE_HOME/lib:/usr/share/mysql/java/:$CLASSPATH
export PATH=$JAVA_HOME/bin:$JRE_HOME/bin:$PATH
```
- format
    - `$HADOOP_HOME/bin/hdfs namenode -format`
- 启动 namenode
    - `$HADOOP_HOME/bin/hdfs --daemon start namenode`
- stop
    - `$HADOOP_HOME/bin/hdfs --daemon stop namenode`

- ISSUES:
  - 拷贝 jdk 会出现 链接文件 的问题！
    - https://www.cnblogs.com/xiashiwendao/p/8505216.html


## hopsfs namenode

## libhdfs
> https://hadoop.apache.org/docs/r1.2.1/libhdfs.html
```bash
g++ -std=c++11 test_path_resolution.cpp -I${HADOOP_HOME}/hdfs-src/libhdfs/ -lhdfs -o test_path_resolution
```

## mdtest
```bash
/home/heifeng.lwh/ior-master-latency/src/mdtest -a=hdfs -z 10 -b 3 -I 1 -d /
```

# 编译
- 参考 github README.md

- 修改配置
  - etc/hadoop/hadoop-env.sh
    - export HADOOP_CLASSPATH="/home/heifeng.lwh/hopsfs/hadoop-3.2.0.0/share/hadoop/*"
  - etc/hadoop/core-site.xml
  - etc/hadoop/hdfs-site.xml
  - etc/hadoop/ndb-config.properties
- 设置日志等级为WARN
  - etc/log4j.properties
    - hadoop.root.logger=WARN,console
  - etc/hadoop-env.sh
    - export HADOOP_ROOT_LOGGER=WARN,console


# commands
```bash
# 监测端口可用
sudo lsof -Pi :50070 -sTCP:LISTEN -t

# mysql 资源监控
# ssh 11.160.43.143 'ndb_mgm -e "all report mem"'
watch -n 1 -d "ssh 11.160.43.143 'ndb_mgm -e \"all report mem\"'"
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'show processlist'
mysql --host=11.160.43.130 --port=3306 -u root -pLaLa123! -e 'show processlist'
# show status 启动时间，查询次数，
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'show status like "Uptime"'
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'show status like "Com_select"'
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'show status like "Com_insert"'
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'show status like "Com_update"'
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'show status like "Com_delete"'
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'show status like "Threads_connected"'
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'show status like "connections"'
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'Show status like "slow_queries"'
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'status'
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'select node_id, memory_type, (used/total)*100 as "Used Memory %" from ndbinfo.memoryusage;'
watch -d -n 1 "mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'select node_id, memory_type, (used/total)*100 as Used_Memory_pct from ndbinfo.memoryusage;'"


ssh 11.160.43.168
ndb_top --host=11.160.43.168 --port=3306 -u root -pLaLa123! -n 2


## 1. start ndb cluster
pssh -h /home/heifeng.lwh/hopsfs/mysql/nodes_mgmd -i 'ndb_mgm -e shutdown'
pssh -h /home/heifeng.lwh/hopsfs/mysql/nodes_mgmd -i 'sudo ndb_mgmd -f /var/lib/mysql-cluster/config.ini --ndb-nodeid=1 --initial'
# /usr/local/mysql/data
pssh -h /home/heifeng.lwh/hopsfs/mysql/nodes_ndbd -i 'rf -f /home/heifeng.lwh/tmpfs/ndbdata/*'
pssh -h /home/heifeng.lwh/hopsfs/mysql/nodes_ndbd -i 'sudo ndbmtd --initial'
pssh -h /home/heifeng.lwh/hopsfs/mysql/nodes_mgmd -i 'ndb_mgm -e show'
while (pssh -h /home/heifeng.lwh/hopsfs/mysql/nodes_mgmd -i 'ndb_mgm -e show' | grep starting); do
    sleep 5
    echo starting
done
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e "show variables like 'max_connections'"
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e "set global max_connections=65536"

## 2. start namenode
pssh -h /home/heifeng.lwh/hopsfs/nodes_nn -i 'cd /home/heifeng.lwh/hopsfs/hadoop-3.2.0.0; ./stop_nn_dn.sh' 
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'drop database hopsfs'; \
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'show databases'; \
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'create database hopsfs'; \
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e 'show databases'; \
mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -D hopsfs -e 'show tables'; \
cd /home/heifeng.lwh/hopsfs/hadoop-3.2.0.0/schema && ./create-tables-diskless.sh 11.160.43.168 3306 root LaLa123! hopsfs && cd -

/home/heifeng.lwh/hopsfs/hadoop-3.2.0.0/bin/hdfs namenode -format
pssh -h /home/heifeng.lwh/hopsfs/nodes_nn -i 'ls /home/heifeng.lwh/hopsfs/hadoop-3.2.0.0/logs/'
pssh -h /home/heifeng.lwh/hopsfs/nodes_nn -i 'rm -f /home/heifeng.lwh/hopsfs/hadoop-3.2.0.0/logs/*'
pssh -p 8 -h /home/heifeng.lwh/hopsfs/nodes_nn -i 'cd /home/heifeng.lwh/hopsfs/hadoop-3.2.0.0; ./start_nn_dn.sh'
pssh -h /home/heifeng.lwh/hopsfs/nodes_nn -i 'cd /home/heifeng.lwh/hopsfs/hadoop-3.2.0.0; ./stop_nn_dn.sh'

$HADOOP_HOME/bin/hdfs --debug --loglevel TRACE dfs -ls /


pssh -H 11.160.43.141 -i 'cd /home/heifeng.lwh/hopsfs/hadoop-3.2.0.0; ./start_nn_dn.sh'
```

# mdtest for hopsfs
## 修改 mdtest.c
- mdtest-no#testdir.c
  - / 和 /testdir，分别在一个shard上，/testdir 下所有元数据都要访问 /testdir，存在热点， 删除 /#test-dir.0/ 层
- mdtest-no#testdir-normdir.c
  - 注释所有 rmdir

```
mpirun --mca btl ^openib --mca btl_tcp_if_include bond0 -hostfile /home/heifeng.lwh/hopsfs/nodes_clients_hostfile -np 512 /home/heifeng.lwh/ior-master-hdfs-iops/src/mdtest -a=hdfs -z 5 -b 2 -I 10 -d / -u -C -T

mpirun --mca mpi_yield_when_idle 1 --mca btl ^openib --mca btl_tcp_if_include bond0 --mca plm_rsh_no_tree_spawn 1 \
-np 128 -hostfile /home/heifeng.lwh/hopsfs/nodes_clients_hostfile \
/home/heifeng.lwh/ior-master-hdfs-iops/src/mdtest -a=hdfs -z 4 -b 6 -I 10 -d / -u -C -T

-z 4 -b 6 -I 10

# 63*10
/home/heifeng.lwh/ior-master-hdfs-iops/src/mdtest -a=hdfs -z 5 -b 2 -I 10 -d / -p 10

/home/heifeng.lwh/ior-master-hdfs-iops/src/mdtest -a=hdfs -z 5 -b 5 -I 5 -u -d /

mpirun -np 32 -hostfile /home/heifeng.lwh/hopsfs/nodes_clients_hostfile /home/heifeng.lwh/ior-master-hdfs-iops/src/mdtest -a=hdfs -z1 -b1 -I1 -d/ -u -D -C

mpirun -np 32 -hostfile /home/heifeng.lwh/hopsfs/nodes_all_hostfile src/mdtest -a=hdfs -z1 -b1 -I1 -d/ -u



mpirun --mca mpi_yield_when_idle 1 --mca btl ^openib --mca btl_tcp_if_include bond0 --mca plm_rsh_no_tree_spawn 1 \
-np 88 -bynode -hostfile /home/heifeng.lwh/hopsfs/nodes_clients_hostfile \
/home/heifeng.lwh/ior-master-hdfs-iops/src/mdtest -a=hdfs -z 1 -b 1 -I 1 -u -d / 


```



































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
mysqlslap -h 11.160.43.168 -p 3306 -u root -pLaLa123! \
  --number-int-cols=23 --number-char-cols=4 \
  --auto-generate-sql --auto-generate-sql-guid-primary --auto-generate-sql-secondary-indexes=2 \
  --auto-generate-sql-load-type=write \
  --engine=ndbcluster \
  --concurrency=1 --auto-generate-sql-execute-number=1000 \
  --iterations=3 

mysqlslap -h 11.160.43.168 -p 3306 -u root -pLaLa123! \
  --number-int-cols=23 --number-char-cols=4 \
  --auto-generate-sql --auto-generate-sql-guid-primary --auto-generate-sql-secondary-indexes=2 \
  --auto-generate-sql-write-number=1000 \
  --auto-generate-sql-load-type=read \
  --engine=ndbcluster \
  --concurrency=32 --auto-generate-sql-execute-number=100 \
  --iterations=3 







$mysqlslap -h 11.160.43.168 -p 3306 -u root -pLaLa123! \
>   --number-int-cols=23 --number-char-cols=4 \
>   --auto-generate-sql --auto-generate-sql-guid-primary --auto-generate-sql-secondary-indexes=2 \
>   --auto-generate-sql-load-type=write \
>   --engine=ndbcluster \
>   --concurrency=1 --auto-generate-sql-execute-number=1000 \
>   --iterations=3 
mysqlslap: [Warning] Using a password on the command line interface can be insecure.
Benchmark
        Running for engine ndbcluster
        Average number of seconds to run all queries: 0.989 seconds
        Minimum number of seconds to run all queries: 0.918 seconds
        Maximum number of seconds to run all queries: 1.058 seconds
        Number of clients running queries: 1
        Average number of queries per client: 1000

$mysqlslap -h 11.160.43.168 -p 3306 -u root -pLaLa123! \
>   --number-int-cols=23 --number-char-cols=4 \
>   --auto-generate-sql --auto-generate-sql-guid-primary --auto-generate-sql-secondary-indexes=2 \
>   --auto-generate-sql-load-type=write \
>   --engine=ndbcluster \
>   --concurrency=64 --auto-generate-sql-execute-number=10000 \
>   --iterations=3 
mysqlslap: [Warning] Using a password on the command line interface can be insecure.
Benchmark
        Running for engine ndbcluster
        Average number of seconds to run all queries: 10.510 seconds
        Minimum number of seconds to run all queries: 10.431 seconds
        Maximum number of seconds to run all queries: 10.653 seconds
        Number of clients running queries: 64
        Average number of queries per client: 10000




./flexAsynch -ndbrecord -l 3 -t 32 -p 32 -o 1000000 -c 3 -a 30


```
