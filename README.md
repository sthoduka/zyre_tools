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
node list                    : lists all peers
node info <uuid>             : gets info on a peer
node listen <uuid>           : listen to shouts and whispers by a peer
group list                   : lists all groups
group info <group name>      : gets info on a group
group listen <group name>    : listens to shouts on a group
group join <group name>      : joins a group
group leave <group name>     : leaves a group
stop                         : stops printing whispers and shouts
shout <group name> <message> : shouts a message to a group
whisper <uuid> <message>     : whispers a message to a peer
help
exit
```
