#pragma once

#include "formula/visitor.hpp"
#include "formula/builder.hpp"

namespace Cosy {

class SimplificationVisitor : public Visitor {
public:
    explicit SimplificationVisitor(FormulaBuilder& builder);

    // 设置要简化的原始公式
    void set_original(Formula* original) { original_ = original; }

    // 获取简化后的结果
    Formula* get_result() const { return result_; }

    // 通用访问方法
    void visit(Formula* formula) override;

    // 根据操作符类型访问
    void visit_binary(Formula* formula) override;
    void visit_unary(Formula* formula) override;
    void visit_literal(Formula* formula) override;
    void visit_constant(Formula* formula) override;

    // 特定操作符访问方法
    void visit_and(Formula* formula) override;
    void visit_or(Formula* formula) override;
    void visit_not(Formula* formula) override;
    void visit_next(Formula* formula) override;
    void visit_wnext(Formula* formula) override;
    void visit_until(Formula* formula) override;
    void visit_release(Formula* formula) override;

    // 常量访问方法
    void visit_true(Formula* formula) override;
    void visit_false(Formula* formula) override;

private:
    FormulaBuilder& builder_;
    Formula* original_ = nullptr;
    Formula* result_ = nullptr;
};

} // namespace Cosy