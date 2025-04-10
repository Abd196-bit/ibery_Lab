"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const compiler_1 = require("./compiler/compiler");
const sourceCode = `
// Quantum Computation (Example: Quantum Gate)
run quantum "apply quantum gate H"

// Physics (Example: Calculate Force using F = m * a)
def calculate_force(mass, acceleration):
    force = mass * acceleration
    return force

// Animate (Simple game animation)
def animate_game():
    run "create_game_object('player')"
    run "set_position('player', 0, 0)"
    run "move_object('player', 5, 0)"
    run "display('player')"

// Backend (Example: Simple API)
def setup_backend():
    run "create_api('GET', '/status')"
    run "create_api('POST', '/submit')"
    print("Server Running")

// Frontend (Simple UI)
def create_frontend():
    run "create_button('Start Game')"
    run "create_input('Enter Name')"
    print("Frontend Loaded")

// Main Code Execution
def main():
    // Quantum
    run quantum "apply quantum gate H"

    // Physics
    mass = 10
    acceleration = 9.8
    force = calculate_force(mass, acceleration)
    print("Calculated Force: " + force)

    // Animation
    animate_game()

    // Backend
    setup_backend()

    // Frontend
    create_frontend()

// Run the main function
main()
`;
const compiler = new compiler_1.IberyCompiler();
// Compile the source code to binary
const binary = compiler.compile(sourceCode);
// Disassemble the binary to see the generated instructions
const disassembly = compiler.disassemble(binary);
console.log('Generated Binary Instructions:');
console.log(disassembly);
// Save the binary to a file
const fs = require('fs');
fs.writeFileSync('output.ibery', binary);
console.log('\nBinary saved to output.ibery');
