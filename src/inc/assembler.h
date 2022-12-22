#include <array>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <tuple>

#include "opcode.h"
#include "register.h"

enum AssemblerReturn : int
{
    SUCCESS = 0,
    CANNOT_OPEN_FILE,
    UNKNOWN_OPCODE
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

    auto hexIndex = literal.find(hexLiteral);
    if (hexIndex != std::string::npos)
    {
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
        ssLiteral >> signedValue;
        return {true, std::reinterpret_cast<uint16_t>(signedValue)};
    }
    ssLiteral >> unsignedValue;
    return {true, unsignedValue};
}