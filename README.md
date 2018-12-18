
What is MTCP-Redis?
--------------

Mtcp-Redis is a Redis database with NFV network components. It rebuilds the network function in user space, which can improve the performance and network module customization on Redis.


The basic idea is using dynamic library reloading to replace the socket function in Redis without modifying Redis function code frequently.

Firstly, we need to analyze network components on Redis server side.

There is one significant Loop(aeMain Cycle) for epoll_wait function.

There are mainly three steps in receiving message : acceptTcpHandler(accept), readQueryFromClient(read) and sendReplyToClient(write). All of them is loaded as a epoll event by epoll_ctl. 

If all socket function in Redis can be replaced by Mtcp function, we can bypass the kernel components and define network in user space!

![image](https://github.com/XinShuYang/mtcp_redis/blob/master/example/structure.png)

We can use dlsym() to handle the address of functions from linux socket library. And then redirect it to our own Mtcp function by function redefinition.

![image](https://github.com/XinShuYang/mtcp_redis/blob/master/example/code1.png)
![image](https://github.com/XinShuYang/mtcp_redis/blob/master/example/code2.png)

Also, to implement this, the Redis-Server executable file need linker to link both library and obj files from Redis and Mtcp.

<div align=center><img src="https://github.com/XinShuYang/mtcp_redis/blob/master/example/linker.png" width="500" height="500" /></div>

However, I found there are some kernel socket library functions are used by mtcp(socket) and other components in Redis. If all functions are redirected to mtcp, it doesn’t work. So we need to choose certain functions and add some variable to indicate whether it should be redirected to mtcp function. 



* The full list of Redis commands. http://redis.io/commands
* There is much more inside the Redis official documentation. http://redis.io/documentation
If you need to get more infromation about original redis version. You can find more detailed documentation at [redis.io](https://redis.io).

Building MTCP-Redis
--------------

MTCP-Redis can be compiled and used with DPDK on X86 Linux platform.


It is as simple as:

    % make

You can build redis-server:

    % make redis-server

However, make test is not useable now.


Fixing build problems (MY ERROR RECORD)
---------

Originally the server node has eth0(128.104.222.152  255.255.254.0),eth1(192.168.1.3 255.255.255.0 ) and eth2(192.168.1.2 255.255.255.0) .

![image](https://github.com/XinShuYang/mtcp_redis/blob/master/example/error1.png)

The client node has eth0 and eth1

![image](https://github.com/XinShuYang/mtcp_redis/blob/master/example/error2.png)

At this time, I tested the traditional tcp connection between server and client on LAN(server bind on 192.168.1.3, client on 192.168.1.1 can send message to it), the connection is successful.


Then, I use the instructions on Cloudlab server node:


    cd /local/onvm/openNetVM/scripts  
    source setup_cloudlab.sh  
    sudo ifconfig eth1 down
    ./setup_environment.sh

I can see eth1 is down and bind with DPDK driver successfully. 

Then I start the dpdk0 interface from dpdk-setup-iface.sh on server node

	Sudo ./dpdk-setup-iface.sh dpdk0 192.168.1.3 255.255.255.0 up

![image](https://github.com/XinShuYang/mtcp_redis/blob/master/example/error3.png)

Then I run set up the cpu core number in ./configure and run “make” in mtcp dictionary
There is no error in this step from bash.

Then I start the epserver in server node

    sudo ./epserver -p www -f epserver-multiprocess.conf -N 1
    
![image](https://github.com/XinShuYang/mtcp_redis/blob/master/example/error4.png)

Then I try to use tcp connection on client server, since the address and netmark of dpdk0 is as same as the eth1. I think the connection will be successful. But the connection is refused

Then I found the new hardware address of dpdk0  is different. So I update the arp table on client node. But then the tcp connection even not get a respond…

![image](https://github.com/XinShuYang/mtcp_redis/blob/master/example/error5.png)

Now I can’t get connection to dpdk0(192.168.1.3) from client(192.168.1.1). The eth2(192.168.1.2) can be access from client.


Solution: All the problems above are from wrong binding address on NIC.
The address you start in dpdk0 should be as same as the address you down in eth.

There are still some problems in redis-benchmark, We can't get the whole data from multiple clients now. I am trying to fix this problem.



Running MTCP-Redis
-------------

To run Redis with the default configuration just type:

    % cd src
    % sudo ./redis-server 

If you want to provide your redis.conf, you have to run it using an additional
parameter (the path of the configuration file):

    % cd src
    % sudo ./redis-server redis.conf 

It is possible to alter the Redis configuration by passing parameters directly
as options using the command line. Examples:

    % cd src
    % sudo ./redis-server redis.conf --maxclients 100000

All the options in redis.conf are also supported as options using the command
line, with exactly the same name.

Using client with Mtcp-Redis Server
------------------

You can use redis-cli to access the server of Mtcp-Redis. Start a redis-server instance,
then in another terminal try the following:

    % cd src
    % ./redis-cli -h 192.168.1.1 -p 6379
    redis> SET runoobkey redis
    OK
    redis> DEL runoobkey
    (integer) 1
    redis> HMSET hashkey name "redis tutorial" description "redis basic commands for caching" likes 20 visitors 23000
    OK
    redis> HGETALL hashkey
    1) "name"
    2) "redis tutorial"
    3) "description"
    4) "redis basic commands for caching"
    5) "likes"
    6) "20"
    7) "visitors"
    8) "23000"
    redis> SADD setexample redis
    (integer) 1
    redis> SADD setexample mongodb
    (integer) 1
    redis> SADD setexample mysql
    (integer) 1
    redis> SMEMBERS setexample
    1) "mysql"
    2) "mongodb"
    3) "redis"
    

