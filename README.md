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
node list
group list
node info <uuid>
group info <group name>
node listen <uuid>
group listen <group name>
help
exit
```
