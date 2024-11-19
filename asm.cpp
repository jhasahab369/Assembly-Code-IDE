#include <bits/stdc++.h>
using namespace std; 

// Struct to store error information in the assembly process
struct ErrorInfo {
    int location;  // To store the location of the error in code
    string msg;    // Error message which will describe the issue

    // Comparison operator to allow sorting by error location
    bool operator<(const ErrorInfo &other) const {
        return location < other.location;
    }
};

// Struct to store warning information in the assembly processes
struct WarningInfo {
    int location; // same as Error Info Variable definitions 
    string msg;   

    // to allow sorting by warning location like in Error Info where it was through error location
    bool operator<(const WarningInfo &other) const {
        return location < other.location;
    }
};

// Struct to represent information about each line in the assembly code
struct LineInfo {
    int pctr;          // Program counter for each line
    string label;      // Label for the line (if there is any)
    string mnemonic;   // Instruction mnemonic (eg-- ADD, SUB)
    string operand;    // Operand associated with the mnemonic in question
};

// Struct to represent listing information after assembly pass
struct ListInfo {
    string address;   // Address generated for each line 
    string macCode;   // The generated machine code
    string statement; // Original statement from source file
};

// Bunch of Error Declarations to store stuff
vector<ErrorInfo> errorList;             // List of error information
vector<WarningInfo> warningList;         // List of warning information
vector<LineInfo> lines;                  // Line by line information for source code
vector<ListInfo> listingEntries;         // Information for listing output after assembly (imp for .lst file)
vector<string> machineCode;              // Machine codes in 8-bit hexadecimal format

// Maps for labels, operands, variable values, and line-by-line usage of labels
map<string, pair<int, int>> labelInfo;         // Maps each label to its {address, line number}
map<string, pair<string, int>> operandTable;   // Maps each instruction to its {opcode, operand type}
map<string, vector<int>> labelLineUsage;       // Maps labels to line numbers where they are used
map<string, string> variableValues;            // Maps variables (labels) to their associated values

vector<string> instructionLines;               // Stores each line of instruction from input

bool isDigit(char c) {
    switch (c) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            return true;
        default:
            return false;
    }
}

bool isAlphabet(char c) {
    char lowerChar = tolower(c);         // Normalize to lowercase
    const char alphabet[] = "abcdefghijklmnopqrstuvwxyz";
    for (int i = 0; alphabet[i] != '\0'; ++i) {
        if (lowerChar == alphabet[i]) {
            return true;
        }
    }
    return false;
}

// check to see if a string can be classified and used as a Label 
bool isValidLabel(string labelName) {
    bool isLabelValid = isAlphabet(labelName[0]);  // Must start with an alphabet
    for (size_t i = 1; i < labelName.length(); ++i) {
        isLabelValid &= (isalnum(labelName[i]) || labelName[i] == '_');  // Must be alphanumeric or underscore
    }
    return isLabelValid;
}

// to check if a string represents a decimal number
bool isDecimal(string str) {
    for (char c : str) {
        if (!isDigit(c)) return false;   // All characters must be digits
    }
    return true;
}

// to check if a string represents an octal number
bool isOctal(string octalNumber) {
    if (octalNumber.size() < 2 || octalNumber[0] != '0') {
        return false;                    // Octal numbers must start with '0'
    }
    for (size_t i = 1; i < octalNumber.size(); ++i) {
        if (octalNumber[i] < '0' || octalNumber[i] > '7') {
            return false;                // Each character must be in the range '0' to '7'
        }
    }
    return true;
}

// to check if a string represents a hexadecimal number
bool isHexadecimal(string hexNumber) {
    if (hexNumber.size() < 3 || hexNumber[0] != '0' || tolower(hexNumber[1]) != 'x') {
        return false;                    // Hexadecimal numbers must start with '0x' or '0X'
    }
    for (size_t i = 2; i < hexNumber.size(); ++i) {
        if (!isxdigit(hexNumber[i])) {
            return false;                // Remaining characters must be valid hexadecimal digits
        }
    }
    return true;
}

