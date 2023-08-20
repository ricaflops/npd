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

// App version
const std::string version = "1.0";
// Output listing file name extension
const std::string lstExtension(".lst");
// Ouput assembly file name extension
const std::string asmExtension(".asm");

void showVersion()
{
	std::cout << "npd " << version << " - a Nanoprocessor disassembler\n\n";
	std::cout << "Copyright (C) 2023  Ricardo F. Lopes\n";
	std::cout << "License GPLv3+: GNU GPL version 3 or later.\n";
	std::cout << "This program comes with ABSOLUTELY NO WARRANTY.\n";
	std::cout << "This is free software, and you are welcome to redistribute it\n";
	std::cout << "under certain conditions; see file 'Copyright.txt' for details\n";
	std::cout << "or visit <https://gnu.org/licenses/gpl.html>.\n\n";
}

/// Show help text
void showHelp()
{
	std::cout << "Usage: npd [OPTION]... FILE [-o OUTFILE]\n";
	std::cout << "Disassemble a binary FILE into HP Nanoprocessor mnemonics.\n\n";
	std::cout << "OPTION\n";
	std::cout << "  -h            Output this help text and exit.\n";
	std::cout << "  -v            Output version and license information, and exit.\n";
	std::cout << "  -o OUTFILE    Set the output file name. The default is FILE.lst (.asm)\n";
	std::cout << "  -f            Overwrite an existing output file without warning.\n";
	std::cout << "  -a            .asm output file (excludes addresses and opcodes).\n";
	std::cout << "  -x            Use hexadecimals. The default is octal.\n";
	std::cout << "  -c            Use '*' in comments. The default is ';'.\n\n";
}

void showUsage()
{
	std::cerr << "Usage: npd 'binary_file'\n";
}

int main(int argc, char* argv[])
{
    if(argc < 2)
    {
		showUsage();
        return -1;
    }

    // Command line options
    std::string inputFilename;
    std::string outputFilename;
    bool overwriteOutput = false;
    bool asmMode = false;
    bool hexMode = false;
    char commentChar = ';';
    
    int opt;
    while ((opt = getopt(argc, argv, ":o:hvfaxc")) != -1) 
    {
        switch (opt) 
        {
            case 'o':  // set output file name
                outputFilename = optarg;
                break;
            case 'h':  // help
                showHelp();
                return 0;
                break;
            case 'v':  // app version
				showVersion();
                return 0;
                break;
            case 'f':  // overwrite output file without warning
                overwriteOutput = true;
                break;
            case 'a':  // suppress addresses and opcodes from output
                asmMode = true;
                break;
            case 'x':  // hexadecimal numbers
                hexMode = true;
                break;
            case 'c':  // use asterisk for comments (original HP documentation)
                commentChar = '*';
                break;
            case '?':  // ERROR: Invalid option
                std::cerr << "Unknown option: -" << char(optopt) << std::endl;
                return -1;
                break;
            case ':':  // ERROR: No output file name
                std::cerr << "Missing output file name (option -" << char(optopt) << ").\n";
                return -1;
                break;
            default:  // (shall not reach here.. but..)
                return -1;
                break;
        }
    }

	// Missing input file name
	if (optind >= argc)
    {
		showUsage();
		return -1;
    }
    
    // Define input file name
	inputFilename = argv[optind++];
	
	// Error on any extra non-option arguments
	while (optind < argc)
	{
		std::cerr << "Invalid argument " << argv[optind++] << std::endl;
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

    // Define output file
    if (outputFilename.empty())
    {
        // remove original extension
        size_t lastdot = inputFilename.find_last_of(".");
        if (lastdot == std::string::npos)
        {
            outputFilename = inputFilename;
        }
        else
        {
            outputFilename =  inputFilename.substr(0, lastdot);
        }

        // set new extension
        if (asmMode)
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
        std::cout << "File: " << outputFilename << "\nAlready exist. Overwrite it? [y/n]";
        std::cin >> answer;
        if (toupper(answer) != 'Y')
        {
            std::cerr << "Halted.\n";
            return -1;
        }		
    }
    
    // Create output file
    std::ofstream outFileStream(outputFilename);
    if (!outFileStream.is_open())
    {
        std::cerr << "Error writing file " << outputFilename << std::endl;
        return -1;
    }

    // Disassemble
    NpDisassembler disasm(asmMode, hexMode, commentChar, version, outFileStream);
    disasm.disassemble(&binaryInput, inputFilename);
    outFileStream.close();
    std::cout << "Output file: " << outputFilename << std::endl;

    return 0;
}
