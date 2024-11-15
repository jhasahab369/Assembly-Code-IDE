# Assembly IDE

A web-based integrated development environment for assembly programming with a built-in assembler and emulator. This project provides a complete environment for writing, assembling, and executing assembly code through an intuitive interface.

## Project Structure

```
assembly-ide/
├── frontend/               # React-based web interface
│   ├── src/
│   ├── public/
│   └── package.json
├── backend/               # Node.js server
│   ├── server.js
│   └── package.json
├── asm.cpp               # Two-pass assembler implementation
├── emu.cpp               # Machine code emulator
└── README.md
```

## Components

### Frontend (Assembly IDE)
- Modern, dark-themed web interface for code editing
- Real-time code editing with syntax highlighting
- Separate panels for code input and execution output
- Integrated buttons for saving, assembling, and emulating code
- Tab system for viewing assembler output and emulator results

### Backend Server (server.js)
- RESTful API handling code compilation and execution
- Manages communication between frontend and native components
- Handles file operations and maintains session state
- Provides endpoints for assembling and emulating code

### Two-Pass Assembler (asm.cpp)
- First pass: Symbol table generation and label resolution
- Second pass: Generates machine code from assembly instructions
- Error detection and reporting
- Support for basic assembly directives
- Output includes both machine code and symbol table

### Emulator (emu.cpp)
- Executes generated machine code
- Simulates basic processor functionality
- Memory management and register operations
- Step-by-step execution capability
- Runtime error detection and reporting

## Setup and Installation

1. **Prerequisites**
   ```bash
   # Install Node.js and npm
   # Install C++ compiler (g++ recommended)
   ```

2. **Compile Native Components**
   ```bash
   # Compile the assembler
   g++ -o assembler asm.cpp
   
   # Compile the emulator
   g++ -o emulator emu.cpp
   ```

3. **Setup Backend**
   ```bash
   cd backend
   npm install
   node server.js
   ```

4. **Setup Frontend**
   ```bash
   cd frontend
   npm install
   npm start
   ```

## Usage

1. **Writing Code**
   - Open the IDE in your web browser
   - Write your assembly code in the editor panel
   - Save your code using the Save button

2. **Assembling Code**
   - Click the "Assemble" button to process your code
   - View assembler output in the output panel
   - Check for any errors or warnings

3. **Running Code**
   - Click "Emulate" to execute the assembled code
   - View execution results in the emulator tab
   - Monitor register and memory states

## API Endpoints

### Backend Server Endpoints

```
POST /api/assemble
- Assembles provided code
- Request body: { code: string }
- Response: { success: boolean, machineCode: string, errors: string[] }

POST /api/emulate
- Emulates assembled code
- Request body: { machineCode: string }
- Response: { success: boolean, output: string, memoryState: object }

POST /api/save
- Saves current code
- Request body: { code: string, filename: string }
- Response: { success: boolean, message: string }
```

## Supported Assembly Instructions

- Data Movement: MOV, PUSH, POP
- Arithmetic: ADD, SUB, MUL, DIV
- Logic: AND, OR, XOR, NOT
- Control Flow: JMP, JZ, JNZ, CALL, RET
- System: INT, HLT

## Development

### Building the Frontend
```bash
cd frontend
npm run build
```

### Running Tests
```bash
# Frontend tests
cd frontend
npm test

# Backend tests
cd backend
npm test
```

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- Thanks to all contributors who have helped with the development
- Inspired by various open-source assembly IDEs and emulators# Two_Pass_Assembler
