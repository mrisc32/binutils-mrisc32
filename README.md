# binutils for MRISC32

This is a fork of [binutils-gdb](https://sourceware.org/git/gitweb.cgi?p=binutils-gdb.git) with support for [MRISC32](https://github.com/mbitsnbites/mrisc32).

## Building

Configure and build:

```bash
$ cd binutils-mrisc32
$ mkdir build
$ cd build
$ ../configure --target=mrisc32 --program-prefix=mrisc32- --with-system-zlib --disable-gdb --disable-sim
$ make
```

Install:

```bash
$ sudo make install
```

## About this Git repo

The MRISC32 port of binutils is maintained as a branch that is periodically rebased on top of the latest upstream master branch and force pushed to the fork repository. To update your local clone you need to:

```bash
$ git fetch origin
$ git reset --hard origin/users/mbitsnbites/mrisc32
```
