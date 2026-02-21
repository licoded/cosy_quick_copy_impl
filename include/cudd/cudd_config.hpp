#pragma once

#include <string>

namespace Cosy {

// CUDD 配置常量
constexpr int CUDD_TIMES_UNIQUE_SLOTS = 4;
constexpr int CUDD_TIMES_CACHE_SLOTS = 4;
constexpr bool PRINT_CUDD_FLAG = false;

// Hash ID 接口类
class IHashId {
public:
    virtual ~IHashId() = default;
    virtual uint64_t getHashId() const = 0;
    virtual std::string toString() const = 0;
};

// 错误退出函数
[[noreturn]] void exit_with_error(const std::string& msg);

} // namespace Cosy
