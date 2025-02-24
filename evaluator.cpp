#include "Evaluator.h"
#include <algorithm>
#include <queue>

// Visitor pattern implementation
void Evaluator::visit(Variable& variable) {
    // Variables evaluate to themselves
    result = std::make_shared<Variable>(variable.getName());
}

void Evaluator::visit(Abstraction& abstraction) {
    // For abstractions, we evaluate the body
    auto evaluatedBody = evaluateNormalOrder(abstraction.getBody());
    result = std::make_shared<Abstraction>(abstraction.getParameter(), evaluatedBody);
}

void Evaluator::visit(Application& application) {
    // For applications, we perform beta reduction if possible
    auto function = application.getFunction();
    auto argument = application.getArgument();
    
    // Check if the function is an abstraction (this is a redex)
    if (auto abstraction = std::dynamic_pointer_cast<Abstraction>(function)) {
        // Perform beta reduction: (λx.M) N -> M[x := N]
        result = substitute(abstraction->getBody(), abstraction->getParameter(), argument);
    } else {
        // If not, we just evaluate both parts
        auto evaluatedFunction = evaluateNormalOrder(function);
        auto evaluatedArgument = evaluateNormalOrder(argument);
        result = std::make_shared<Application>(evaluatedFunction, evaluatedArgument);
    }
}

void Evaluator::visit(NamedReference& reference) {
    // Look up the definition in the environment
    auto definition = environment.lookup(reference.getName());
    if (definition) {
        // Found a definition, return it
        result = definition->clone();
    } else {
        // If not defined, it's just a variable with the same name
        result = std::make_shared<Variable>(reference.getName());
    }
}

// Helper method to substitute a variable with an expression
std::shared_ptr<Expression> Evaluator::substitute(
    const std::shared_ptr<Expression>& expr,
    const std::string& var,
    const std::shared_ptr<Expression>& replacement) {
    
    // Case 1: Variable
    if (auto variable = std::dynamic_pointer_cast<Variable>(expr)) {
        if (variable->getName() == var) {
            // If this is the variable we're replacing, return the replacement
            return replacement->clone();
        } else {
            // Otherwise, keep the variable as is
            return variable->clone();
        }
    }
    
    // Case 1.5: Named Reference
    if (auto reference = std::dynamic_pointer_cast<NamedReference>(expr)) {
        // Look up the definition and substitute in it
        auto definition = environment.lookup(reference->getName());
        if (definition) {
            return substitute(definition, var, replacement);
        } else {
            // If not defined, treat it as a variable
            if (reference->getName() == var) {
                return replacement->clone();
            } else {
                return reference->clone();
            }
        }
    }
    
    // Case 2: Abstraction
    if (auto abstraction = std::dynamic_pointer_cast<Abstraction>(expr)) {
        if (abstraction->getParameter() == var) {
            // If the parameter shadows our variable, don't substitute in the body
            return abstraction->clone();
        } else {
            // Check if the parameter occurs free in the replacement
            auto freeVarsInReplacement = getFreeVariables(replacement);
            if (freeVarsInReplacement.find(abstraction->getParameter()) != freeVarsInReplacement.end()) {
                // Parameter capture would occur - rename the parameter
                auto allVars = getFreeVariables(expr);
                allVars.insert(freeVarsInReplacement.begin(), freeVarsInReplacement.end());
                auto freshVar = generateFreshVariable(allVars, abstraction->getParameter());
                
                // Create a new abstraction with the renamed parameter
                auto renamedBody = substitute(abstraction->getBody(), 
                                             abstraction->getParameter(), 
                                             std::make_shared<Variable>(freshVar));
                
                // Now substitute in the renamed body
                auto newBody = substitute(renamedBody, var, replacement);
                return std::make_shared<Abstraction>(freshVar, newBody);
            } else {
                // No parameter capture - substitute in the body
                auto newBody = substitute(abstraction->getBody(), var, replacement);
                return std::make_shared<Abstraction>(abstraction->getParameter(), newBody);
            }
        }
    }
    
    // Case 3: Application
    if (auto application = std::dynamic_pointer_cast<Application>(expr)) {
        auto newFunction = substitute(application->getFunction(), var, replacement);
        auto newArgument = substitute(application->getArgument(), var, replacement);
        return std::make_shared<Application>(newFunction, newArgument);
    }
    
    // Default case (shouldn't happen with well-formed expressions)
    return expr->clone();
}

// Helper method to get all free variables in an expression
std::unordered_set<std::string> Evaluator::getFreeVariables(const std::shared_ptr<Expression>& expr) {
    std::unordered_set<std::string> freeVars;
    
    if (auto variable = std::dynamic_pointer_cast<Variable>(expr)) {
        freeVars.insert(variable->getName());
    } else if (auto reference = std::dynamic_pointer_cast<NamedReference>(expr)) {
        // For a named reference, get free variables from its definition
        auto definition = environment.lookup(reference->getName());
        if (definition) {
            freeVars = getFreeVariables(definition);
        } else {
            // If not defined, treat it as a variable
            freeVars.insert(reference->getName());
        }
    } else if (auto abstraction = std::dynamic_pointer_cast<Abstraction>(expr)) {
        freeVars = getFreeVariables(abstraction->getBody());
        freeVars.erase(abstraction->getParameter());
    } else if (auto application = std::dynamic_pointer_cast<Application>(expr)) {
        auto functionVars = getFreeVariables(application->getFunction());
        auto argumentVars = getFreeVariables(application->getArgument());
        freeVars.insert(functionVars.begin(), functionVars.end());
        freeVars.insert(argumentVars.begin(), argumentVars.end());
    }
    
    return freeVars;
}

// Helper method to generate a fresh variable name
std::string Evaluator::generateFreshVariable(const std::unordered_set<std::string>& usedVars, 
                                           const std::string& hint) {
    std::string base = hint;
    int suffix = 0;
    std::string freshVar = base;
    
    while (usedVars.find(freshVar) != usedVars.end()) {
        freshVar = base + std::to_string(++suffix);
    }
    
    return freshVar;
}

// Evaluate using normal order reduction
std::shared_ptr<Expression> Evaluator::evaluateNormalOrder(const std::shared_ptr<Expression>& expr) {
    if (isNormalForm(expr)) {
        return expr->clone();
    }
    
    auto reduced = betaReduce(expr);
    return evaluateNormalOrder(reduced);
}

// Evaluate using applicative order reduction
std::shared_ptr<Expression> Evaluator::evaluateApplicativeOrder(const std::shared_ptr<Expression>& expr) {
    // To be implemented later - for now, use normal order
    return evaluateNormalOrder(expr);
}

// Perform a single beta reduction step
std::shared_ptr<Expression> Evaluator::betaReduce(const std::shared_ptr<Expression>& expr) {
    // Reset result
    result.reset();
    
    // Use the visitor pattern to perform reduction
    expr->accept(*this);
    
    // If no reduction was performed, return the original expression
    if (!result) {
        return expr->clone();
    }
    
    return result;
}

// Check if an expression is in normal form
bool Evaluator::isNormalForm(const std::shared_ptr<Expression>& expr) {
    // Check if expression can be reduced further
    auto reduced = betaReduce(expr);
    return reduced->toString() == expr->toString();
}