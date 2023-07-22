/* npd project: decoder.h
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

/// @brief Nanoprocessor opcode, instruction, and description
struct Instruction
{
    uint8_t opcode;
    std::string mnemonic;
    std::string comment;
};

/// @brief Nanoprocessor opcode translation class
class Decoder
{
public:
    Decoder();
    ~Decoder();

    void TranslateOpCode(uint8_t opcode, uint8_t parameter, std::string &mnemonic, std::string &comment);
    void SetHexMode() { m_hex = true; };
    void SetOctalMode() { m_hex = false; };

    void AppendByteString(uint8_t x, std::string &out);
    void AppendAddressString(uint16_t x, std::string &out);

    bool isHexMode() const { return m_hex; };
    bool isDirectAddressing(uint8_t opcode) const;
    bool isTwoByteInstruction(uint8_t opcode) const;
    bool isReturnOrJumpInstruction(uint8_t opcode) const;
    bool isSkipInstruction(uint8_t opcode) const;
    
    uint16_t DirectAddress(uint8_t opcode, uint8_t parameter) const;
    
private:
    void AppendNumberString(uint8_t x, std::string &out);
    uint8_t Mask3bits(uint8_t x) const { return (x & 0b00000111); };
    uint8_t Mask4bits(uint8_t x) const { return (x & 0b00001111); };
    uint8_t Clear3bits(uint8_t x) const { return (x & 0b11111000); };
    uint8_t Clear4bits(uint8_t x) const { return (x & 0b11110000); };
    
private:
    bool m_hex;

    static const std::vector<Instruction> SimpleDirectOpCode;
    static const Instruction DoubleDirectOpCode;
    static const std::vector<Instruction> DoublePagedOpCode;
    static const std::vector<Instruction> Single3bitOpCode;
    static const std::vector<Instruction> Single4bitOpCode;
    static const std::vector<Instruction> Double4bitOpCode;

    // One byte instructions, simple opcodes
    static constexpr uint8_t INB_opcode = 0b00000000;
    static constexpr uint8_t DEB_opcode = 0b00000001;
    static constexpr uint8_t IND_opcode = 0b00000010;
    static constexpr uint8_t DED_opcode = 0b00000011;
    static constexpr uint8_t CLA_opcode = 0b00000100;
    static constexpr uint8_t CMA_opcode = 0b00000101;
    static constexpr uint8_t LSA_opcode = 0b00000110;
    static constexpr uint8_t RSA_opcode = 0b00000111;
    static constexpr uint8_t SGT_opcode = 0b00001000;
    static constexpr uint8_t SLT_opcode = 0b00001001;
    static constexpr uint8_t SEQ_opcode = 0b00001010;
    static constexpr uint8_t SAZ_opcode = 0b00001011;
    static constexpr uint8_t SLE_opcode = 0b00001100;
    static constexpr uint8_t SGE_opcode = 0b00001101;
    static constexpr uint8_t SNE_opcode = 0b00001110;
    static constexpr uint8_t SAN_opcode = 0b00001111;
    static constexpr uint8_t SES_opcode = 0b00011111;
    static constexpr uint8_t ENI_opcode = 0b00101111;
    static constexpr uint8_t SEZ_opcode = 0b00111111;
    static constexpr uint8_t NOP_opcode = 0b01011111;
    static constexpr uint8_t DSI_opcode = 0b10101111;
    static constexpr uint8_t RTI_opcode = 0b10110000;
    static constexpr uint8_t RTE_opcode = 0b10110001;
    static constexpr uint8_t STE_opcode = 0b10110100;
    static constexpr uint8_t CLE_opcode = 0b10110101;
    static constexpr uint8_t RTS_opcode = 0b10111000;
    static constexpr uint8_t RSE_opcode = 0b10111001;
    
    // Two bytes instruction, simple opcode and data
    static constexpr uint8_t LDR_opcode = 0b11001111;

    // Two bytes instructions, 3-bit page in opcode and offset
    static constexpr uint8_t JMP_opcode = 0b10000000;
    static constexpr uint8_t JSB_opcode = 0b10001000;

	// One byte instructions, 3-bit operand in opcode
    static constexpr uint8_t SBS_opcode = 0b00010000;
    static constexpr uint8_t SFS_opcode = 0b00011000;
    static constexpr uint8_t SBN_opcode = 0b00100000;
    static constexpr uint8_t STC_opcode = 0b00101000;
    static constexpr uint8_t SBZ_opcode = 0b00110000;
    static constexpr uint8_t SFZ_opcode = 0b00111000;
    static constexpr uint8_t JAI_opcode = 0b10010000;
    static constexpr uint8_t JAS_opcode = 0b10011000;
    static constexpr uint8_t CBN_opcode = 0b10100000;
    static constexpr uint8_t CLC_opcode = 0b10101000;

	// One byte instruction, 4-bit operand in opcode
    static constexpr uint8_t INA_opcode = 0b01000000;
    static constexpr uint8_t OTA_opcode = 0b01010000;
    static constexpr uint8_t LDA_opcode = 0b01100000;
    static constexpr uint8_t STA_opcode = 0b01110000;
    static constexpr uint8_t LDI_opcode = 0b11100000;
    static constexpr uint8_t STI_opcode = 0b11110000;

	// Two bytes instructions. 4-bit operand in opcode and data
    static constexpr uint8_t OTR_opcode = 0b11000000;
    static constexpr uint8_t STR_opcode = 0b11010000;
};

