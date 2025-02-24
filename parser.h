#pragma once

#include "Expression.h"
#include "Environment.h"
#include <string>
#include <vector>
#include <stdexcept>

// Custom exception for parser errors
class ParserError : public std::runtime_error {
public:
    explicit ParserError(const std::string& message) : std::runtime_error(message) {}
};

// Parser for lambda calculus expressions
class Parser {
private:
    std::string input;
    size_t position = 0;
    Environment& environment;
    
    // Helper methods for parsing
    char peek() const;
    char current() const;
    bool isAtEnd() const;
    void advance();
    void skipWhitespace();
    bool match(char expected);
    bool isAlpha(char c) const;
    bool isAlphaNumeric(char c) const;
    
    // Recursive descent parsing
    std::shared_ptr<Expression> parseExpression();
    std::shared_ptr<Expression> parseApplication();
    std::shared_ptr<Expression> parsePrimary();
    std::shared_ptr<Expression> parseVariable();
    std::shared_ptr<Expression> parseAbstraction();
    std::shared_ptr<Expression> parseParenthesized();
    
    // Token methods
    std::string parseIdentifier();

public:
    explicit Parser(std::string input, Environment& env) 
        : input(std::move(input)), environment(env) {}
    
    // Parse a lambda calculus expression from a string
    std::shared_ptr<Expression> parse();
    
    // Parse a definition (name = expression)
    bool parseDefinition(std::string& name, std::shared_ptr<Expression>& expr);
};