string octalToDecimal(string octalNumber) {
    int decimalResult = 0;
    int powerOfEight = 1;

    // Iterate through the octal string from right to left
    for (int i = octalNumber.size() - 1; i >= 0; --i) {
        int digit = octalNumber[i] - '0';   // Convert character to integer
        decimalResult += digit * powerOfEight;  // Update result by digit * 8^position
        powerOfEight *= 8;  // Update power of 8 for the next position
    }

    return to_string(decimalResult);  // Convert integer result to string and return
}

string hexToDecimal(string hexNumber) {
    int decimalResult = 0;
    int powerOfSixteen = 1;

    // Iterate through the hexadecimal string from right to left
    for (int i = hexNumber.size() - 1; i >= 0; --i, powerOfSixteen *= 16) {
        int digitValue;
        if (isdigit(hexNumber[i])) {
            digitValue = hexNumber[i] - '0';  // Convert numeric characters
        } else {
            digitValue = tolower(hexNumber[i]) - 'a' + 10;  // Convert alphabetic hex digits
        }
        decimalResult += powerOfSixteen * digitValue;  // Update result by digit * 16^position
    }

    return to_string(decimalResult);  // Convert integer result to string and return
}


string decimalToHex(int decimalNumber) {
    unsigned int number = decimalNumber;  // Use unsigned for consistency
    string hexResult = "";

    // Process each hexadecimal digit (up to 8 digits for 32-bit compatibility)
    for (int i = 0; i < 8; ++i, number /= 16) {
        int remainder = number % 16;
        char hexChar;

        // Convert remainder to appropriate hex character
        if (remainder <= 9) {
            hexChar = char(remainder + '0');  // Numeric characters
        } else {
            hexChar = char(remainder - 10 + 'A');  // Alphabetic characters (A-F)
        }
        hexResult += hexChar;
    }

    reverse(hexResult.begin(), hexResult.end());  // Reversing to get correct hexadecimal order
    return hexResult;
}

// Function to extract the name of a file (excluding its extension) from a given file path
string getFileName(string filePath) {
    string fileName = "";
    int dotPosition = -1;

    // Find the position of the first '.' character (indicating start of file extension)
    for (int i = 0; i < filePath.length(); i++) {
        if (filePath[i] == '.') {
            dotPosition = i;
            break;
        }
    }

    // Append characters up to the position of the '.' to create the file name
    for (int i = 0; i < dotPosition; i++) {
        fileName += filePath[i];
    }

    return fileName;
}

