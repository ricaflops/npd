/* npd project: main.cpp
 * Copyright (C) 2023  Ricardo Fernandes Lopes
 * 
 * This file is part of 'npd' - A Nanoprocessor Disassembler.
 *
 * 'npd' is free software: you can redistribute it and/or modify it 
 * under the terms of the GNU General Public License as published by 
 * the Free Software Foundation, either version 3 of the License, or 
 * (at your option) any later version.
 *
 * 'npd' is distributed in the hope that it will be useful, but WITHOUT 
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or 
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License 
 * for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with 'npd'. If not, see <https://www.gnu.org/licenses/>. 
 */

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <getopt.h>

#include "npd.h"

const std::string version = "1.0";

const std::string lstExtension(".lst");
const std::string asmExtension(".asm");

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
        std::cerr << "Usage: npd 'filename'" << std::endl;
        return -1;
    }

    // Command line options
    std::string inputFilename;
    std::string outputFilename;
    bool otherOutputFilename = false;
    bool overwriteOutput = false;
    bool asmOut = false;
    bool hexMode = false;
    char commentChar = ';';
    
    int opt;
    bool optError = false;
    while ((opt = getopt(argc, argv, ":o:hvfaxc")) != -1) 
    {
        switch (opt) 
        {
            case 'o':  // set output file name
                otherOutputFilename = true;
                outputFilename = optarg;
                break;
            case 'h':  // help text
                std::cout << "Usage: npd [OPTION]... FILE [-o OUTFILE]\n";
                std::cout << "Disassemble binary FILE to HP Nanoprocessor instructions.\n\n";
                std::cout << "Default output file is FILE.lst\n\n";
                std::cout << "OPTION\n";
                std::cout << "  -h            output this help text and exit.\n";
                std::cout << "  -v            output version and license information and exit.\n";
                std::cout << "  -o OUTFILE    set output file name.\n";
                std::cout << "  -f            overwrite existing output file without warning.\n";
                std::cout << "  -a            .asm output file (no addresses or opcodes).\n";
                std::cout << "  -x            use hexadecimals. Default is octal.\n";
                std::cout << "  -c            set comment character to '*'. Default is ';'.\n";
                return 0;
                break;
            case 'v':  // version text
				std::cout << "npd - Nanoprocessor Disassembler version " << version << "\n";
                std::cout << "Copyright (C) 2023  Ricardo F. Lopes\n";
                std::cout << "License GPLv3+: GNU GPL version 3 or later.\n";
                std::cout << "This program comes with ABSOLUTELY NO WARRANTY.\n";
                std::cout << "This is free software, and you are welcome to redistribute it\n";
                std::cout << "under certain conditions; see file 'Copyright.txt' for details\n";
                std::cout << "or visit <https://gnu.org/licenses/gpl.html>.\n";
                return 0;
                break;
            case 'f':  // overwrite output file without warning
                overwriteOutput = true;
                break;
            case 'a':  // suppress addresses and opcodes
                asmOut = true;
                break;
            case 'x':  // hexadecimal numbers
                hexMode = true;
                break;
            case 'c':  // use asterisk comment char as in the original HP documentation
                commentChar = '*';
                break;
            case '?':  // ERROR: Invalid option
                std::cerr << "Unknown option: -" << char(optopt) << std::endl;
                optError = true;
                break;
            case ':':  // ERROR: No output file name
                std::cerr << "Missing output file name (option -" << char(optopt) << ").\n";
                optError = true;
                break;
            default:  // (shall not reach here.. but..)
                optError = true;
                break;
        }
    }

    // Define input filename
    if (optind < argc)
    {
		inputFilename = argv[optind++];
	    // Find any extra non-option arguments (error)
        while (optind < argc)
        {
            std::cerr << "Invalid argument " << argv[optind++] << std::endl;
            optError = true;
        }
    }
    else
    {
	    std::cerr << "Missing input file name.\n";
	    optError = true;
    }

    // ERROR: Command line argument
    if (optError)
    {
		std::cerr << "Quitting..\n";
        return -1;
    }

    // Read input binary file
    std::ifstream inFileStream(inputFilename.c_str(), std::ios::in | std::ios::binary);
    if (!inFileStream.is_open()) 
    {
		std::cerr << "Error reading file '" << inputFilename << "'\n";
		return -1;
	}
	std::vector<uint8_t> binaryInput(std::istreambuf_iterator<char>(inFileStream), {});
    inFileStream.close();

    // Define output file name with proper extension
    if (!otherOutputFilename)
    {
        // Output file name
        size_t lastdot = inputFilename.find_last_of(".");
        if (lastdot == std::string::npos)
        {
            outputFilename = inputFilename;
        }
        else
        {
            outputFilename =  inputFilename.substr(0, lastdot);
        }

        // Output file extension
        if (asmOut)
        {
            outputFilename += asmExtension;
        }
        else
        {
            outputFilename += lstExtension;
        }
    }

    // Confirm overwrite output file
    std::ifstream testFileStream(outputFilename);
    bool fileExist = testFileStream.is_open();
    testFileStream.close();
    if (fileExist && !overwriteOutput)
    {
        char answer;
        testFileStream.close();
        std::cout << "File '" << outputFilename << "' already exist. Overwrite it? [Y/n]";
        std::cin >> answer;
        if (toupper(answer) != 'Y')
        {
            std::cerr << "Quitting.. No file written!\n";
            return -1;
        }		
    }
    
    // Create output file
    std::ofstream outFileStream(outputFilename);
    if (!outFileStream.is_open())
    {
        std::cerr << "Error opening file '" << outputFilename << "'\n";
        return -1;
    }

    // Disassemble
    NpDisassembler disasm(asmOut, hexMode, commentChar, outFileStream);
    disasm.disassemble(&binaryInput, inputFilename, version);
    outFileStream.close();
    
    std::cout << "Output file created: " << outputFilename << std::endl;
  
    return 0;
}
