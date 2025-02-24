#include "Parser.h"
#include <sstream>
#include <regex>

// Helper methods
char Parser::peek() const {
    if (isAtEnd()) return '\0';
    return input[position];
}

char Parser::current() const {
    if (position == 0 || position > input.length()) return '\0';
    return input[position - 1];
}

bool Parser::isAtEnd() const {
    return position >= input.length();
}

void Parser::advance() {
    if (!isAtEnd()) position++;
}

void Parser::skipWhitespace() {
    while (!isAtEnd() && std::isspace(peek())) {
        advance();
    }
}

bool Parser::match(char expected) {
    if (isAtEnd() || peek() != expected) return false;
    advance();
    return true;
}

bool Parser::isAlpha(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool Parser::isAlphaNumeric(char c) const {
    return isAlpha(c) || (c >= '0' && c <= '9');
}

// Parsing methods
std::shared_ptr<Expression> Parser::parse() {
    skipWhitespace();
    auto expr = parseExpression();
    
    // Check that we've consumed all input
    skipWhitespace();
    if (!isAtEnd()) {
        std::stringstream ss;
        ss << "Unexpected character '" << peek() << "' at position " << position;
        throw ParserError(ss.str());
    }
    
    return expr;
}

bool Parser::parseDefinition(std::string& name, std::shared_ptr<Expression>& expr) {
    // Regular expression to match definitions (name = expression)
    std::regex def_regex("^\\s*([a-zA-Z][a-zA-Z0-9]*)\\s*=\\s*(.+)$");
    std::smatch matches;
    
    if (std::regex_match(input, matches, def_regex)) {
        name = matches[1];
        
        // Create a new parser for the expression part
        Parser exprParser(matches[2], environment);
        expr = exprParser.parse();
        
        return true;
    }
    
    return false;
}

std::shared_ptr<Expression> Parser::parseExpression() {
    return parseApplication();
}

std::shared_ptr<Expression> Parser::parseApplication() {
    // Parse the first expression
    auto expr = parsePrimary();
    
    // Look for additional expressions to create applications
    while (true) {
        skipWhitespace();
        
        // If we see another expression, it's part of the application
        if (!isAtEnd() && (isAlpha(peek()) || peek() == '(' || peek() == 'λ' || peek() == '\\')) {
            auto right = parsePrimary();
            expr = std::make_shared<Application>(expr, right);
        } else {
            break;
        }
    }
    
    return expr;
}

std::shared_ptr<Expression> Parser::parsePrimary() {
    skipWhitespace();
    
    if (isAlpha(peek())) {
        return parseVariable();
    }
    
    if (match('(')) {
        return parseParenthesized();
    }
    
    if (match('λ') || match('\\')) {
        return parseAbstraction();
    }
    
    std::stringstream ss;
    ss << "Unexpected character '" << peek() << "' at position " << position;
    throw ParserError(ss.str());
}

std::shared_ptr<Expression> Parser::parseVariable() {
    std::string name = parseIdentifier();
    
    // Check if this is a named reference to a defined expression
    if (environment.isDefined(name)) {
        return std::make_shared<NamedReference>(name);
    }
    
    // Otherwise, it's just a variable
    return std::make_shared<Variable>(name);
}

std::string Parser::parseIdentifier() {
    std::string name;
    
    // First character must be alphabetic
    if (!isAtEnd() && isAlpha(peek())) {
        name += peek();
        advance();
    } else {
        std::stringstream ss;
        ss << "Expected identifier at position " << position;
        throw ParserError(ss.str());
    }
    
    // Subsequent characters can be alphanumeric
    while (!isAtEnd() && isAlphaNumeric(peek())) {
        name += peek();
        advance();
    }
    
    return name;
}

std::shared_ptr<Expression> Parser::parseAbstraction() {
    // Parse the parameter
    skipWhitespace();
    std::string parameter = parseIdentifier();
    
    // Parse the dot
    skipWhitespace();
    if (!match('.')) {
        std::stringstream ss;
        ss << "Expected '.' after lambda parameter at position " << position;
        throw ParserError(ss.str());
    }
    
    // Parse the body
    skipWhitespace();
    auto body = parseExpression();
    
    return std::make_shared<Abstraction>(parameter, body);
}

std::shared_ptr<Expression> Parser::parseParenthesized() {
    // Parse the expression inside the parentheses
    auto expr = parseExpression();
    
    // Make sure we close the parentheses
    skipWhitespace();
    if (!match(')')) {
        std::stringstream ss;
        ss << "Expected ')' at position " << position;
        throw ParserError(ss.str());
    }
    
    return expr;
}