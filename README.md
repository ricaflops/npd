# npd

**npd** is a simple disassembler for the *HP Nanoprocessor*.

## Example

Disassemble a binary file `rom.bin` by typing `./npd rom.bin` .

A listing file `rom.lst` will be created:

```
                ;----------------------------------------
                ; npd v1.0 - Nanoprocessor Disassembler
                ; File: ./rom.bin
                ; Size: 14 Bytes
                ; Date: 2023-07-20   17:43
                ; Mode: Octal
                ;----------------------------------------
                L_0000
0000:  145                LDA  R5         ; Load ACC from Register
0001:  003                DED             ; Decrement ACC (BCD)
0002:  017                SAN             ; Skip on ACC != 0
                ;-------------
0003:  200 013            JMP  L_0013     ; Unconditional Jump
                ;--------------------------
0005:  165                STA  R5         ; Store ACC at Register
0006:  146                LDA  R6         ; Load ACC from Register
0007:  002                IND             ; Increment ACC (BCD)
0010:  166                STA  R6         ; Store ACC at Register
0011:  200 000            JMP  L_0000     ; Unconditional Jump
                ;--------------------------
                L_0013
0013:  146                LDA  R6         ; Load ACC from Register
0014:  002                IND             ; Increment ACC (BCD)
0015:  166                STA  R6         ; Store ACC at Register

                          END
```

## Motivation

I started this project to help understand the workings of some vintage HP 
equipments that uses the Nanoprocessor like the old HP 4262A LCR meter.

## Requirements

As the source code is very simple the only requirement is a modern C++ compiler.
No external library is necessary.

I compiled it in Linux using **g++** version 11.3.0 and **GNU Make** version 4.3 
but it shall be easy to compile in other operating systems as well using other C++ compiler.
 Just read the compiler of choice documentation.

## Compiling

At the terminal prompt in the project root directory type:

		g++ -O2 -s -std=c++11 ./src/*.cpp -o npd

Alternativelly, if you have `make` installed, type:

		make

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
| `-v`         | Output version and license information and exit |
| `-o OUTFILE` | Set output file name |
| `-f`         | Overwrite existing output file without warning |
| `-a`         | `.asm` output file (no addresses or opcodes) |
| `-x`         | Hexadecimal numbers (default is octal). |
| `-c`         | Asterisk comment character (default is `;`). |

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
