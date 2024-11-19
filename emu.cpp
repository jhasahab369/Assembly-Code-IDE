#include <bits/stdc++.h>
using namespace std;

// humble digit check
bool isDigit(char c) {
    switch (c) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return true;
        default:
            return false;
    }
}

// humble character check
bool isAlphabet(char c) {
    char lowerChar = tolower(c);
    const char alphabets[] = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; alphabets[i] != '\0'; ++i) {
        if (lowerChar == alphabets[i]) {
            return true;
        }
    }
    return false;
}

// label quality control
bool isValidLabel(string labelName) {
    bool isLabelValid = isAlphabet(labelName[0]);  
    
    for (size_t i = 1; i < labelName.length(); ++i) {
        char currentChar = labelName[i];
        isLabelValid &= (isalnum(currentChar) || currentChar == '_');
    }
    return isLabelValid;
}

// If it looks like a decimal, walks like a decimal... it just might be a decimal
bool isDecimal(string str) {
    for (char c : str) {
        if (!isDigit(c)) return false;  // All characters must be digits
    }
    return true;
}


bool isOctal(string octalNumber) {
    // Octal numbers must start with '0' and contain only digits '0' to '7'
    if (octalNumber.size() < 2 || octalNumber[0] != '0') {
        return false;
    }
    for (size_t i = 1; i < octalNumber.size(); ++i) {
        if (octalNumber[i] < '0' || octalNumber[i] > '7') {
            return false;
        }
    }
    return true;
}

bool isHexadecimal(string hexNumber) {
    // Hex numbers start with '0x' or '0X' and are followed by valid hex digits
    if (hexNumber.size() < 3 || hexNumber[0] != '0' || tolower(hexNumber[1]) != 'x') {
        return false;
    }
    for (size_t i = 2; i < hexNumber.size(); ++i) {
        if (!isxdigit(hexNumber[i])) {
            return false;  // Each character must be a valid hex digit
        }
    }
    return true;
}

// Convert an octal string to a decimal string
string octalToDecimal(string octalNumber) {
    int decimalResult = 0;
    int powerOfEight = 1;

    for (int i = octalNumber.size() - 1; i >= 0; --i) {
        int digit = octalNumber[i] - '0';
        decimalResult += digit * powerOfEight;
        powerOfEight *= 8;
    }
    return to_string(decimalResult);  // Turn the final result back into a string
}

string hexToDecimal(string hexNumber) {
    int decimalResult = 0;
    int powerOfSixteen = 1;

    for (int i = hexNumber.size() - 1; i >= 0; --i, powerOfSixteen *= 16) {
        int digitValue;
        if (isdigit(hexNumber[i])) {
            digitValue = hexNumber[i] - '0';
        } else {
            digitValue = tolower(hexNumber[i]) - 'a' + 10;
        }
        decimalResult += powerOfSixteen * digitValue;
    }
    return to_string(decimalResult);  // decimal debut!
}

// A quick check to see if this string is decimal or just pretending to be
bool decimalCheck(string value) {
    if (value.empty()) return false;

    size_t start = (value[0] == '+' || value[0] == '-') ? 1 : 0;

    return start < value.length() && all_of(value.begin() + start, value.end(), ::isdigit);
}

bool octalCheck(string value) {
    return (value.length() > 1 && value[0] == '0' &&
            all_of(value.begin() + 1, value.end(), [](char c) { return c >= '0' && c <= '7'; }));
}

bool digitsCheck(const string& temp) {
    size_t start_index = 0;

    if (temp.size() >= 2 && temp[0] == '0' && (temp[1] == 'x' || temp[1] == 'X')) start_index = 2; // Hexadecimal
    else if (temp.size() == 1 && temp[0] == '0') return true;  // Single "0" is valid
    else if (temp.size() > 1 && temp[0] == '0') start_index = 1;  // Octal

    // Check each remaining character to see if it’s a valid digit or hex digit
    return all_of(temp.begin() + start_index, temp.end(), [](char c) {
        return isdigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    });
}

// label quality checker (again)
bool labelCheck(string label) {
    if (!label.empty() && label.back() == ':') {
        char firstChar = label[0];
        return isalpha(firstChar);
    }
    return false;
}

// Check if an operand exists in the operand table
bool operandCheck(string operand, unordered_map<string, int> operandTable) {
    return operandTable.count(operand) > 0;
}

