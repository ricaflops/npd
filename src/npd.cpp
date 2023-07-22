/* npd project: npd.cpp
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

// NpDisassembler class implementation

#include <iostream>
#include <time.h>
#include <iomanip> //std::hex oct
#include <algorithm>  // find

#include "npd.h"

// Constructor
NpDisassembler::NpDisassembler(bool asmOut, bool hexMode, char commentChar, std::ostream& outStream)
: m_asmOutput(asmOut), m_commentChar(commentChar), m_outStream(outStream)
{
    m_romSize = 0;  // No binary data yet
    // set line bar characters according to comment character option
    m_barChar = '*';
	if (m_commentChar == ';')
	{
	    m_barChar = '-';
	}

    // set numeric mode hex/octal
    if (hexMode)
    {
        m_decoder.SetHexMode();
    }
    else
    {
        m_decoder.SetOctalMode();
    }
}

NpDisassembler::~NpDisassembler()
{
}

/// @brief Disassemble binary vector to output stream
void NpDisassembler::disassemble(std::vector<uint8_t> const *pInput, const std::string &filename, const std::string &version)
{    
	pBinary = pInput;
    m_romSize = (MaxRomSize < pBinary->size()) ? MaxRomSize : pBinary->size();
    
    std::string comment;

	// Header
	AddBarLine(LongBarSize);
    comment = "npd v" + version + " - Nanoprocessor Disassembler";
	AddCommentLine(comment);
	
    comment.erase();
	comment.append("File: ");
	comment.append(filename);
	AddCommentLine(comment);

	comment.erase();
	comment.append("Size: ");
	comment.append(std::to_string(m_romSize));
	comment.append(" Bytes");
	AddCommentLine(comment);

	// Get current time
    time_t rawtime = time(NULL);
    // Convert time_t to tm as local time
    struct tm *timeinfo;
    timeinfo = localtime (&rawtime);  
    // Format time as string
    char buffer [40];
    strftime(buffer, 40, "Date: %Y-%m-%d   %H:%M", timeinfo);
    comment.erase();
    comment.append(buffer);
    AddCommentLine(comment);

    if (m_decoder.isHexMode())
    {
		AddCommentLine("Mode: Hexadecimal");
	}
	else
	{
		AddCommentLine("Mode: Octal");
	}

	AddBarLine(LongBarSize);
	
	FirstPass();
	SecondPass();
}

/// @brief Disassembly First Pass. Creates labelled address list
void NpDisassembler::FirstPass()
{
	m_labelList.clear();
	// The reset vector always has a label
	AddToLabelList(0);
	
    // collect labels from Direct Addressing instruction operands: JMP, JSB
    uint16_t address = 0;
    uint8_t opcode;
    uint8_t parameter;
    
    while( (size_t)address < m_romSize-1 )
    {
        opcode = pBinary->at(address++);
        if (m_decoder.isTwoByteInstruction(opcode))
        {
            parameter = pBinary->at(address++);
            if (m_decoder.isDirectAddressing(opcode))
            {
                AddToLabelList( m_decoder.DirectAddress(opcode, parameter) );
            }
        }
    } 
}

/// @brief Disassembly Second Pass
void NpDisassembler::SecondPass()
{
    uint16_t address = 0;    // Program counter
    uint8_t opcode = 0; // current instruction
    uint8_t parameter = 0;  // eventual current instruction parameter
    std::string mnemonic;
    std::string comment;
    std::string text;
    
    while( (size_t)address < m_romSize )
    {
		// Insert Label
		if (hasLabel(address))
		{
			AddLabelLine(address);
		}

        // get instruction opcode
		opcode = pBinary->at(address);

        // Build new text line
        text.erase();
		// Include Address and opcode byte (.lst output only)
		if (!m_asmOutput)
	    {
			m_decoder.AppendAddressString(address, text);
			text.append(":  ");
			m_decoder.AppendByteString(opcode, text);
		}

        // get instruction parameter (only for two byte instructions)
        parameter = 0;
        if (m_decoder.isTwoByteInstruction(opcode) && 
            ((size_t)address < m_romSize-1))
        {
			parameter = pBinary->at(++address);
			// Include parameter byte (.lst output only)
            if (!m_asmOutput)
            {
                text.push_back(' ');
                m_decoder.AppendByteString(parameter, text);
            }
		}
        
        // Translate opcode
        m_decoder.TranslateOpCode(opcode, parameter, mnemonic, comment);

		// Include Instruction and comment
        AppendTab(InstructionTabSize, text);
        text.append(mnemonic);
        AppendComment(text, comment);
        m_outStream << text << std::endl;

        // Insert break after 'Jump' and 'Return' type instructions
        if ( m_decoder.isReturnOrJumpInstruction(opcode) )
        {
            AddBarLine(ShortBarSize);
        }
        
        // Insert break after 'Skip' type instructions
        if (m_decoder.isSkipInstruction(opcode))
        {
			AddBarLine(TinyBarSize);
		}
        
        // Next instruction
		address++;
	}
	
	// Finish
    m_outStream << std::endl;
    text.erase();
    AppendTab(InstructionTabSize, text);
    text.append("END");
    m_outStream << text << std::endl;
}

/// @brief Include address in the to-be-Label list
/// Insert address once in the list
void NpDisassembler::AddToLabelList(uint16_t address)
{
	if (!hasLabel(address))
	{
		m_labelList.push_back(address);
	}
}

/// @brief Check if address is in the label list
/// @return true if it does
bool NpDisassembler::hasLabel(uint16_t address) const
{
    return (std::find(m_labelList.begin(), m_labelList.end(), address) != m_labelList.end());
}

/// @brief Add spaces to align text in columns
void NpDisassembler::AppendTab(int tabSize, std::string &text)
{
    if (!m_asmOutput)
    {
        tabSize += OpCodeTabSize;
    }
    int tab = tabSize - text.size();
    if (tab > 0)
    {
        text.append(tab, ' ');
    }
}

void NpDisassembler::AppendComment(std::string &text, const std::string &comment)
{
    AppendTab(CommentTabSize, text);
    
	text.push_back(m_commentChar);
	text.push_back(' ');
	text.append(comment);
}

void NpDisassembler::AddCommentLine(const std::string &comment)
{
    std::string text;
    AppendTab(0, text);
    
	text.push_back(m_commentChar);
	text.push_back(' ');
	text.append(comment);
    m_outStream << text << std::endl;
}

void NpDisassembler::AddBarLine(int n)
{
    std::string text;
    AppendTab(0, text);
    
    text.push_back(m_commentChar);
    text.append(n, m_barChar);
    m_outStream << text << std::endl;
}

void NpDisassembler::AddLabelLine(uint16_t x)
{
    std::string text;
    AppendTab(0, text);
	
	text.append("L_");
	m_decoder.AppendAddressString(x, text);
    m_outStream << text << std::endl;
}

