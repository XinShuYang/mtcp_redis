Building MTCP_Redis on ONVM
--------------

MTCP_Redis can be compiled and used with both DPDK and ONVM. If you want to use MTCP_Redis on ONVM. Please follow the instruction below.    

First, make sure your ONVM version is 19.02(or later).  

Second, make sure you can run MTCP(devel branch) on your ONVM.  

Then：

     git clone https://github.com/XinShuYang/mtcp_redis.git

Switch to onvm branch:

     git checkout onvm

Set up both MTCP and MTCP_Redis path:

     export MTCP_SDK = <PATH>    e.g. export MTCP_SDK = /local/onvm/mtcp
     export MTCP_REDIS = <PATH>    e.g. export MTCP_REDIS = /local/onvm/mtcp_redis

Run setup.sh to compile mtcp_redis
```bash
  cd $MTCP_REDIS  
  ./onvm_setup.sh  
```

Running MTCP_Redis with ONVM
-------------
Start ONVM
```bash
  e.g. ./go.sh 0,1,2 3 0X3F8 -a 0x7f000000000 -s stdout
```

To run MTCP_Redis with the configuration file, type:

     cd src
     sudo ./redis-server <cpu_core_number> --maxclients <clients number>
     e.g.  sudo ./redis-server 3 --maxclients 100000

You should make sure the cpu core is not conflict with the ONVM manager core.  

Then you can test the redis-server from a client or benchmark on another node.  

You may need to change the ip address or other parameters in redis.conf.  

Running Multiple MTCP_Redis Instances with ONVM
-------------
This function need a new onvm forward NF. It will be updated soon!

Enjoy!
