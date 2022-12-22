#include <algorithm>
#include <sstream>
#include <string>

#include "assembler.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: Assembler <input_file> <output_file>";
        return AssemblerReturn::CANNOT_OPEN_FILE;
    }
    const std::string inputFileName(argv[1]);
    const std::string outputFileName(argv[2]);
    std::ifstream inputFile(inputFileName);

    if (!inputFile)
    {
        std::cout << "Error: File does not exist, or cannot be opened" << std::endl;
        return AssemblerReturn::SUCCESS;
    }

    std::string stringLine;

    // 1 because we're counting lines to report to the user
    for (unsigned i = 1; std::getline(inputFile, stringLine); ++i)
    {
        // We have at max 3 arguments on any instruction
        std::array<std::string, 3> args;

        // Let's remove any commas first
        std::replace(stringLine.begin(), stringLine.end(), ',', ' ');
        std::stringstream line(stringLine);
        std::string mnemonic;
        line >> mnemonic;

        if (!ValidateMnemonic(mnemonic))
        {
            return AssemblerReturn::UNKNOWN_OPCODE;
        }

        const OpCode &opCode = opCodeTable[mnemonic];

        // Remember that SET, and SET_M are special commands. We'll process them separately
        const bool isSpecialInstruction = (opCode.id == OpCodeId::SET) || (opCode.id == OpCodeId::SET_M);
        if (isSpecialInstruction)
        {
            line >> args[0];
            line >> args[1];
            ValidateRegister(args[0]);
            auto instReg = std::reinterpret_cast<RegisterId>(registerMap[args[0]]);
            continue;
        }

        for (unsigned i = 0; i < opCode.argCount; ++i)
        {
            line >> args[i];
        }

        // Most instructions accept just registers. The exeptions are:
        // SET, SET_M [Reg] [Number]
    }

    return AssemblerReturn::SUCCESS;
}