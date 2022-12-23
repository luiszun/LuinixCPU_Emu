#include "common.h"
#include "opcode.h"
#include "register.h"

enum AssemblerReturn : int
{
    SUCCESS = 0,
    BAD_ARGUMENTS,
    CANNOT_OPEN_FILE,
    UNKNOWN_OPCODE,
    UNKNOWN_REGISTER,
    INVALID_LITERAL
};

bool ValidateRegister(std::string regName)
{
    if (registerMap.count(regName) < 1)
    {
        std::cout << "Error: unrecognized register named '" << regName << "'" << std::endl;
        return false;
    }
    return true;
}

bool ValidateMnemonic(std::string mnemonic)
{
    if (opCodeTable.count(mnemonic) < 1)
    {
        std::cout << "Error: unrecognized mnemonic '" << mnemonic << "'" << std::endl;
        return false;
    }
    return true;
}

std::tuple<bool, uint16_t> GetLiteralValue(std::string literal)
{
    const std::string hexLiteral = "h'";
    uint16_t unsignedValue;
    int16_t signedValue;

    // First check if it's a decimal or hex
    auto hexIndex = literal.find(hexLiteral);
    if (hexIndex != std::string::npos)
    {
        // Hex doesn't accept negatives
        literal.erase(hexIndex, hexLiteral.length());
        if (!std::regex_match(literal, std::regex("[0-9a-fA-F]*")))
        {
            std::cout << "Error: Invalid hex number" << std::endl;
            return {false, 0};
        }

        std::stringstream ssLiteral(literal);
        ssLiteral >> std::hex >> unsignedValue;
        return {true, unsignedValue};
    }
    if (!std::regex_match(literal, std::regex("[0-9]*")))
    {
        std::cout << "Error: Invalid number" << std::endl;
        return {false, 0};
    }

    std::stringstream ssLiteral(literal);
    if (literal.find("-"))
    {
        // Keep the bit ordering
        ssLiteral >> signedValue;
        std::memcpy(&unsignedValue, &signedValue, sizeof(unsignedValue));
        return {true, unsignedValue};
    }
    ssLiteral >> unsignedValue;
    return {true, unsignedValue};
}

uint16_t EncodeInstructionWord(const OpCode &opCode, const std::array<RegisterId, 3> &args)
{
    uint16_t instructionWord = (opCode.opCode << (opCode.argCount * 4));
    for (auto i = 0; i < opCode.argCount; ++i)
    {
        // If we ever hit this, something went wrong.
        assert(opCode.argCount >= (i + 1));

        auto nArgShiftCount = opCode.argCount - (i + 1);
        instructionWord |= (static_cast<uint16_t>(args[i]) << nArgShiftCount);
    }
    return 0;
}