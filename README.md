# Lambda Calculus Interpreter

A C++ implementation of a lambda calculus interpreter. Interpreter supports named expressions, allowing you to define and reuse common lambda expressions.

## Features

- **Pure Lambda Calculus Core**: Supports variables, abstractions (λx.M), and applications (M N)
- **Named Expressions**: Define expressions once and reuse them by name
- **Beta Reduction**: Properly handles variable substitution with alpha conversion
- **Normal Order Evaluation**: Implements the standard evaluation strategy for lambda calculus
- **Church Encodings**: Pre-loaded examples of Church numerals, booleans, and operations
- **Interactive Mode**: Command-line interface for experimenting with lambda expressions

## Building the Project

### Prerequisites

- CMake 3.10 or higher
- C++17 compatible compiler

### Build Steps

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Running the Interpreter

```bash
./lambda_calculus
```

## Usage Examples

### Defining Expressions

```
> zero = \f.\x.x
Defined zero = λf.λx.x

> one = \f.\x.f x
Defined one = λf.λx.f x

> plus = \m.\n.\f.\x.m f (n f x)
Defined plus = λm.λn.λf.λx.m f (n f x)
```

### Evaluating Expressions

```
> (plus one one)
Parsed: ((λm.λn.λf.λx.m f (n f x) λf.λx.f x) λf.λx.f x)
Result: λf.λx.f (f x)
This is equivalent to: two
```

### Built-in Commands

- `:defs` - Show all defined expressions
- `:help` - Display help information
- `:quit` or `:exit` - Exit the interpreter

## Church Encodings

The interpreter comes pre-loaded with several Church encodings:

- **Numerals**: `zero`, `one`, `two`, `three`
- **Arithmetic**: `succ`, `plus`, `mult`, `pred`
- **Booleans**: `true`, `false`
- **Control Flow**: `if`, `iszero`
- **Recursion**: `Y` (Y combinator)

## Implementation Details

The project is structured around these key components:

- **Expression Hierarchy**: Defines the AST structure
- **Visitor Pattern**: Separates operations from AST structure
- **Parser**: Converts strings to expression trees
- **Evaluator**: Performs beta reduction according to normal order rules
- **Environment**: Stores and manages named expressions

## Extending the Interpreter

Some possible extensions to consider:

1. **Applicative Order Evaluation**: Implement innermost-first reduction
2. **Type Checking**: Add simple types and type inference
3. **Standard Library**: More pre-defined combinators and utilities
4. **Step-by-Step Evaluation**: Show the intermediate steps of reduction
5. **Y-Combinator**: Fixing issues with y-combinator definition