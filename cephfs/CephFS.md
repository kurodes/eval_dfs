# Deploy and Evaluate CephFS
ceph version 12.2.13 (584a20eb0237c657dc0567da126be145106aa47e) luminous (stable)

## install from yum repo
``` bash
wget -O /etc/yum.repos.d/CentOS-Base.repo http://mirrors.aliyun.com/repo/
wget -O /etc/yum.repos.d/epel.repo http://mirrors.aliyun.com/repo/epel-7.repo
http://mirrors.aliyun.com/ceph/rpm-luminous/el7/
yum install ceph
```

## init ceph cluster
### ceph.conf
- diable cephx
``` ini
[global]
auth client required = none
auth cluster required = none
auth service required = none
auth supported = none

mon allow pool delete = True

osd pool default crush rule = -1
osd pool default min size = 1
osd pool default size = 1

fsid = b6f9e985-bb57-4404-951b-733bafef27f4
cluster network = 11.160.44.0/24,11.160.43.0/24
public network = 11.160.44.0/24,11.160.43.0/24
mon host = 11.160.43.130
mon initial members = i32d05173.sqa.eu95
```

### mons
```bash
sudo monmaptool --create --add `hostname` 11.160.43.130 --fsid b6f9e985-bb57-4404-951b-733bafef27f4 /tmp/monmap;
sudo rm -rf /var/lib/ceph;
sudo mkdir -p /var/lib/ceph/mon/ceph-`hostname`;
sudo ceph-mon --mkfs -i `hostname` --monmap /tmp/monmap;
sudo chown -R ceph:ceph /var/lib/ceph/mon;
sudo systemctl start ceph-mon@`hostname`;

sudo systemctl status ceph-mon@`hostname`;
sudo systemctl stop ceph-mon@`hostname`;
```

### mgrs
```bash
sudo mkdir -p /var/lib/ceph/mgr/ceph-`hostname`;
sudo chown -R ceph:ceph /var/lib/ceph/mgr;
sudo systemctl start ceph-mgr@$`hostname`;
```

### osds
```bash
sudo mkdir -p /var/lib/ceph/osd
sudo chown -R ceph:ceph /var/lib/ceph/osd;
sudo ceph-volume lvm create --data /dev/ram0;
```

- remove osd
```bash
# remove membership
ceph osd out all;
ceph osd down all;
pssh -h nodes-osds -i 'sudo systemctl stop ceph-osd@*'
ceph osd purge * --yes-i-really-mean-it;
ceph osd rm all;
ceph osd crush rm osd.5;
ceph auth del osd.5
    - ceph auth list
# remove data
pssh -h nodes-osds -i 'ceph-volume lvm zap --destroy /dev/ram0;'
```
- If ceph-volume failed:
```bash
pssh -h nodes-tmp -i "sudo dd if=/dev/zero of=/dev/ram0 bs=512 count=1 conv=notrunc"
ceph osd crush rm osd.${i}
sudo ceph auth del osd.${i}
    - ceph auth list
```

### mdss
```bash
sudo mkdir -p /var/lib/ceph/mds/ceph-`hostname`;
sudo chown -R ceph:ceph /var/lib/ceph/mds;
pssh -h nodes-servers -i 'sudo systemctl start ceph-mds@`hostname`;'
ceph mds stat
```
> CAUTIOUS: use single quote ' for keep `hostname` unexpanded

### ceph dashboard
```bash
ceph mgr module ls
ceph mgr module disable dashboard
ceph config-key set mgr/dashboard/server_addr 0.0.0.0
ceph config-key set mgr/dashboard/server_port 7000
ceph mgr module enable dashboard
systemctl restart ceph-mgr@`hostname`
```

### cephfs
> num of pg -> 64 osd
> 
> cephfs_metadata 128; ref: https://docs.ceph.com/en/latest/cephfs/createfs/
> 
> cephfs_data 2048; ref: https://docs.ceph.com/en/latest/rados/operations/placement-groups/#choosing-number-of-placement-groups, https://ceph.com/pgcalc/
```bash
ceph osd pool create cephfs_metadata 128
ceph osd pool create cephfs_data 2048
ceph fs new cephfs cephfs_metadata cephfs_data
ceph fs set cephfs max_mds 64
```

- remove cephfs
```bash
pssh -h nodes-osds -i 'sudo systemctl stop ceph-mds@`hostname`;'
ceph mds fail 37
ceph fs rm cephfs --yes-i-really-mean-it
rados purge <pool-name> --yes-i-really-really-mean-it
ceph osd pool delete cephfs_metadata cephfs_metadata --yes-i-really-really-mean-it
ceph osd pool delete cephfs_data cephfs_data --yes-i-really-really-mean-it
```

### mount cephfs using kernel driver
> https://ceph.io/geen-categorie/feature-set-mismatch-error-on-ceph-kernel-client/
```bash
mount -t ceph 11.160.43.130:6789:/ /mnt/cephfs
umount /mnt/cephfs
ceph osd crush tunables legacy
ceph osd crush tunables optimal
```

### manually pinning directory trees to a particular rank
> https://docs.ceph.com/en/mimic/cephfs/multimds/?#manually-pinning-directory-trees-to-a-particular-rank
```bash
setfattr -n ceph.dir.pin -v 1 lala/
```

### test with ior ( 2711ae5 ) mdtest
- 编译 ior for cephfs
  - checkout 2711ae5
  - replace aiori-CEPHFS.c
```bash
# libcephfs
mpirun --mca btl ^openib --mca btl_tcp_if_include bond0 -hostfile /home/kuro/cephfs/nodes-clients -np 64 \
/home/kuro/ior-ceph/src/mdtest -a=cephfs --cephfs.user=kuro --cephfs.conf=/etc/ceph/ceph.conf -z 2 -b 2 -I 2 -d /-u

# ceph kernel driver
mpirun --mca btl ^openib --mca btl_tcp_if_include bond0 -hostfile nodes-clients -np 48 \
/home/kuro/ior-master/src/mdtest -z 2 -b 2 -I 100 -d /mnt/cephfs
```



