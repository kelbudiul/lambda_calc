// main.cpp
#include "Expression.h"
#include "Visitor.h"
#include "Evaluator.h"
#include "Parser.h"
#include "Environment.h"
#include <iostream>
#include <string>
#include <memory>
#include <regex>
#include "Windows.h"

// Class to print lambda expressions in a pretty format
class PrettyPrinter : public IVisitor {
private:
    std::string result;
    const Environment& environment;

public:
    explicit PrettyPrinter(const Environment& env) : environment(env) {}

    void visit(Variable& variable) override {
        result = variable.getName();
    }

    void visit(Abstraction& abstraction) override {
        result = "L" + abstraction.getParameter() + ".";
        
        // Use visitor to get body string
        PrettyPrinter bodyPrinter(environment);
        abstraction.getBody()->accept(bodyPrinter);
        result += bodyPrinter.result;
    }

    void visit(Application& application) override {
        // Use visitor to get function and argument strings
        PrettyPrinter functionPrinter(environment);
        PrettyPrinter argumentPrinter(environment);
        
        application.getFunction()->accept(functionPrinter);
        application.getArgument()->accept(argumentPrinter);
        
        // Add parentheses around the application
        result = "(" + functionPrinter.result + " " + argumentPrinter.result + ")";
    }
    
    void visit(NamedReference& reference) override {
        // For named references, just show the name
        result = reference.getName();
        
        // Optionally, we could expand the reference to show its definition
        // auto definition = environment.lookup(reference.getName());
        // if (definition) {
        //     definition->accept(*this);
        // } else {
        //     result = reference.getName();
        // }
    }

    std::string toString(const std::shared_ptr<Expression>& expr) {
        expr->accept(*this);
        return result;
    }
};

int main() {

    SetConsoleOutputCP(CP_UTF8);
    
    std::cout << "Enhanced Lambda Calculus Interpreter" << std::endl;
    std::cout << "==================================" << std::endl;
    
    // Create environment
    Environment env;
    
    // Create evaluator with the environment
    Evaluator evaluator(env);
    
    // Define Church numerals and operations
    std::vector<std::pair<std::string, std::string>> definitions = {
        {"zero", "\\f.\\x.x"},
        {"one", "\\f.\\x.f x"},
        {"two", "\\f.\\x.f (f x)"},
        {"three", "\\f.\\x.f (f (f x))"},
        {"succ", "\\n.\\f.\\x.f (n f x)"},
        {"plus", "\\m.\\n.\\f.\\x.m f (n f x)"},
        {"mult", "\\m.\\n.\\f.m (n f)"},
        {"pred", "\\n.\\f.\\x.n (\\g.\\h.h (g f)) (\\u.x) (\\u.u)"},
        {"iszero", "\\n.n (\\x.\\t.\\f.f) (\\t.\\f.t)"},
        {"true", "\\t.\\f.t"},
        {"false", "\\t.\\f.f"},
        {"if", "\\p.\\a.\\b.p a b"},
        {"Y", "\\f.(\\x.f (x x)) (\\x.f (x x))"}  // Y combinator for recursion
    };
    
    // Add definitions to environment
    for (const auto& [name, exprStr] : definitions) {
        try {
            Parser parser(exprStr, env);
            auto expr = parser.parse();
            env.define(name, expr);
            std::cout << "Defined " << name << " = " << expr->toString() << std::endl;
        } catch (const ParserError& e) {
            std::cerr << "Parser error in definition of " << name << ": " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error in definition of " << name << ": " << e.what() << std::endl;
        }
    }
    
    // Interactive mode
    std::cout << "\nInteractive Mode" << std::endl;
    std::cout << "================" << std::endl;
    std::cout << "Commands:" << std::endl;
    std::cout << "  name = expression   Define a named expression" << std::endl;
    std::cout << "  expression          Evaluate an expression" << std::endl;
    std::cout << "  :quit or :exit      Exit the interpreter" << std::endl;
    std::cout << "  :defs               Show all definitions" << std::endl;
    std::cout << "  :help               Show this help message" << std::endl;
    
    std::string line;
    
    while (true) {
        std::cout << "\n> ";
        std::getline(std::cin, line);
        
        if (line == ":quit" || line == ":exit") {
            break;
        }
        
        if (line == ":help") {
            std::cout << "Commands:" << std::endl;
            std::cout << "  name = expression   Define a named expression" << std::endl;
            std::cout << "  expression          Evaluate an expression" << std::endl;
            std::cout << "  :quit or :exit      Exit the interpreter" << std::endl;
            std::cout << "  :defs               Show all definitions" << std::endl;
            std::cout << "  :help               Show this help message" << std::endl;
            continue;
        }
        
        if (line == ":defs") {
            env.printDefinitions();
            continue;
        }
        
        try {
            // Check if this is a definition
            std::string name;
            std::shared_ptr<Expression> expr;
            Parser parser(line, env);
            
            if (parser.parseDefinition(name, expr)) {
                // Add to environment
                env.define(name, expr);
                std::cout << "Defined " << name << " = " << expr->toString() << std::endl;
            } else {
                // This is an expression to evaluate
                expr = parser.parse();
                std::cout << "Parsed: " << expr->toString() << std::endl;
                
                auto result = evaluator.evaluateNormalOrder(expr);
                std::cout << "Result: " << result->toString() << std::endl;
            }
        } catch (const ParserError& e) {
            std::cerr << "Parser error: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
    
    return 0;
}