#!/usr/bin/env python2
from __future__ import print_function
import sys
import os
import time


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


if __name__ == "__main__":
    server_num = 64
    client_num_list = [96, 128, 160, 192]
    # server_num = 32
    # client_num_list = [64, 96, 128, 160, 192]
    # server_num = 16
    # client_num_list = [32, 48, 64, 80, 96, 112, 128, 144]
    # server_num = 8
    # client_num_list = [16, 24, 32, 40, 48, 56]
    # server_num = 4
    # client_num_list = [16, 24, 32, 40, 48]
    # server_num = 2
    # client_num_list = [16, 24, 32]
    # server_num = 1
    # client_num_list = [8, 16, 24, 32]
    for client_num in client_num_list:
        prCyan('TEST_Servers_Clients:(%d,%d)' % (server_num, client_num))

        # run_cmd(
        #     'mpirun --mca btl ^openib --mca btl_tcp_if_include bond0 -hostfile /home/heifeng.lwh/cephfs/nodes-clients-hostfile -np %d /home/heifeng.lwh/ior-ceph/src/mdtest -a=cephfs --cephfs.user=heifeng.lwh --cephfs.conf=/etc/ceph/ceph.conf -z 10 -b 2 -I 50 -d / -u'
        #     % (client_num)
        # )
        run_cmd(
            'mpirun --mca btl ^openib --mca btl_tcp_if_include bond0 -hostfile /home/heifeng.lwh/cephfs/64-nodes-clients-hostfile -np %d /home/heifeng.lwh/ior-ceph/src/mdtest -a=cephfs --cephfs.user=heifeng.lwh --cephfs.conf=/etc/ceph/ceph.conf -z 10 -b 2 -I 50 -d / -u'
            % (client_num)
        )

        time.sleep(60)
