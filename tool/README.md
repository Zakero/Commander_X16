# Tools

The scripts in this directory support developement for _Commander X16_.

## ConverterX16.py

Convert various data into a format native to the _Commander X16_.  This utility
has extensive help built-in.  Use `ConverterX16.py --help` to get started.

The `Example_Resource.ini` as an example of a resource file for
`ConverterX16.py`.

### Requirements

- Python v3
- Pillow v7

## sdcard.sh

The _Commander X16_ emulator is able to mound SD Card images.  In the 
[x16-emulator](https://github.com/commanderx16/x16-emulator) is a zipped SD 
Card that you can use, 
[sdcard.img.zip](https://github.com/commanderx16/x16-emulator/blob/master/sdcard.img.zip).
Using this script replaces the need for entering the commands to create  
loopback devices and mounting files. See `sdcard.sh --help` for details.

### Requirements

- Bash
