# Block device driver

# Execution

[root@ Bdevicedriver]# cat Makefile
obj-m:=blockvdisk.o

[root@ Bdevicedriver]# make -C /lib/modules/`uname -r`/build M=`pwd` modules
make: Entering directory `/usr/src/kernels/3.10.0-514.el7.x86_64'
  CC [M]  /root/gitrepo/Bdevicedriver/blockvdisk.o
  Building modules, stage 2.
  MODPOST 1 modules
  CC      /root/gitrepo/Bdevicedriver/blockvdisk.mod.o
  LD [M]  /root/gitrepo/Bdevicedriver/blockvdisk.ko
make: Leaving directory `/usr/src/kernels/3.10.0-514.el7.x86_64'
[root@ Bdevicedriver]#

[root@ Bdevicedriver]# ls
blockvdisk.c  blockvdisk.ko  blockvdisk.mod.c  blockvdisk.mod.o  blockvdisk.o  Makefile  modules.order  Module.symvers

[root@ Bdevicedriver]# insmod blockvdisk.ko

[root@ Bdevicedriver]# lsmod | grep blockvdisk
blockvdisk             12752  0
[root@ Bdevicedriver]#
