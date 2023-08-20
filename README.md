# npd

**npd** is a simple disassembler for the *HP Nanoprocessor*.

## Example

Disassemble a binary file `rom.bin` by typing:

	./npd rom.bin

A listing file `rom.lst` will be created:

```
                ;----------------------------------------
                ; npd 1.0 - Nanoprocessor Disassembler
                ; File: rom.bin   (19 Bytes)
                ; Date: 2023-08-20   18:16
                ; Mode: Octal
                ;----------------------------------------
                L_0000
0000:  100                INA  DS0        ; Input from Device to ACC
0001:  165                STA  R5         ; Store ACC at Register
0002:  320 000            STR  R0,000     ; Store data at Register
                L_0004
0004:  145                LDA  R5         ; Load ACC from Register
0005:  017                SAN             ; Skip on ACC != 0
                ;-------------
0006:  200 017            JMP  L_0017     ; Unconditional Jump
                ;--------------------------
0010:  003                DED             ; Decrement ACC (BCD)
0011:  165                STA  R5         ; Store ACC at Register
0012:  140                LDA  R0         ; Load ACC from Register
0013:  000                INB             ; Increment ACC (binary)
0014:  160                STA  R0         ; Store ACC at Register
0015:  200 004            JMP  L_0004     ; Unconditional Jump
                ;--------------------------
                L_0017
0017:  140                LDA  R0         ; Load ACC from Register
0020:  121                OTA  DS1        ; Output ACC to Device
0021:  200 000            JMP  L_0000     ; Unconditional Jump
                ;--------------------------
                          END
```

## Motivation

I started this project to help understand the workings of some vintage HP 
equipments that uses the Nanoprocessor like the old HP 4262A LCR meter.

## Requirements

As the source code is very simple the only requirement is a modern C++ compiler.
No special external library is necessary.

I compiled it in Linux using **g++** version 11.3.0 and **GNU Make** version 4.3 
but it shall be easy to compile in other operating systems as well using other C++ compiler.
 Check your compiler documentation.

## Compiling

At the terminal prompt in the project root directory type:

	g++ -std=c++17 ./src/*.cpp -o npd

Alternativelly, if you have `make` installed, type:

	make release=1

An executable file named **npd** shall be created in the same directory.

## Usage

**npd** is a command line application. It runs on a terminal.

To disassemble a binary file run **npd** followed by it's name.
For example, to disassemble the file `rom.bin` type:

	./npd rom.bin

It will create a listing file named `rom.lst`.

By default the output file includes addresses and opcodes.
To get a clean *rom.asm* file use the `-a` option:

	./npd -a rom.bin

By default all addresses, opcodes, and parameters will be in octal base
as the original processor documentation.
If you prefer hexadecimals use option `-x` .

### Command line options

These are the available **npd** command line options:

| option  | effect |
|---------|--------|
| `-h`         | Output a help text and exit |
| `-v`         | Output version and license information, and exit |
| `-o OUTFILE` | Set the output file name |
| `-f`         | Overwrite an existing output file without warning |
| `-a`         | `.asm` output file (excludes addresses and opcodes) |
| `-x`         | Use hexadecimals. The default is octal |
| `-c`         | Use '*' in comments. The default is ';' |

## References

To learn about the HP Nanoprocessor check these great resources:

- [CPU Schack Museum](<https://www.cpushack.com/2020/08/09/the-forgotten-ones-hp-nanoprocessor/>):
"The Forgotten Ones: HP Nanoprocessor".

- [CPU Schack Museum](<https://www.cpushack.com/2020/08/20/hp-nanoprocessor-mask-set/>):
"HP NanoProcessor Mask Set".

- [Ken Shirriff's blog](<http://www.righto.com/2020/09/inside-hp-nanoprocessor-high-speed.html>):
"Inside the HP Nanoprocessor: a high-speed processor that can't even add". Here you can find a link to the *Nanoprocessor user's guide*.

- [Ken Shirriff's blog](http://www.righto.com/2020/09/hp-nanoprocessor-part-ii-reverse.html):
"HP Nanoprocessor part II: Reverse-engineering the circuits from the masks".

## License

**npd**  Copyright (C) 2023  Ricardo Fernandes Lopes.

License GPLv3+: GNU GPL version 3 or later.
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it under 
certain conditions; see file `Copyright.txt` for details or visit 
<https://gnu.org/licenses/gpl.html>.
