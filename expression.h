#pragma once

#include <string>
#include <memory>

// Forward declarations
class Variable;
class Abstraction;
class Application;
class NamedReference;
class IVisitor;

// Base Expression class
class Expression {
public:
    virtual ~Expression() = default;
    
    // Accept method for the visitor pattern
    virtual void accept(IVisitor& visitor) = 0;
    
    // Helper method to create a deep copy of the expression
    virtual std::shared_ptr<Expression> clone() const = 0;
    
    // Helper method to convert expression to string for display
    virtual std::string toString() const = 0;
};

// Variable expression (represents a variable in lambda calculus)
class Variable : public Expression {
private:
    std::string name;

public:
    explicit Variable(std::string name) : name(std::move(name)) {}
    
    void accept(IVisitor& visitor) override;
    
    std::shared_ptr<Expression> clone() const override {
        return std::make_shared<Variable>(name);
    }
    
    std::string toString() const override {
        return name;
    }
    
    const std::string& getName() const {
        return name;
    }
};

// Abstraction expression (represents a lambda abstraction: λx.M)
class Abstraction : public Expression {
private:
    std::string parameter;
    std::shared_ptr<Expression> body;

public:
    Abstraction(std::string parameter, std::shared_ptr<Expression> body)
        : parameter(std::move(parameter)), body(std::move(body)) {}
    
    void accept(IVisitor& visitor) override;
    
    std::shared_ptr<Expression> clone() const override {
        return std::make_shared<Abstraction>(parameter, body->clone());
    }
    
    std::string toString() const override {
        return "λ" + parameter + "." + body->toString();
    }
    
    const std::string& getParameter() const {
        return parameter;
    }
    
    const std::shared_ptr<Expression>& getBody() const {
        return body;
    }
};

// Application expression (represents function application: M N)
class Application : public Expression {
private:
    std::shared_ptr<Expression> function;
    std::shared_ptr<Expression> argument;

public:
    Application(std::shared_ptr<Expression> function, std::shared_ptr<Expression> argument)
        : function(std::move(function)), argument(std::move(argument)) {}
    
    void accept(IVisitor& visitor) override;
    
    std::shared_ptr<Expression> clone() const override {
        return std::make_shared<Application>(function->clone(), argument->clone());
    }
    
    std::string toString() const override {
        return "(" + function->toString() + " " + argument->toString() + ")";
    }
    
    const std::shared_ptr<Expression>& getFunction() const {
        return function;
    }
    
    const std::shared_ptr<Expression>& getArgument() const {
        return argument;
    }
};

// Named reference expression (refers to a defined expression)
class NamedReference : public Expression {
private:
    std::string name;

public:
    explicit NamedReference(std::string name) : name(std::move(name)) {}
    
    void accept(IVisitor& visitor) override;
    
    std::shared_ptr<Expression> clone() const override {
        return std::make_shared<NamedReference>(name);
    }
    
    std::string toString() const override {
        return name;  // Just show the name
    }
    
    const std::string& getName() const {
        return name;
    }
};