// Verify if a string is a recognized operand type
bool operandType(const string& x) {
    unordered_set<string> validOperands{"data", "ldc", "adc", "adj", "SET"};
    return validOperands.count(x) > 0;
}


bool operandOne(string temp) {
    unordered_set<string> validInstructions{"data", "ldc", "adc", "ldl", "stl", "ldnl", "stnl",
                                            "10", "call", "brz", "brlz", "br", "SET", "adj"};
    return validInstructions.count(temp) > 0;
}

// Check if an instruction requires zero operands
// For the more independent, operand-free instructions
bool operandZero(string temp) {
    unordered_set<string> validInstructions{"add", "sub", "shl", "shr", "a2sp", "sp2a", "return", "HALT"};
    return validInstructions.count(temp) > 0;
}

vector<int> machineCodeInstructions;  // Stores the machine code instructions
int memorySpace[1 << 24];             // Memory array with a size of 2^24
int accumulator, registerB, programCounter, stackPointer, lineCounter = 0;  // Main registers
pair<int, int> readWriteLog;          // Tracks read/write actions with address and value pairs

// ISA-defined operations

// Load a constant value into the accumulator, shifting previous accumulator to registerB
void loadConstant(int value) {
    registerB = accumulator;
    accumulator = value;
}

// Add a constant value to the accumulator
void addConstant(int value) {
    accumulator += value;
}

// Load value from memory at (stackPointer + offset) into accumulator; shift current accumulator to registerB
void loadLocal(int offset) {
    registerB = accumulator;
    accumulator = memorySpace[stackPointer + offset];
    readWriteLog = {stackPointer + offset, 0};  // Track read address in readWriteLog
}

// Store accumulator into memory at (stackPointer + offset); restore accumulator from registerB
void storeLocal(int offset) {
    readWriteLog = {stackPointer + offset, memorySpace[stackPointer + offset]};  // Track previous value at address
    memorySpace[stackPointer + offset] = accumulator;
    accumulator = registerB;
}

// Load value from memory at (accumulator + offset) into accumulator
void loadNonLocal(int offset) {
    accumulator = memorySpace[accumulator + offset];
    readWriteLog = {accumulator + offset, 0};  // Track read address in readWriteLog
}

// Store registerB into memory at (accumulator + offset)
void storeNonLocal(int offset) {
    readWriteLog = {accumulator + offset, memorySpace[accumulator + offset]};  // Track previous value at address
    memorySpace[accumulator + offset] = registerB;
}

// Add accumulator and registerB, store result in accumulator
void add(int) {
    accumulator += registerB;
}

// Subtract accumulator from registerB, store result in accumulator
void subtract(int) {
    accumulator = registerB - accumulator;
}

// Shift registerB left by the value of accumulator, store result in accumulator
void shiftLeft(int) {
    accumulator = registerB << accumulator;
}
// Shift registerB right by the value of accumulator, store result in accumulator
void shiftRight(int) {
    accumulator = registerB >> accumulator;
}
// Adjust stack pointer by the specified value
void adjustStackPointer(int value) {
    stackPointer += value;
}
// Set stack pointer to accumulator, shift registerB to accumulator
void accumulatorToStackPointer(int) {
    stackPointer = accumulator;
    accumulator = registerB;
}
// Set accumulator to stack pointer, shift accumulator to registerB
void stackPointerToAccumulator(int) {
    registerB = accumulator;
    accumulator = stackPointer;
}
// Store program counter in accumulator, update program counter by offset (jump to subroutine)
void callFunction(int offset) {
    registerB = accumulator;
    accumulator = programCounter;
    programCounter += offset;
}
// Set program counter to accumulator (return from subroutine), shift accumulator to registerB
void returnFunction(int) {
    programCounter = accumulator;
    accumulator = registerB;
}
// If accumulator is zero, update program counter by offset
void branchIfZero(int offset) {
    if (accumulator == 0) {
        programCounter += offset;
    }
}
// If accumulator is negative, update program counter by offset
void branchIfLessThanZero(int offset) {
    if (accumulator < 0) {
        programCounter += offset;
    }
}
// Update program counter by offset
void unconditionalBranch(int offset) {
    programCounter += offset;
}
// stop
void haltExecution(int) {
    return;
}
// Information to call respective function for each operation
vector<string> operationMnemonics = {"ldc", "adc", "ldl", "stl", "ldnl", "stnl", "add", "sub", 
                                     "shl", "shr", "adj", "a2sp", "sp2a", "call", "return", "brz", "brlz", "br", "HALT"};
