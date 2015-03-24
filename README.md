# ctfdump
Print CTF data to `stdout`.

## Usage
`ctfdump [-adfglt] <file>`
 * `-a` print all
 * `-d` print data objects
 * `-f` print function objects
 * `-g` print general information
 * `-l` print labels
 * `-t` print types

## Build
```
$ ninja
```
or
```
$ make
```

## Dependencies
 * libctf

## Examples
#### Labels
```
root@FreeBSD_Box:~ # ctfdump -l /boot/kernel/zlib.ko
-- Labels ------
 Name: FreeBSD 10.0-RELEASE i386 1000510
 Index: 143
```

#### Data objects
```
root@FreeBSD_Box:~ # ctfdump -d /boot/kernel/zfs.ko | head -9
-- Data Objects ------
Name: avl_balance2child
Type: const int [3]

Name: avl_child2balance
Type: const int [2]

Name: nvs_native_ops
Type: const nvs_ops_t
```

#### Types
```
root@FreeBSD_Box:~ # ctfdump -t /boot/kernel/if_xl.ko | head -15
-- Types ------
      ID: 1
    Kind: int
    Root: no
    Name: unsigned char
    Size: 8
  Offset: 0
  Signed: no
 Content: char

      ID: 2
    Kind: typedef
    Root: yes
Old type: unsigned char
New type: __uint8_t
```

## License
2-clause BSD, for more information please see the [license](LICENSE.md).

## Author
Daniel Lovasko lovasko@freebsd.org

## Thanks
 * George Neville-Neil

