zyre_tools allows you to list [Zyre](https://github.com/zeromq/zyre) nodes and groups, get info on them and listen to shouts from them.

## Usage
#### Dependencies
[Zyre](https://github.com/zeromq/zyre)

[GNU Readline](http://tiswww.cwru.edu/php/chet/readline/rltop.html)

#### Compile
```
make
```

#### Run
```
./zyre_tools
```

Available commands are as follows:
```
nl                       : list all peers
ni <uuid>                 : get info on a peer
necho <uuid>             : echo shouts and whispers by a peer
gl                       : list all groups
gi <group name>          : get info on a group
gecho <group name>       : echo shouts on a group
join <group name>        : join a group
leave <group name>       : leave a group
stop                     : stop echoing whispers and shouts
s <group name> <message> : shout a message to a group
w <uuid> <message>       : whisper a message to a peer
help
q
```