// Function pointers for each operation, matching the mnemonics order
void (*operationFunctions[])(int value) = {loadConstant, addConstant, loadLocal, storeLocal, loadNonLocal, storeNonLocal, add, 
                                           subtract, shiftLeft, shiftRight, adjustStackPointer, accumulatorToStackPointer, 
                                           stackPointerToAccumulator, callFunction, returnFunction, branchIfZero, 
                                           branchIfLessThanZero, unconditionalBranch, haltExecution};

// Operand table mapping mnemonics to opcodes and operand types
map<string, pair<string, int>> operandTable;  // {mnemonic, {opcode, operandType}}
string decimalToHex(int decimalNumber) {
    unsigned int number = decimalNumber;
    string hexResult = "";
    for (int i = 0; i < 8; ++i, number /= 16) {
        int remainder = number % 16;
        hexResult += (remainder <= 9) ? char(remainder + '0') : char(remainder - 10 + 'A');
    }
    reverse(hexResult.begin(), hexResult.end());
    return hexResult;
}
// Error handling function for runtime issues (segmentation faults, infinite loops, etc.)
void throwError() {
    cout << "Segmentation Fault (or possible infinite loop)" << endl;
    cout << "Please check your code for issues." << endl;
    exit(0);
}
// Print current values of the main registers (program counter, stack pointer, accumulator, registerB)
void traceRegisters() {
    printf("PC = %08X, SP = %08X, A = %08X, B = %08X ", programCounter, stackPointer, accumulator, registerB);
}
// Reads operation trace and shows the last read address and current value in the accumulator
void traceReadOperation() {
    cout << "Reading memory[" << decimalToHex(readWriteLog.first) << "] finds " << decimalToHex(accumulator) << endl;
}
// Writes operation trace and shows last write address, previous value, and updated value in memory
void traceWriteOperation() {
    cout << "Writing memory[" << decimalToHex(readWriteLog.first) << "] was " 
         << decimalToHex(readWriteLog.second) << " now " << decimalToHex(memorySpace[readWriteLog.first]) << endl;
}
// Display memory contents before execution
void memoryDumpBeforeExecution() {
    cout << "Memory dump before execution:" << endl;
    for (int i = 0; i < (int)machineCodeInstructions.size(); i += 4) {
        cout << decimalToHex(i) << " ";
        for (int j = i; j < min((int)machineCodeInstructions.size(), i + 4); ++j) {
            cout << decimalToHex(machineCodeInstructions[j]) << " ";
        }
        cout << endl;
    }
}
// Display memory contents after execution
void memoryDumpAfterExecution() {
    cout << "Memory dump after execution:" << endl;
    for (int i = 0; i < (int)machineCodeInstructions.size(); i += 4) {
        cout << decimalToHex(i) << " ";
        for (int j = i; j < min((int)machineCodeInstructions.size(), i + 4); ++j) {
            cout << decimalToHex(memorySpace[j]) << " ";
        }
        cout << endl;
    }
}
// Reset all primary registers (accumulator, registerB, stackPointer, programCounter) to zero
void resetRegisters() {
    accumulator = registerB = stackPointer = programCounter = 0;
}
// Display the instruction set
void displayInstructionSet() {
    cout << "Opcode  Mnemonic  Operand" << '\n';
    cout << "0       ldc       value" << "\n";
    cout << "1       adc       value" << "\n";
    cout << "2       ldl       value" << "\n";
    cout << "3       stl       value" << "\n";
    cout << "4       ldnl      value" << "\n";
    cout << "5       stnl      value" << "\n";
    cout << "6       add             " << "\n";
    cout << "7       sub             " << "\n";
    cout << "9       shr             " << "\n";
    cout << "10      adj       value " << "\n";
    cout << "11      a2sp            " << "\n";
    cout << "12      sp2a            " << "\n";
    cout << "13      call      offset" << "\n";
    cout << "14      return          " << "\n";
    cout << "15      brz       offset" << "\n";
    cout << "16      brlz      offset" << "\n";
    cout << "17      br        offset" << "\n";
    cout << "18      HALT            " << "\n";
    cout << "         SET       value " << "\n";
}
// Execute each decoded instruction
int executionCount = 0;
bool executeInstruction(int currentLine, int traceFlag) {
    int opcode = (currentLine & 0xFF);
    int value = (currentLine - opcode) >> 8;
    ++lineCounter;
    operationFunctions[opcode](value);  // Call the respective operation function
    int prevPC = programCounter;

    // Boundary and infinite loop checks
    if ((programCounter < 0) || (programCounter > (int)machineCodeInstructions.size()) || (lineCounter > (1 << 24))) {
        throwError();
        return true;
    }

    if (traceFlag == 0) {
        traceRegisters();
        cout << operationMnemonics[opcode] << " ";
        if (operandTable[operationMnemonics[opcode]].second > 0) {
            cout << decimalToHex(value);
        }
        cout << endl;

        if (opcode == 17 && value == -1) {
            if (executionCount == 0) {
                executionCount = 1;
            } else {
                cout << "Infinite Loop Detected!" << endl;
                return true;
            }
        }
    } else if (traceFlag == 1 && (opcode == 2 || opcode == 4)) {
        traceReadOperation();
    } else if (traceFlag == 2 && (opcode == 3 || opcode == 5)) {
        traceWriteOperation();
    }

    if (opcode >= 18) {
        return true;
    }
    return false;
}
// Execute all instructions line by line
void runProgram(int traceFlag) {
    while (programCounter < (int)machineCodeInstructions.size()) {
        int currentLine = machineCodeInstructions[programCounter++];
        bool shouldQuit = executeInstruction(currentLine, traceFlag);
        if (shouldQuit) break;
    }
}
// Parse and execute commands
void executeCommand(string command) {
    if (command == "-trace") {
        runProgram(0);
        cout << "Program execution finished!" << endl;
    } else if (command == "-read") {
        runProgram(1);
    } else if (command == "-write") {
        runProgram(2);
    } else if (command == "-before") {
        runProgram(3);
        memoryDumpBeforeExecution();
    } else if (command == "-after") {
        runProgram(3);
        memoryDumpAfterExecution();
    } else if (command == "-wipe") {
        resetRegisters();
    } else if (command == "-isa") {
        displayInstructionSet();
    } else {
        cout << "Invalid command" << endl;
        exit(0);
    }
    cout << lineCounter << " instructions executed" << endl;
}

