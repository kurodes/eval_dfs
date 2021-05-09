#!/usr/bin/env python2
from __future__ import print_function
import sys
import os
import time

global_server_list = ['11.160.43.130', '11.160.43.141', '11.160.43.144', '11.160.43.146', '11.160.43.148', '11.160.43.160', '11.160.43.163', '11.160.43.167', '11.160.43.170', '11.160.43.171', '11.160.43.172', '11.160.43.174', '11.160.43.66', '11.160.43.77', '11.160.43.81',
                      '11.160.43.82', '11.160.43.83', '11.160.43.84', '11.160.43.85', '11.160.43.86', '11.160.43.87', '11.160.43.90', '11.160.43.92', '11.160.43.94', '11.160.43.95', '11.160.43.96', '11.160.43.98', '11.160.43.100', '11.160.43.101', '11.160.43.102', '11.160.43.103', '11.160.43.105']


def prRed(skk):
    print("\033[91m %s \033[00m" % skk)
    sys.stdout.flush()


def prGreen(skk):
    print("\033[92m %s \033[00m" % skk)
    sys.stdout.flush()


def prCyan(skk):
    print("\033[96m %s \033[00m" % skk)
    sys.stdout.flush()


def run_cmd(cmd):
    prGreen(cmd)
    os.system(cmd)
    return


def generate_pssh_hoststr(hostlist):
    hoststr = '\"'
    for i in hostlist:
        hoststr = hoststr + i + ' '
    hoststr = hoststr.strip(' ')
    hoststr = hoststr + '\"'
    return hoststr


def restart_hopsfs(server_num):
    server_list = global_server_list[0:server_num]

    run_cmd('pssh -H %s -i \'cd /home/heifeng.lwh/hopsfs/hadoop-3.2.0.0; ./stop_nn_dn.sh\'' %
            (generate_pssh_hoststr(global_server_list)))
    time.sleep(10)
    run_cmd('pssh -h /home/heifeng.lwh/hopsfs/mysql/nodes_mgmd -i \'ndb_mgm -e show\'')
    run_cmd(
        'mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e \'drop database hopsfs\';'
        'mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e \'show databases\';'
        'mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e \'create database hopsfs\';'
        'mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e \'show databases\';'
        'mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e \"set global max_connections=50000\";'
        'mysql --host=11.160.43.168 --port=3306 -u root -pLaLa123! -e \"show variables like \'max_connections\'\";'
        'cd /home/heifeng.lwh/hopsfs/hadoop-3.2.0.0/schema; ./create-tables-diskless.sh 11.160.43.168 3306 root LaLa123! hopsfs'
    )
    time.sleep(2)
    run_cmd('/home/heifeng.lwh/hopsfs/hadoop-3.2.0.0/bin/hdfs namenode -format')
    time.sleep(2)
    run_cmd('pssh -p 8 -H %s -i \'cd /home/heifeng.lwh/hopsfs/hadoop-3.2.0.0; ./start_nn_dn.sh\'' %
            (generate_pssh_hoststr(server_list)))
    time.sleep(10)
    run_cmd('pssh -h /home/heifeng.lwh/hopsfs/mysql/nodes_mgmd -i \'ndb_mgm -e show\'')
    time.sleep(2)


op_list = [
    '-D -C',
    # '-D -T',
    '-D -r',
    # '-F -C',
    # '-F -T',
    # '-F -r'
]

mpiargs = str(
    '--mca mpi_yield_when_idle 1 --mca btl ^openib --mca btl_tcp_if_include bond0 --mca plm_rsh_no_tree_spawn 1')
hostfile = '/home/heifeng.lwh/hopsfs/nodes_clients_hostfile'
mdtest = '/home/heifeng.lwh/ior-master-hdfs-lat/src/mdtest'

# python test_hopsfs_iops.py test_iops 1>iops.log 2>iops.err.log
# python test_hopsfs_iops.py test_iops > >(tee -a iops.err.log) 2> >(tee -a iops.log >&2)
if __name__ == "__main__":

    if len(sys.argv) <= 1:
        print('./script func')
        sys.stdout.flush()
        exit(0)

    if sys.argv[1] == 'restart_hopsfs':
        server_num = 32
        restart_hopsfs(server_num)
        exit(0)

    if sys.argv[1] == 'test_lat':
        server_num_list = [32, 16, 8, 4, 2, 1]
        # run
        for server_num_index in range(0, len(server_num_list)):
            server_num = server_num_list[server_num_index]
            client_num = 1
            restart_hopsfs(server_num)

            prCyan('TEST_Servers_Clients:(%d,%d)' % (server_num, client_num))

            # 1555*20 = 31100
            # structure = '-z 4 -b 6 -I 20 -u'
            # 3906*10 = 39060
            # structure = '-z 5 -b 5 -I 10 -u'
            # 3906*5 = 19530
            # structure = '-z 5 -b 5 -I 5 -u'
            # 3906*3 = 11718
            # structure = '-z 5 -b 5 -I 3 -u'

            structure = '-z 10 -b 3 -I 1 -u'
            run_cmd('%s -a=hdfs %s -d /' % (mdtest, structure))
            run_cmd(
                'cp -r /home/heifeng.lwh/latency /home/heifeng.lwh/latency-%d' % server_num)
            run_cmd('rm -f /home/heifeng.lwh/latency/*')
        exit(0)

    if sys.argv[1] == 'test_rmdir':
        server_num_list = [32]
        # run
        for server_num_index in range(0, len(server_num_list)):
            server_num = server_num_list[server_num_index]
            client_num = 1
            restart_hopsfs(server_num)
            prCyan('TEST_Servers_Clients:(%d,%d)' % (server_num, client_num))
            structure = '-z 10 -b 3 -I 1 -u'

            mdtest_rmdir = '/home/heifeng.lwh/ior-master-hdfs-lat-rmdir/src/mdtest'
            run_cmd('%s -a=hdfs %s -d / -D -C -r' % (mdtest_rmdir, structure))

            run_cmd(
                'cp -r /home/heifeng.lwh/latency /home/heifeng.lwh/latency-%d' % server_num)
            run_cmd('rm -f /home/heifeng.lwh/latency/*')
        exit(0)
