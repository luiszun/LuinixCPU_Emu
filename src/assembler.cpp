#include "assembler.h"
#include "common.h"
#include "register.h"

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        std::cout << "Usage: Assembler <input_file> <output_file>";
        return AssemblerReturn::BAD_ARGUMENTS;
    }
    const std::string inputFileName(argv[1]);
    const std::string outputFileName(argv[2]);
    std::ifstream inputFile(inputFileName);
    std::ofstream outputFile(outputFileName, std::ios::app | std::ios::binary);

    if (!inputFile || !outputFile)
    {
        std::cout << "Error: File does not exist, or cannot be opened" << std::endl;
        return AssemblerReturn::CANNOT_OPEN_FILE;
    }

    std::string stringLine;

    // 1 because we're counting lines to report to the user
    for (unsigned i = 1; std::getline(inputFile, stringLine); ++i)
    {
        // We have at max 3 arguments on any instruction
        std::array<std::string, 3> args;
        std::array<RegisterId, 3> registerArgs;

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
            if (!ValidateRegister(args[0]))
            {
                return AssemblerReturn::UNKNOWN_REGISTER;
            }
            registerArgs[0] = reinterpret_cast<RegisterId>(registerMap[args[0]]);

            auto [validLiteral, literalValue] = GetLiteralValue(args[1]);
            if (!validLiteral)
            {
                return AssemblerReturn::INVALID_LITERAL;
            }

            auto instr = EncodeInstructionWord(opCode, registerArgs);
            outputFile.write(reinterpret_cast<char *>(&instr), sizeof(instr));
            outputFile.write(reinterpret_cast<char *>(&literalValue), sizeof(literalValue));
            // outputFile << instr;
            // outputFile << literalValue;
            continue;
        }

        for (unsigned i = 0; i < opCode.argCount; ++i)
        {
            line >> args[i];
            if (!ValidateRegister(args[i]))
            {
                return AssemblerReturn::UNKNOWN_REGISTER;
            }
            registerArgs[i] = reinterpret_cast<RegisterId>(registerMap[args[i]]);
            auto instr = EncodeInstructionWord(opCode, registerArgs);
            // outputFile << instr;
            outputFile.write(reinterpret_cast<char *>(&instr), sizeof(instr));
        }
    }

    inputFile.close();
    outputFile.close();
    return AssemblerReturn::SUCCESS;
}