#include "Expression.h"
#include "Visitor.h"

// Implementation of the accept methods for each expression type

void Variable::accept(IVisitor& visitor) {
    visitor.visit(*this);
}

void Abstraction::accept(IVisitor& visitor) {
    visitor.visit(*this);
}

void Application::accept(IVisitor& visitor) {
    visitor.visit(*this);
}

void NamedReference::accept(IVisitor& visitor) {
    visitor.visit(*this);
}