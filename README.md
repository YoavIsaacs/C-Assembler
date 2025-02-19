# Two-Pass Assembler

A comprehensive assembler implementation for a custom assembly language. This project implements a complete compilation process including preprocessing, parsing, symbol resolution, and code generation.

## Overview

This assembler provides a full-featured toolchain for compiling custom assembly code into machine code. It features:

- Macro preprocessing system
- Two-pass compilation approach
- Complete symbol table management
- Support for directives and labels
- Error detection and reporting
- Output generation in base-4 encoding

## Architecture

The assembler is structured in multiple components:

### 1. Preprocessor
Handles macro definitions and expansions before the main compilation process.

### 2. Front-End Parser
Parses assembly language instructions into an Abstract Syntax Tree (AST).

### 3. First Pass
- Builds symbol table
- Calculates memory addresses
- Processes directives (.data, .string, .entry, .extern, .define)
- Performs initial validation

### 4. Second Pass
- Resolves symbolic references
- Generates final machine code
- Handles external references

### 5. Code Output
Generates output files in the required format.

## Data Structures

The project utilizes several specialized data structures:

- **Trie**: For efficient symbol lookup and keyword management
- **Linked List**: For storing macro definitions
- **Symbol Table**: For tracking labels and their addresses
- **Abstract Syntax Tree (AST)**: For representing parsed instructions

## Supported Instruction Set

The assembler supports the following instructions:

| Instruction | Description | Operands |
|-------------|-------------|----------|
| `mov` | Move data | Source, Destination |
| `cmp` | Compare values | Source, Destination |
| `add` | Add values | Source, Destination |
| `sub` | Subtract values | Source, Destination |
| `not` | Logical NOT | Destination |
| `clr` | Clear (zero) | Destination |
| `lea` | Load effective address | Source, Destination |
| `inc` | Increment | Destination |
| `dec` | Decrement | Destination |
| `jmp` | Jump | Destination |
| `bne` | Branch if not equal | Destination |
| `red` | Read input | Destination |
| `prn` | Print value | Source |
| `jsr` | Jump to subroutine | Destination |
| `rts` | Return from subroutine | None |
| `hlt` | Halt execution | None |

## Directives

The assembler supports the following directives:

- `.data` - Define integer data values
- `.string` - Define string constants
- `.entry` - Declare entry points
- `.extern` - Declare external symbols
- `.define` - Define symbolic constants

## Addressing Modes

The assembler supports various addressing modes:

1. **Immediate addressing** (`#value`)
2. **Direct addressing** (label)
3. **Register addressing** (`r0`-`r7`)
4. **Index addressing** (`label[index]`)

## Build and Usage

### Prerequisites
- C compiler (GCC recommended)
- Standard C libraries

### Building
```bash
gcc -o assembler *.c DataStructures/*.c Frontend/*.c Preprocessor/*.c -I.
```

### Running
```bash
./assembler filename
```

This will process `filename.as` and generate:
- `filename.am` - After macro preprocessing
- `filename.ob` - Object code in base-4 format
- `filename.ent` - Entry points (if any)
- `filename.ext` - External references (if any)

## Files Output Format

### Object File (.ob)
Contains the compiled machine code in base-4 representation:
```
  [code words count] [data words count]
[address] [encoded word]
...
```

### Entry File (.ent)
Lists all entry points with their addresses:
```
[symbol name] [address]
...
```

### External File (.ext)
Lists all external references with their usage addresses:
```
[external symbol] [reference address]
...
```

## Error Handling

The assembler provides detailed error messages for various issues:
- Syntax errors
- Undefined symbols
- Invalid operands
- Memory allocation failures
- File access issues

## Implementation Details

### Memory Word Format
Instructions are encoded in a 10-bit word format with specific bit allocations for opcodes and operands.

### Base-4 Encoding
The final output uses a custom base-4 encoding with the following symbols:
- '*' represents 0
- '#' represents 1
- '%' represents 2
- '!' represents 3

## Contributors

Yoav Isaacs

## License

[MIT License](LICENSE)
