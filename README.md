# T1

## OS and environment used for the project
```console
foo@bar:~$ sw_vers
ProductName:    Mac OS X
ProductVersion: 10.14.4
BuildVersion:   18E226

foo@bar:~$ hyper version
2.1.2

foo@bar:~$ gcc-8 --version
gcc-8 (Homebrew GCC 8.3.0) 8.3.0
Copyright (C) 2018 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
```

## How to compile on macOS

### Compile code

```console
foo@bar:~$ brew install gcc-8
foo@bar:~$ gcc-8 T1/main.c -o main
```

## Screens from code

### Success case

```console
foo@bar:~$ ./main 192.111.144.114
Data/hora: Sun Apr 14 03:22:44 2019
```

### Fail cases

```console
foo@bar:~$ ./main 
You need to put the host IP address:

./main [Host IP address]

: Undefined error: 0
```

```console
foo@bar:~$ ./main 132.163.96.5
Trying again...
Data/hora: não foi possível contactar servidor
: Resource temporarily unavailable
```

```console
foo@bar:~$ ./main 132.163.96.12
Segmentation fault: 11
```