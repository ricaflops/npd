/* npd project: decoder.cpp
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

// Decoder class implementation

#include <iostream>
#include <iomanip> //std::hex oct

#include "decoder.h"

Decoder::Decoder()
{
    m_hex = false;
}

Decoder::~Decoder()
{
}

/// @brief Translate opcode, returning mnemonic and comment strings
void Decoder::TranslateOpCode(uint8_t opcode, uint8_t parameter, std::string &mnemonic, std::string &comment)
{
    mnemonic = "???";
    comment = "Unknow Opcode!";
    uint16_t address;

    // Translate single byte simple instructions
    size_t i = 0;
    while( i < SimpleDirectOpCode.size() )
    {
        if (opcode == SimpleDirectOpCode.at(i).opcode)
        {
            mnemonic = SimpleDirectOpCode[i].mnemonic;
            comment = SimpleDirectOpCode[i].comment;
            return;
        }
        i++;
    }
    
    // Translate two byte instruction: LDR
    // 'LDR' (11001111) must be translated before 'OTR' (1100xxxx)
    if (opcode == DoubleDirectOpCode.opcode)
    {
        mnemonic = DoubleDirectOpCode.mnemonic;
        AppendByteString(parameter, mnemonic);
        comment = DoubleDirectOpCode.comment;
        return;
    }
    
    // Translate two byte instructions with page and offset adress: JMP JSB
    address = DirectAddress(opcode, parameter);
    i = 0;
    while( i < DoublePagedOpCode.size() )
    {
        if (Clear3bits(opcode) == DoublePagedOpCode.at(i).opcode)
        {
            mnemonic = DoublePagedOpCode[i].mnemonic;
            AppendAddressString(address, mnemonic);
            comment = DoublePagedOpCode[i].comment;
            return;
        }
        i++;
    }
    
    // Translate single byte instructions with 3-bit operands (bit, DC, indexing)
    i = 0;
    while( i < Single3bitOpCode.size() )
    {
        if (Clear3bits(opcode) == Single3bitOpCode.at(i).opcode)
        {
            mnemonic = Single3bitOpCode[i].mnemonic;
            AppendNumberString(Mask3bits(opcode), mnemonic);
            comment = Single3bitOpCode[i].comment;
            return;
        }
        i++;
    }
    
    // Translate single byte instructions with 4-bit operands (R, DS, Z)
    i = 0;
    while( i < Single4bitOpCode.size() )
    {
        if (Clear4bits(opcode) == Single4bitOpCode.at(i).opcode)
        {
            mnemonic = Single4bitOpCode[i].mnemonic;
            AppendNumberString(Mask4bits(opcode), mnemonic);
            comment = Single4bitOpCode[i].comment;
            return;
        }
        i++;
    }

    // Translate two bytes instructions with 4-bit operands (OTR, STR)
    i = 0;
    while( i < Double4bitOpCode.size() )
    {
        if (Clear4bits(opcode) == Double4bitOpCode[i].opcode)
        {
            mnemonic = Double4bitOpCode[i].mnemonic;
            AppendNumberString(Mask4bits(opcode), mnemonic);
            mnemonic.push_back(',');
            AppendByteString(parameter, mnemonic);
            comment = Double4bitOpCode[i].comment;
            return;
        }
        i++;
    }   

	// if reaches here it's a unknown opcode
    return;
}

/// @brief Check if an opcode is 'Direct Addressing' type
bool Decoder::isDirectAddressing(uint8_t opcode) const
{
    // 'JMP' and 'JSB' instructions use direct addressing
    opcode = Clear3bits(opcode);
    if ((opcode == JMP_opcode) || (opcode == JSB_opcode))
    {
        return true;
    }
    return false;
}

// Check if two byte instruction (LDR, OTR, STR, JMP, JSB)
bool Decoder::isTwoByteInstruction(uint8_t opcode) const
{
    if (opcode == LDR_opcode)
    {
        return true;
    }

    uint8_t maskedOpcode = Clear4bits(opcode);
    if ( (maskedOpcode == OTR_opcode) || (maskedOpcode == STR_opcode) )
    {
        return true;
    }

    maskedOpcode = Clear3bits(opcode);
    if ( (maskedOpcode == JMP_opcode) || (maskedOpcode == JSB_opcode) )
    {
        return true;
    }

    return false;
}

bool Decoder::isReturnOrJumpInstruction(uint8_t opcode) const
{
	return ( (opcode == RTS_opcode) ||
             (opcode == RSE_opcode) ||
             (opcode == RTI_opcode) ||
             (opcode == RTE_opcode) ||
             (Clear3bits(opcode) == JMP_opcode) ||
             (Clear3bits(opcode) == JAI_opcode) );
}

bool Decoder::isSkipInstruction(uint8_t opcode) const
{
	return ( (opcode == SGT_opcode) ||
             (opcode == SLT_opcode) ||
             (opcode == SEQ_opcode) ||
             (opcode == SAZ_opcode) ||
             (opcode == SLE_opcode) ||
             (opcode == SGE_opcode) ||
             (opcode == SNE_opcode) ||
             (opcode == SAN_opcode) ||
             (opcode == SES_opcode) ||
             (opcode == SEZ_opcode) ||
             (Clear3bits(opcode) == SBS_opcode) ||
             (Clear3bits(opcode) == SFS_opcode) ||
             (Clear3bits(opcode) == SBZ_opcode) ||
             (Clear3bits(opcode) == SFZ_opcode) );
}

uint16_t Decoder::DirectAddress(uint8_t opcode, uint8_t parameter) const
{
    uint16_t address = (uint16_t)(parameter);       // second byte = offset
    address |= ((uint16_t)Mask3bits(opcode)) << 8;  // opcode 3 bits = page
    return address;
}

/// @brief Append string representing a simple decimal number
void Decoder::AppendNumberString(uint8_t x, std::string &out)
{
	std::stringstream stream;
    stream << std::setw(1) << std::dec << (int) x;
    out.append(stream.str());
}

/// @brief Append string representing a byte number in hex or octal
void Decoder::AppendByteString(uint8_t x, std::string &out)
{
	std::stringstream stream;
    stream << std::setfill ('0');
    
    // Set stream numeric mode and padding size
	if (m_hex)
	{
		stream << std::uppercase << std::setw(2) << std::hex;
	}
	else
	{
		stream << std::setw(3) << std::oct;
	}
    
    stream << (int) x;
    out.append(stream.str());
}

/// @brief Append string representing a double byte number in hex or octal
void Decoder::AppendAddressString(uint16_t x, std::string &out)
{
	std::stringstream stream;
	stream << std::setfill ('0') << std::setw(4);
    
    // Set stream numeric mode
	if (m_hex)
	{
		stream << std::uppercase << std::hex;
	}
	else
	{
		stream << std::oct;
	}
    
    stream << (int) x;
    out.append(stream.str());
}

//------------------------------------------------------------
// Instruction Set. Grouped by opcode type
//------------------------------------------------------------

// One byte instructions. Simple opcodes
const std::vector<Instruction> Decoder::SimpleDirectOpCode =
{
    {INB_opcode, "INB", "Increment ACC (binary)"},
    {DEB_opcode, "DEB", "Decrement ACC (binary)"},
    {IND_opcode, "IND", "Increment ACC (BCD)"},
    {DED_opcode, "DED", "Decrement ACC (BCD)"},
    {CLA_opcode, "CLA", "Clear ACC"},
    {CMA_opcode, "CMA", "Complement ACC"},
    {LSA_opcode, "LSA", "Left shift ACC"},
    {RSA_opcode, "RSA", "Right shift ACC"},
    {SGT_opcode, "SGT", "Skip on ACC > R0"},
    {SLT_opcode, "SLT", "Skip on ACC < R0"},
    {SEQ_opcode, "SEQ", "Skip on ACC = R0"},
    {SAZ_opcode, "SAZ", "Skip on ACC = 0"},
    {SLE_opcode, "SLE", "Skip on ACC <= R0"},
    {SGE_opcode, "SGE", "Skip on ACC >= R0"},
    {SNE_opcode, "SNE", "Skip on ACC != R0"},
    {SAN_opcode, "SAN", "Skip on ACC != 0"},
    {SES_opcode, "SES", "Skip on Extend flag set"},
    {ENI_opcode, "ENI", "Enable Interrupt"},
    {SEZ_opcode, "SEZ", "Skip on Extend flag clear"},
    {NOP_opcode, "NOP", "No operation"},
    {DSI_opcode, "DSI", "Disable Interrupt"},
    {RTI_opcode, "RTI", "Return from Interrupt"},
    {RTE_opcode, "RTE", "Return from Interrupt and enable Interrupt"},
    {STE_opcode, "STE", "Set Extend flag"},
    {CLE_opcode, "CLE", "Clear Extend flag"},
    {RTS_opcode, "RTS", "Return from subroutine"},
    {RSE_opcode, "RSE", "Return from subroutine and enable Interrupt"}
};

// Two bytes instruction. Simple opcode and data
const Instruction Decoder::DoubleDirectOpCode =
    {LDR_opcode, "LDR  ", "Load ACC"};

// Two bytes instructions. 3-bit page in opcode and offset
const std::vector<Instruction> Decoder::DoublePagedOpCode = 
{
    {JMP_opcode, "JMP  L_", "Unconditional Jump"},
    {JSB_opcode, "JSB  L_", "Unconditional Jump to subroutine"}
};

// One byte instructions. 3-bit operand in opcode: bit, DC, indirect indexing
const std::vector<Instruction> Decoder::Single3bitOpCode =
{
    {SBS_opcode, "SBS  ", "Skip on ACC bit set"},
    {SFS_opcode, "SFS  DC", "Skip on Direct Control line set"},
    {SBN_opcode, "SBN  ", "Set ACC bit"},
    {STC_opcode, "STC  DC", "Set Direct Control line"},
    {SBZ_opcode, "SBZ  ", "Skip on ACC bit clear"},
    {SFZ_opcode, "SFZ  DC", "Skip on Direct Control line clear"},
    {JAI_opcode, "JAI  ", "Jump indirect indexed"},
    {JAS_opcode, "JAS  ", "Jump indirect indexed to subroutine"},
    {CBN_opcode, "CBN  ", "Clear ACC bit"},
    {CLC_opcode, "CLC  DC", "Clear Direct Control line"}
};

// One byte instruction. 4-bit operand in opcode: R, DS, Z
const std::vector<Instruction> Decoder::Single4bitOpCode =
{
    {INA_opcode, "INA  DS", "Input from Device to ACC"},
    {OTA_opcode, "OTA  DS", "Output ACC to Device"},
    {LDA_opcode, "LDA  R", "Load ACC from Register"},
    {STA_opcode, "STA  R", "Store ACC at Register"},
    {LDI_opcode, "LDI  ", "Load ACC from indexed Register"},
    {STI_opcode, "STI  ", "Store ACC at indexed Register"}
};

// Two bytes instructions. 4-bit operand in opcode and data: R, DS (OTR, STR)
const std::vector<Instruction> Decoder::Double4bitOpCode =
{
    {OTR_opcode, "OTR  DS", "Output data to Device"},
    {STR_opcode, "STR  R", "Store data at Register"}
};
