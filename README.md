# ctfdump
Command-line utility that dumps the CTF data of the selected file to the
stdout.

## Usage
`ctfdump [-dfglt] <file>`

## Example output
```
root@FreeBSD_Box:~ # ctfdump -l /boot/kernel/zlib.ko
-- Labels ------
 Name: FreeBSD 10.0-RELEASE i386 1000510
 Index: 143
```

