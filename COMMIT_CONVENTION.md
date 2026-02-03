# Git Commit Convention

本项目采用基于 Vue.js 的 commit 约定，格式如下：

```
^(revert: )?(feat|fix|docs|dx|style|refactor|perf|test|workflow|build|ci|chore|types|wip)(\(.+\))?: .{1,50}
```

## 规则

- **type**: 必需。表示提交的类型。
  - `feat`: 新功能
  - `fix`: 修复 bug
  - `docs`: 文档更新
  - `dx`: 开发者体验改进 (Developer Experience)
  - `style`: 代码风格调整 (不影响逻辑)
  - `refactor`: 重构 (不新增功能)
  - `perf`: 性能优化
  - `test`: 测试相关
  - `workflow`: 工作流相关
  - `build`: 构建系统相关
  - `ci`: CI/CD 相关
  - `chore`: 杂项 (如依赖更新)
  - `types`: 类型定义相关
  - `wip`: 工作进行中

- **scope**: 可选。中英文都可以，表示影响的模块，如 `formula`, `parser`, `git`, `CMake` 等。

- **description**: 必需。中文 + 英文专有名词/术语，描述变更内容，长度 1-50 字符。

- **revert**: 可选，用于撤销之前的提交。

## 示例

- `feat(formula): 实现 Formula 类的 to_string 方法`
- `fix(parser): 修复 unary 运算符解析优先级`
- `docs: 更新 README.md`
- `refactor(build): 重构 CMake 配置以支持多平台`
- `revert: feat(formula): 撤销新增的 simplify 方法`

## 参考

- [Vue.js Commit Convention](https://github.com/vuejs/core/blob/main/.github/commit-convention.md)