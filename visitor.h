#pragma once

// Forward declarations
class Variable;
class Abstraction;
class Application;
class NamedReference;

// Visitor interface
class IVisitor {
public:
    virtual ~IVisitor() = default;
    
    // Visit methods for each type of expression
    virtual void visit(Variable& variable) = 0;
    virtual void visit(Abstraction& abstraction) = 0;
    virtual void visit(Application& application) = 0;
    virtual void visit(NamedReference& reference) = 0;
};