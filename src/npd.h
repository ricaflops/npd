/* npd project: npd.h
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

#pragma once

#include <string>
#include <vector>
#include <cstdint>

#include "decoder.h"

/// @brief Disassembler class
class NpDisassembler
{
public:
    NpDisassembler(bool asmOut, 
                   bool hexMode, 
                   char commentChar, 
                   const std::string &version, 
                   std::ostream& outStream);
    ~NpDisassembler();
    
    void disassemble(std::vector<uint8_t> const *pInput, 
                     const std::string &filename);
    
private:
    void FirstPass();
    void SecondPass();
    
    void AddToLabelList(uint16_t address);
    bool hasLabel(uint16_t address) const;
    
    void AppendTab(int tabSize, std::string &textLine);
    void AppendComment(std::string &text, const std::string &comment);
    
    void AddCommentLine(const std::string &comment);
    void AddBarLine(int n);
    void AddLabelLine(uint16_t x);

private:
    // Instruction decoder
    Decoder m_decoder;

    bool m_asmOutput;
    char m_commentChar;
	std::string m_version;
    
    char m_barChar;
    size_t m_romSize;
	std::vector<uint8_t> const *pBinary=NULL;
    std::ostream& m_outStream;
    
    // Address Label List
    std::vector<uint16_t> m_labelList;

    // The Nanoprocessor address bus size is 11-bits
    static constexpr size_t MaxRomSize = 2048; 

    static constexpr int OpCodeTabSize = 16;
    static constexpr int InstructionTabSize = 10;
    static constexpr int CommentTabSize = 26;
    static constexpr int TinyBarSize = 13;
    static constexpr int ShortBarSize = 26;
    static constexpr int LongBarSize = 40;
};