// Reading machine code instructions from a binary file
void loadMachineCode(string filename) {
    ifstream file(filename, ios::in | ios::binary);
    unsigned int currentInstruction;
    int memoryAddress = 0;

    while (file.read((char*)&currentInstruction, sizeof(int))) { 
        machineCodeInstructions.push_back(currentInstruction);
        memorySpace[memoryAddress++] = currentInstruction;
    }
}
// Main program function to handle command-line arguments
int main(int argc, char* argv[]) {
    if (argc <= 2) {
        cout << "Invalid Format of the command , please retry!" << endl;
        cout << "Expected format: ./emu [command] filename.o" << endl;
        cout << "commands you should try instead :" << endl;
        cout << "-trace  show instruction trace" << endl;
        cout << "-read   show memory reads" << endl;
        cout << "-write  show memory writes" << endl;
        cout << "-before show memory dump before execution" << endl;
        cout << "-after  show memory dump after execution" << endl;
        cout << "-wipe   reset registers before execution" << endl;
        cout << "-isa    display ISA" << endl;
        return 0;
    }
    string filename = argv[2];    
    operandTable["data"] = {"", 1};
    operandTable["ldc"]  = {"00", 1};
    operandTable["adc"]  = {"01", 1};
    operandTable["ldl"]  = {"02", 2};
    operandTable["stl"]  = {"03", 2};
    operandTable["ldnl"] = {"04", 2};
    operandTable["stnl"] = {"05", 2};
    operandTable["add"]  = {"06", 0};
    operandTable["sub"]  = {"07", 0};
    operandTable["shl"]  = {"08", 0};
    operandTable["shr"]  = {"09", 0};
    operandTable["adj"]  = {"0A", 1};
    operandTable["a2sp"] = {"0B", 0};
    operandTable["sp2a"] = {"0C", 0};
    operandTable["call"] = {"0D", 2};
    operandTable["return"] = {"0E", 0};
    operandTable["brz"]  = {"0F", 2};
    operandTable["brlz"] = {"10", 2};
    operandTable["br"]   = {"11", 2};
    operandTable["HALT"] = {"12", 0};
    operandTable["SET"]  = {"", 1};    

    loadMachineCode(filename);
    string command = argv[1];
    executeCommand(command);
    return 0;
}