You can find the list of all the available commands at http://redis.io/commands.

Installing MTCP-Redis
-----------------

In order to install Redis binaries into /usr/local/bin just use:

    % make install

You can use `make PREFIX=/some/other/directory install` if you wish to use a
different destination.

Make install will just install binaries in your system, but will not configure
init scripts and configuration files in the appropriate place. This is not
needed if you want just to play a bit with Redis, but if you are installing
it the proper way for a production system, we have a script doing this
for Ubuntu and Debian systems:

    % cd utils
    % ./install_server.sh

The script will ask you a few questions and will setup everything you need
to run Redis properly as a background daemon that will start again on
system reboots.

You'll be able to stop and start Redis using the script named
`/etc/init.d/redis_<portnumber>`, for instance `/etc/init.d/redis_6379`.


Redis internals
===

If you are reading this README you are likely in front of a Github page
or you just untarred the Redis distribution tar ball. In both the cases
you are basically one step away from the source code, so here we explain
the Redis source code layout, what is in each file as a general idea, the
most important functions and structures inside the Redis server and so forth.
We keep all the discussion at a high level without digging into the details
since this document would be huge otherwise and our code base changes
continuously, but a general idea should be a good starting point to
understand more. Moreover most of the code is heavily commented and easy
to follow.

Source code layout
---

The MTCP-Redis root directory just contains this README, the Makefile which
calls the real Makefile inside the `src` directory and an example
configuration for Redis and Sentinel.

Inside the root are the following important directories:

* `src`: contains the Redis implementation, written in C.
* `tests`: contains the unit tests, implemented in Tcl.
* `deps`: contains libraries Redis uses. Everything needed to compile Redis is inside this directory; your system just needs to provide `libc`, a POSIX compatible interface and a C compiler.
* `mtcp`: contains the Mtcp object file and library implementation and the mod.h file, written in C.
* `util`: contains the Mtcp object file and library implementation, written in C.
* `io_engine`: contains the DPDK file, written in C.

mod.h
---
This file contains all functions which can access mtcp library and redirect the socket functions in redis server.c


server.c
---

This is the entry point of the Redis server, where the `main()` function
is defined. The following are the most important steps in order to startup
the Redis server.

* `initServerConfig()` setups the default values of the `server` structure.
* `initServer()` allocates the data structures needed to operate, setup the listening socket, and so forth.
* `aeMain()` starts the event loop which listens for new connections.

There are two special functions called periodically by the event loop:

1. `serverCron()` is called periodically (according to `server.hz` frequency), and performs tasks that must be performed from time to time, like checking for timedout clients.
2. `beforeSleep()` is called every time the event loop fired, Redis served a few requests, and is returning back into the event loop.

Inside server.c you can find code that handles other vital things of the Redis server:

* `call()` is used in order to call a given command in the context of a given client.
* `activeExpireCycle()` handles eviciton of keys with a time to live set via the `EXPIRE` command.
* `freeMemoryIfNeeded()` is called when a new write command should be performed but Redis is out of memory according to the `maxmemory` directive.
* The global variable `redisCommandTable` defines all the Redis commands, specifying the name of the command, the function implementing the command, the number of arguments required, and other properties of each command.

networking.c
---

This file defines all the I/O functions with clients, masters and replicas
(which in Redis are just special clients):

* `createClient()` allocates and initializes a new client.
* the `addReply*()` family of functions are used by commands implementations in order to append data to the client structure, that will be transmitted to the client as a reply for a given command executed.
* `writeToClient()` transmits the data pending in the output buffers to the client and is called by the *writable event handler* `sendReplyToClient()`.
* `readQueryFromClient()` is the *readable event handler* and accumulates data from read from the client into the query buffer.
* `processInputBuffer()` is the entry point in order to parse the client query buffer according to the Redis protocol. Once commands are ready to be processed, it calls `processCommand()` which is defined inside `server.c` in order to actually execute the command.
* `freeClient()` deallocates, disconnects and removes a client.

