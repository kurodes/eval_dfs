#!/usr/bin/env python2
from __future__ import print_function
import sys
import os


def prRed(skk):
    print("\033[91m %s \033[00m" % skk)
    sys.stdout.flush()


def prGreen(skk):
    print("\033[92m %s \033[00m" % skk)
    sys.stdout.flush()


def run_cmd(cmd):
    prGreen(cmd)
    os.system(cmd)
    return


if __name__ == "__main__":
    phase_list = [
        '-C',
        '-T',
        # '-T -R',
        '-E',
        '-r'
    ]
    for phase in phase_list:
        prRed("Phase: %s" % (phase))
#         run_cmd(
#             "mpirun -np 1 --oversubscribe --mca mpi_yield_when_idle 1 --mca btl ^openib --mca btl_tcp_if_include bond0 --mca plm_rsh_no_tree_spawn 1 \
# mdtest -z 10 -b 3 -I 10 -d /mnt/cephfs %s" % (phase)
#         )
        run_cmd(
            "/home/heifeng.lwh/ior-3.3.0-latency/src/mdtest -z 10 -b 3 -I 10 -d /mnt/cephfs %s" % (
                phase)
        )
        # run_cmd(
        #     "mkdir -p /home/heifeng.lwh/res/cephfs/%s"
        # )
        run_cmd(
            "sudo umount /mnt/cephfs; sleep 60;"
        )
        run_cmd(
            "sudo mount -t ceph 11.160.43.130:6789:/ /mnt/cephfs; sleep 60;"
        )
