#pragma once

#include "Expression.h"
#include <string>
#include <map>
#include <memory>
#include <iostream>

// Environment to store named expressions
class Environment {
private:
    // Map of names to expressions
    std::map<std::string, std::shared_ptr<Expression>> definitions;

public:
    // Add a definition to the environment
    void define(const std::string& name, const std::shared_ptr<Expression>& expr) {
        definitions[name] = expr;
    }
    
    // Look up a definition
    std::shared_ptr<Expression> lookup(const std::string& name) const {
        auto it = definitions.find(name);
        if (it != definitions.end()) {
            return it->second;
        }
        return nullptr;
    }
    
    // Check if a name is defined
    bool isDefined(const std::string& name) const {
        return definitions.find(name) != definitions.end();
    }
    
    // Print all definitions
    void printDefinitions() const {
        if (definitions.empty()) {
            std::cout << "No definitions yet." << std::endl;
            return;
        }
        
        for (const auto& [name, expr] : definitions) {
            std::cout << name << " = " << expr->toString() << std::endl;
        }
    }
    
    // Access the definitions map directly (for iteration)
    const std::map<std::string, std::shared_ptr<Expression>>& getDefinitions() const {
        return definitions;
    }
};