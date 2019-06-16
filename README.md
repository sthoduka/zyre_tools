# Zyre tools

zyre_tools allows you to list [Zyre](https://github.com/zeromq/zyre) nodes and groups, get info on them and listen to shouts from them.

## Index
- [Dependencies](#dependencies)
- [Compile and Install](#compile-and-install)
- [Usage](#usage)
  - [zyrenode](#zyrenode)
  - [zyregroup](#zyregroup)
  - [zyrecli](#zyrecli)

### Dependencies

- [Zyre](https://github.com/zeromq/zyre)
- [GNU Readline](http://tiswww.cwru.edu/php/chet/readline/rltop.html) (`sudo apt-get install libreadline-dev`)

### Compile and Install
```
mkdir build
make
sudo make install
```

## Usage
### zyrenode
```
usage: zyrenode COMMAND [args]

Command line tool for printing information about zyre nodes

Commands:
  list                     list all the zyre nodes in the network
  info NODE_UUID           provide info on a zyre node in the network
  echo NODE_UUID           print all the messages from a zyre group
  pub NODE_UUID MESSAGE    whisper message to a zyre group
  pub NODE_UUID -f FILE    whisper contents of file to a zyre group

optional arguments:
  -h, --help    show this help message and exit
```

### zyregroup
```
usage: zyregroup COMMAND [args]

Command line tool for printing information about zyre groups

Commands:
  list                 list all the zyre groups in the network
  info GROUP           provide info on a zyre groups in the network
  echo GROUP           print all the SHOUT messages from a zyre group
  pub GROUP MESSAGE    shout message to a zyre group
  pub GROUP -f FILE    shout contents of file to a zyre group

optional arguments:
  -h, --help    show this help message and exit
```

### zyrecli
```
usage: zyrecli [node_name]

Command line interface for complete zyre based operations

Commands:
  nl                       : list all peers
  ni <uuid>                : get info on a peer
  necho <uuid>             : echo shouts and whispers by a peer
  gl                       : list all groups
  gi <group name>          : get info on a group
  gecho <group name>       : echo shouts on a group
  join <group name>        : join a group
  leave <group name>       : leave a group
  stop                     : stop echoing whispers and shouts
  s <group name> <message> : shout a message to a group
  w <uuid> <message>       : whisper a message to a peer
  help                     : print list of available commands
  q                        : quit program
```
