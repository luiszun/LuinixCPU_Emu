#include <algorithm>
#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "opcode.h"
#include "register.h"

enum AssemblerReturn : int
{
    SUCCESS = 0,
    CANNOT_OPEN_FILE,
    UNKNOWN_OPCODE
};

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

        if (opCodeTable.count(mnemonic) < 1)
        {
            std::cout << "Error: unrecognized instruction mnemonic'" << mnemonic << "' on line " << i << std::endl;
            return AssemblerReturn::UNKNOWN_OPCODE;
        }

        const OpCode &opCode{};
        // Now let's see how many operators we'll deal with.
        // Remember that SET, and SET_M are special commands.
        // They are the only 2-word instructions. The second word comes as an argument, so add +1 to its op count.

        const unsigned argCount =
            ((opCode.id == OpCodeId::SET) || (opCode.id == OpCodeId::SET_M)) ? opCode.argCount + 1 : opCode.argCount;

        for (unsigned i = 0; i < argCount; ++i)
        {
            line >> args[i];
        }

        // Most instructions accept just registers. The exeptions are:
        // SET, SET_M [Reg] [Number]
    }

    return AssemblerReturn::SUCCESS;
}