int main(int argc, char **argv)
{

    if (argc != 2) {
        cout << "Incorrect File Naming Format\n";
        return 0;
    }

    string fileName = getFileName(argv[1]);
    ifstream inputFile(argv[1]);

    if (!inputFile.is_open()) {
        cout << "File is Empty\n";
        return 0;
    }

    string instruction;
    while (getline(inputFile, instruction)) {
        instructionLines.push_back(instruction);
    }
    inputFile.close();

    // Initializing the operand table with opcodes and operand types
    operandTable["data"] = {"", 1};
    operandTable["ldc"] = {"00", 1};
    operandTable["adc"] = {"01", 1};
    operandTable["ldl"] = {"02", 2};
    operandTable["stl"] = {"03", 2};
    operandTable["ldnl"] = {"04", 2};
    operandTable["stnl"] = {"05", 2};
    operandTable["add"] = {"06", 0};
    operandTable["sub"] = {"07", 0};
    operandTable["shl"] = {"08", 0};
    operandTable["shr"] = {"09", 0};
    operandTable["adj"] = {"0A", 1};
    operandTable["a2sp"] = {"0B", 0};
    operandTable["sp2a"] = {"0C", 0};
    operandTable["call"] = {"0D", 2};
    operandTable["return"] = {"0E", 0};
    operandTable["brz"] = {"0F", 2};
    operandTable["brlz"] = {"10", 2};
    operandTable["br"] = {"11", 2};
    operandTable["HALT"] = {"12", 0};
    operandTable["SET"] = {"", 1};

    int lineNumber = 0;
    int programCounter = 0;
    bool haltFlag = false;

    for (int i = 0; i < instructionLines.size(); i++) {
        lineNumber++;
        string current = instructionLines[i];
        if (current.empty()) continue;

        vector<string> tokens;
        stringstream lineStream(current);
        string segment;

        // Tokenize the line while handling labels and comments
        while (lineStream >> segment) {
            if (segment == "HALT") haltFlag = true;

            if (!segment.empty()) {
                if (segment[0] != ';') {  /* check for fixing the error where the assembler wasnt able to figure the comments in the assembly file */
                    if (segment.back() != ':') {
                        if (segment.find(':') != string::npos) {
                            int colonPosition = -1;
                            for (int j = 0; j < segment.size(); j++) {
                                if (segment[j] == ':') {
                                    colonPosition = j;
                                    break;
                                }
                            }
                            tokens.push_back(segment.substr(0, colonPosition + 1));
                            segment = segment.substr(colonPosition + 1);
                        }
                    }
                    if (segment.back() == ';') {
                        segment.pop_back();  // Remove trailing semicolon
                        tokens.push_back(segment);
                        break;
                    }
                    tokens.push_back(segment);
                } else {
                    break;  // If a comment is detected, ignore the rest of the current line
                }
            }
        }

        string operand = "";
        string operation = "";
        string label = "";
        if (operation == "HALT" || label == "HALT" || operand == "HALT") {
            haltFlag = true;
        }
        int tokenIndex = 0;

        if (tokens.empty()) continue;

        // Check if the first token is a label
        if (tokens[tokenIndex].back() == ':') {
            label = tokens[tokenIndex];
            label.pop_back();  // Remove trailing colon from label
            tokenIndex++;
        }

        // Assign operation mnemonic if present
        if (tokenIndex < tokens.size()) {
            operation = tokens[tokenIndex];
            tokenIndex++;
        }

        // Assign operand if present
        if (tokenIndex < tokens.size()) {
            operand = tokens[tokenIndex];
            tokenIndex++;
        }

        // Validate and store label information if a label is identified
        if (!label.empty()) {
            if (!isValidLabel(label)) {
                errorList.push_back({lineNumber, "Invalid Label Name"});
            } else {
                if (labelInfo.count(label)) {
                    if (labelInfo[label].first != -1) {
                        errorList.push_back({lineNumber, "Duplicate Label used"});
                    }
                }
                if (!labelInfo.count(label) || labelInfo[label].first == -1) {
                    labelInfo[label] = {programCounter, lineNumber};
                }
            }
        }

        bool flag = false;

        if (!operation.empty()) {
            if (operandTable.count(operation)) {
                if (operandTable[operation].second == 1 || operandTable[operation].second == 2) {
                    if (operand.empty()) {
                        errorList.push_back({lineNumber, "Operand is missing"});
                    } else if (tokens.size() - tokenIndex > 0) {
                        errorList.push_back({lineNumber, "Extra Operand present"});
                    } else {
                        string check = "";

                        if (!isValidLabel(operand)) {
                            string tempOperand = operand;
                            if (operand[0] == '+') {
                                check += '+';
                                tempOperand = operand.substr(1);
                            }
                            if (operand[0] == '-') {
                                check += '-';
                                tempOperand = operand.substr(1);
                            }

                            bool isOctalFlag = isOctal(tempOperand);
                            bool isHexFlag = isHexadecimal(tempOperand);
                            bool isDecFlag = isDecimal(tempOperand);
                            string convertedValue = "";

                            if (isOctalFlag) {
                                convertedValue = octalToDecimal(tempOperand.substr(1));
                                check += convertedValue;
                            } else if (isHexFlag) {
                                convertedValue = hexToDecimal(tempOperand.substr(2));
                                check += convertedValue;
                            } else if (isDecFlag) {
                                check += tempOperand;
                            } else {
                                check = "";
                            }
                        } else {
                            check = operand;

                            // Track the label usage or define a new entry if it doesn't exist
                            if (labelInfo.count(operand)) {
                                labelLineUsage[operand].push_back(lineNumber);
                            } else {
                                labelInfo[operand] = {-1, lineNumber};
                                labelLineUsage[operand].push_back(lineNumber);
                            }
                        }
                        string checkedOperand = check;
                        if (!checkedOperand.empty()) {
                            operand = checkedOperand;
                            flag = true;
                        } else {
                            errorList.push_back({lineNumber, "Invalid Label Format"});
                        }
                    }
                } else {
                    if (operandTable[operation].second == 0) {
                        if (operand.empty()) {
                            flag = true;
                        } else {
                            errorList.push_back({lineNumber, "Extra Operand in zero operand operation"});
                        }
                    }
                }
            } else {
                errorList.push_back({lineNumber, "Invalid Operation Name"});
            }
        }

        // Added line information to lines vector
        lines.push_back({programCounter, label, operation, operand});
        programCounter += flag;

        // for handling "SET" operation specifically
        if (flag) {
            if (operation == "SET") {
                if (!label.empty()) {
                    variableValues[label] = operand;
                } else {
                    errorList.push_back({lineNumber, "Missing label name"});
                }
            }
        }
    }


    /* 

       This si the fix for the label errors I was getting in this ------
        ran a simple for loop through labelInfo
        Each label represents a pair<string, pair<int, int>> where-
        label.first is the label name.
        label.second.first is the address (or -1 if undeclared).
        label.second.second is the line number where the label is defined.
        

    */


    for (auto& label : labelInfo) {
        if (label.second.first == -1) {
            // If label was not declared, log error for each line where it was used
            for (auto lineNum : labelLineUsage[label.first]) {
                errorList.push_back({lineNum, "Label not declared"});
            }
        } else if (!labelLineUsage.count(label.first)) {
            // If label is declared but never used, log a warning but that wont be an error ofcourse
            warningList.push_back({label.second.second, "Unused Label"});
        }
    }

    // Sorting the errors and warnings by line number 
    //sorting will work without any comparator function , as we just need to sort it by line number, and that is the 
    // first attribute assinged to it
    sort(errorList.begin(), errorList.end());
    sort(warningList.begin(), warningList.end());

    //  For writing errors and warnings to log file
    string errorFileName = fileName + ".log";
    ofstream errorFileOutput(errorFileName);

    for (const auto& warning : warningList) {
        errorFileOutput << "Line: " << warning.location << " WARNING: " << warning.msg << endl;
    }

    if (!haltFlag) {
        errorFileOutput << "WARNING !! : HALT not found" << endl;
    }

    if (!errorList.empty()) {
        for (const auto& error : errorList) {
            errorFileOutput << "Line: " << error.location << " ERROR: " << error.msg << endl;
        }
        errorFileOutput.close();
    } else {
        errorFileOutput << "No errors!" << endl;
        errorFileOutput.close();
    }
    //this is where the second pass starts from ----------------------
    // would proceed if there arent errors -

    int noErrorFlag = 0;
    if (errorList.empty()) {
        noErrorFlag = 1;
    }
    if (noErrorFlag) {
        int i = 0;
        while (i < lines.size()) {
            int programCounter = lines[i].pctr;
            string label = lines[i].label;
            string mnemonic = lines[i].mnemonic;
            string operand = lines[i].operand;

            if (!mnemonic.empty()) {
                if (operandTable[mnemonic].second == 2) {
                    string result = "        ";
                    string hexValue, processedHex;

                    if (labelInfo.count(operand)) {
                        int offset = labelInfo[operand].first - (programCounter + 1);
                        hexValue = decimalToHex(offset);
                        processedHex = "";

                        for (int j = 2; j < hexValue.size(); j++)
                            processedHex += hexValue[j];

                        processedHex += operandTable[mnemonic].first;
                    } else {
                        int numericOperand = stoi(operand);
                        hexValue = decimalToHex(numericOperand);
                        processedHex = "";

                        for (int j = 2; j < hexValue.size(); j++)
                            processedHex += hexValue[j];

                        processedHex += operandTable[mnemonic].first;
                    }

                    result = processedHex;
                    machineCode.push_back(result);

                    // Constructing the listing line
                    string formattedLine = label;
                    if (!label.empty())
                        formattedLine += ": ";
                    formattedLine += mnemonic;
                    if (!mnemonic.empty())
                        formattedLine += " ";
                    formattedLine += operand;

                    string addressHex = decimalToHex(programCounter);
                    listingEntries.push_back({addressHex, result, formattedLine});
                }
                else if (operandTable[mnemonic].second == 1) {
                    if (mnemonic == "data") {
                        string result = "        ";
                        int numericOperand = stoi(operand);
                        result = decimalToHex(numericOperand);
                        machineCode.push_back(result);

                        string formattedLine = label;
                        if (!label.empty())
                            formattedLine += ": ";
                        formattedLine += mnemonic;
                        if (!mnemonic.empty())
                            formattedLine += " ";
                        formattedLine += operand;

                        string addressHex = decimalToHex(programCounter);
                        listingEntries.push_back({addressHex, result, formattedLine});
                    } 
                    else if (mnemonic == "SET") {
                        string result = "        ";
                        int numericOperand = stoi(operand);
                        result = decimalToHex(numericOperand);

                        string formattedLine = label;
                        if (!label.empty())
                            formattedLine += ": ";
                        formattedLine += mnemonic;
                        if (!mnemonic.empty())
                            formattedLine += " ";
                        formattedLine += operand;

                        string addressHex = decimalToHex(programCounter);
                        listingEntries.push_back({addressHex, result, formattedLine});
                    } 
                    else {
                        string result = "        ";
                        string hexValue, processedHex;

                        if (labelInfo.count(operand)) {
                            int operandAddress = labelInfo[operand].first;
                            hexValue = decimalToHex(operandAddress);
                            processedHex = hexValue.substr(2) + operandTable[mnemonic].first;
                        } else {
                            int numericOperand = stoi(operand);
                            hexValue = decimalToHex(numericOperand);
                            processedHex = hexValue.substr(2) + operandTable[mnemonic].first;
                        }

                        result = processedHex;

                        if (variableValues.count(operand)) {
                            int variableValue = stoi(variableValues[operand]);
                            string tempHex = decimalToHex(variableValue);
                            processedHex = tempHex.substr(2) + operandTable[mnemonic].first;
                            result = processedHex;
                        }

                        machineCode.push_back(result);

                        string formattedLine = label;
                        if (!label.empty())
                            formattedLine += ": ";
                        formattedLine += mnemonic;
                        if (!mnemonic.empty())
                            formattedLine += " ";
                        formattedLine += operand;

                        string addressHex = decimalToHex(programCounter);
                        listingEntries.push_back({addressHex, result, formattedLine});
                    }
                }
                else if (operandTable[mnemonic].second == 0) {
                    string result = "        ";
                    result = "000000";
                    result += operandTable[mnemonic].first;
                    machineCode.push_back(result);

                    string formattedLine = label;
                    if (!label.empty())
                        formattedLine += ": ";
                    formattedLine += mnemonic;
                    if (!mnemonic.empty())
                        formattedLine += " ";
                    formattedLine += operand;

                    string addressHex = decimalToHex(programCounter);
                    listingEntries.push_back({addressHex, result, formattedLine});
                }
            } else {
                string result = "        ";
                machineCode.push_back(result);

                string formattedLine = label;
                if (!label.empty())
                    formattedLine += ": ";
                formattedLine += mnemonic;
                if (!mnemonic.empty())
                    formattedLine += " ";
                formattedLine += operand;

                string addressHex = decimalToHex(programCounter);
                listingEntries.push_back({addressHex, result, formattedLine});
            }

            i++;
        }

        // listing files

        string listFileName = fileName + ".lst";
        ofstream listFileOutput(listFileName);
        for (const auto& output : listingEntries) {
            listFileOutput << output.address << " " << output.macCode << " " << output.statement << endl;
        }
        listFileOutput.close();

        // converting and writing machine code file in binary format
        //and exporting the file 
        // unsigned int is used for "consistency" across machine code 
        string machineFileName = fileName + ".o";
        ofstream machineFileOutput(machineFileName, ios::binary | ios::out);
        for (const auto& code : machineCode) {
            if (!code.empty() && code != "        ") {
                unsigned int binaryCode = static_cast<unsigned int>(stoi(hexToDecimal(code)));
                machineFileOutput.write(reinterpret_cast<const char*>(&binaryCode), sizeof(unsigned int));
            }
        }
        machineFileOutput.close();

    }

    return 0;
}



