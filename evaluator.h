#pragma once

#include "Expression.h"
#include "Visitor.h"
#include "Environment.h"
#include <unordered_set>

// Evaluator for lambda expressions using the visitor pattern
class Evaluator : public IVisitor {
private:
    std::shared_ptr<Expression> result;
    Environment& environment;
    
    // Helper methods for evaluation
    std::shared_ptr<Expression> substitute(
        const std::shared_ptr<Expression>& expr,
        const std::string& var,
        const std::shared_ptr<Expression>& replacement);
    
    std::unordered_set<std::string> getFreeVariables(const std::shared_ptr<Expression>& expr);
    std::string generateFreshVariable(const std::unordered_set<std::string>& usedVars, 
                                      const std::string& hint = "x");

public:
    explicit Evaluator(Environment& env) : environment(env) {}
    
    // Visitor pattern implementation
    void visit(Variable& variable) override;
    void visit(Abstraction& abstraction) override;
    void visit(Application& application) override;
    void visit(NamedReference& reference) override;
    
    // Evaluate using normal order reduction (outermost, leftmost redex first)
    std::shared_ptr<Expression> evaluateNormalOrder(const std::shared_ptr<Expression>& expr);
    
    // Evaluate using applicative order reduction (innermost redexes first)
    std::shared_ptr<Expression> evaluateApplicativeOrder(const std::shared_ptr<Expression>& expr);
    
    // Perform a single beta reduction step
    std::shared_ptr<Expression> betaReduce(const std::shared_ptr<Expression>& expr);
    
    // Check if an expression is in normal form (cannot be reduced further)
    bool isNormalForm(const std::shared_ptr<Expression>& expr);
};