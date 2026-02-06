#pragma once

#include "formula.hpp"
#include "operator.hpp"

namespace Cosy {

class Visitor {
public:
    virtual ~Visitor() = default;

    // 通用访问方法
    virtual void visit(Formula* formula) = 0;

    // 根据操作符类型访问
    virtual void visit_binary(Formula* formula) = 0;
    virtual void visit_unary(Formula* formula) = 0;
    virtual void visit_literal(Formula* formula) = 0;
    virtual void visit_constant(Formula* formula) = 0;

    // 特定操作符访问方法
    virtual void visit_and(Formula* formula) = 0;
    virtual void visit_or(Formula* formula) = 0;
    virtual void visit_not(Formula* formula) = 0;
    virtual void visit_next(Formula* formula) = 0;
    virtual void visit_wnext(Formula* formula) = 0;
    virtual void visit_until(Formula* formula) = 0;
    virtual void visit_release(Formula* formula) = 0;

    // 为常量值提供专门的方法
    virtual void visit_true(Formula* formula) = 0;
    virtual void visit_false(Formula* formula) = 0;
};

} // namespace Cosy