aof.c and rdb.c
---

As you can guess from the names these files implement the RDB and AOF
persistence for Redis. Redis uses a persistence model based on the `fork()`
system call in order to create a thread with the same (shared) memory
content of the main Redis thread. This secondary thread dumps the content
of the memory on disk. This is used by `rdb.c` to create the snapshots
on disk and by `aof.c` in order to perform the AOF rewrite when the
append only file gets too big.

The implementation inside `aof.c` has additional functions in order to
implement an API that allows commands to append new commands into the AOF
file as clients execute them.

The `call()` function defined inside `server.c` is responsible to call
the functions that in turn will write the commands into the AOF.

db.c
---

Certain Redis commands operate on specific data types, others are general.
Examples of generic commands are `DEL` and `EXPIRE`. They operate on keys
and not on their values specifically. All those generic commands are
defined inside `db.c`.

Moreover `db.c` implements an API in order to perform certain operations
on the Redis dataset without directly accessing the internal data structures.

The most important functions inside `db.c` which are used in many commands
implementations are the following:

* `lookupKeyRead()` and `lookupKeyWrite()` are used in order to get a pointer to the value associated to a given key, or `NULL` if the key does not exist.
* `dbAdd()` and its higher level counterpart `setKey()` create a new key in a Redis database.
* `dbDelete()` removes a key and its associated value.
* `emptyDb()` removes an entire single database or all the databases defined.

The rest of the file implements the generic commands exposed to the client.

object.c
---

The `robj` structure defining Redis objects was already described. Inside
`object.c` there are all the functions that operate with Redis objects at
a basic level, like functions to allocate new objects, handle the reference
counting and so forth. Notable functions inside this file:

* `incrRefcount()` and `decrRefCount()` are used in order to increment or decrement an object reference count. When it drops to 0 the object is finally freed.
* `createObject()` allocates a new object. There are also specialized functions to allocate string objects having a specific content, like `createStringObjectFromLongLong()` and similar functions.

This file also implements the `OBJECT` command.

replication.c
---

This is one of the most complex files inside Redis, it is recommended to
approach it only after getting a bit familiar with the rest of the code base.
In this file there is the implementation of both the master and replica role
of Redis.

One of the most important functions inside this file is `replicationFeedSlaves()` that writes commands to the clients representing replica instances connected
to our master, so that the replicas can get the writes performed by the clients:
this way their data set will remain synchronized with the one in the master.

This file also implements both the `SYNC` and `PSYNC` commands that are
used in order to perform the first synchronization between masters and
replicas, or to continue the replication after a disconnection.

Other C files
---

* `t_hash.c`, `t_list.c`, `t_set.c`, `t_string.c` and `t_zset.c` contains the implementation of the Redis data types. They implement both an API to access a given data type, and the client commands implementations for these data types.
* `ae.c` implements the Redis event loop, it's a self contained library which is simple to read and understand.
* `sds.c` is the Redis string library, check http://github.com/antirez/sds for more information.
* `anet.c` is a library to use POSIX networking in a simpler way compared to the raw interface exposed by the kernel.
* `dict.c` is an implementation of a non-blocking hash table which rehashes incrementally.
* `scripting.c` implements Lua scripting. It is completely self contained from the rest of the Redis implementation and is simple enough to understand if you are familar with the Lua API.
* `cluster.c` implements the Redis Cluster. Probably a good read only after being very familiar with the rest of the Redis code base. If you want to read `cluster.c` make sure to read the [Redis Cluster specification][3].

[3]: http://redis.io/topics/cluster-spec

Anatomy of a Redis command
---

All the Redis commands are defined in the following way:

    void foobarCommand(client *c) {
        printf("%s",c->argv[1]->ptr); /* Do something with the argument. */
        addReply(c,shared.ok); /* Reply something to the client. */
    }

The command is then referenced inside `server.c` in the command table:

    {"foobar",foobarCommand,2,"rtF",0,NULL,0,0,0,0,0},

In the above example `2` is the number of arguments the command takes,
while `"rtF"` are the command flags, as documented in the command table
top comment inside `server.c`.

After the command operates in some way, it returns a reply to the client,
usually using `addReply()` or a similar function defined inside `networking.c`.

There are tons of commands implementations inside the Redis source code
that can serve as examples of actual commands implementations. To write
a few toy commands can be a good exercise to familiarize with the code base.

There are also many other files not described here, but it is useless to
cover everything. We want to just help you with the first steps.
Eventually you'll find your way inside the Redis code base :-)

Enjoy!
