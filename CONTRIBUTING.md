# HOW TO CONTRIBUTE TO ***lib-dnml***

## Table of Content
- [How to sart contributing?](#how-to-start-contributing)
- [Development Workflow](#development-workflow)
- [Code Standard & Requirements](#code-standard--requiremnets)
- [Pull Request Guidelines](#pull-request-guidelines)
- [Commit Message Guidelines](#commit-message-guidelines)
- [Issue Guidelines](#issue-guidelines)
- [Testing expectations](#testing-expectations)

<!-- ----------------------------------------------- -->
## How to start contributing?
You can start contributing by:

- **Help update the documentation**: Since lib-dnml is still in its early stages, API documentation might still be dissatisfactory, and may need cleanup and clarification work

- **Write and run test cases**: Write manual or automated test cases that fact-check or stress-tests APIs are of tremendous importance. Integration into our Rust-based framework for main types are pretty easy, and refactorization and clean-up of the C-based framework is needed. For more details, please look at [Testing Expectations](#testing-expectations)

- **Open an [issue on GitHub](https://github.com/ThienBeos123/dynamol/issues)**: Request a new feature, ask a question about the project, or reporting a bug would be all great contributions to our projects.

- **Implement patches or feature**: To suggest new ideas or features, please open up a [pull request on GitHub](https://github.com/ThienBeos123/dynamol/pulls). For large-contributions affecting the codebase's architecture, please open an issue to receive comments on such matter before continuing. For more details regarding PRs, please look at [Pull Request Guidelines](#pull-request-guidelines)


<!-- ----------------------------------------------- -->
## Development Workflow
The following is the Git branching workflow model for our library
- `main: stable, release-ready code`
- `dev: feature integration with higher-level units`
- `feature/*: new features`
- `refactor/*: infra refactoring`
- `fix/*: bug fixes`

All contributions must target `dev` as the final endpoint, while `main` is only updated on community agreements


<!-- ----------------------------------------------- -->
## Code Standard & Requiremnets
### Code Standard:
```
- Language: C (C99 or later)
- Identation: 4 spaces (no tabs)
- Naming convention:
    + Public API: "type_name"_*
    + Internal type-specific: __"TYPE_NAME"_*__
    + Internal generic: _"function_name"
```
### Legal Upholding <br> 
All source files should start with the following text (with appropriate comment characters and year updated):
```txt
Copyright 20xx-20yy The lib-dnml Project Authors. All Rights Reserved.

Licensed under the Apache License 2.0. You may not use this file except in compliance with the License. You can obtain a copy in the file LICENSE in the source distribution
```
For more information, please refer to [CODE-STANDARDS.md](/CODE-STANDARDS.md)


<!-- ----------------------------------------------- -->
## Pull Request Guidelines

Each PR must include:

- Clear description of change
- Justification
- Complexity analysis (if applicable)
- Edge case considerations

### Required checks:
- Provided code follows our code style
- Passed all automated test cases (if possible)
- Builds successfully (with --strict-wranings on `gcc` and `clang`)
- No memory leaks (Valgrind OR aSan)
- No regressionin existing behavior


<!-- ----------------------------------------------- -->
## Commit Message Guidelines

Each commit message should use prefixes:
- `feat: finish new feature`
- `add: add new feature idea`
- `fix: bug fix`
- `refactor: internal change `
- `improv: improvement (performance OR lexical)`
- `docs: documentation`

### Example:
~~~sh
git commit -m "feat(x86): finish x86_64 clz intrinsic"
git commit -m "add(suite): add testing suite interface"
git commit -m "fix(add64c): fix compiler typo"
git commit -m "refactor(test): test animation refactorization"
git commit -m "improv(karat): karatsuba improved allocation strat"
git commit -m "docs(div): division api documentation"
~~~

### Additional Note:
Multi-line commits with a descriptive body are encouraged for enhanced clarity, and are mandatory for large commits exceeding 500+ total lines changed. Commits' titles should be short and concise, being around 50-80 characters long.


<!-- ----------------------------------------------- -->
## Testing Expectations
### New features:
Contributors should:
- Implement & test edge cases (0, 1, negative, boundaries)
- Validate against known correct results
- Implement randomly-generated test cases against a truth-srouce
- Avoid undefined behavior

Before starting testing such features


### Existing features:
Contributors must either:
- Re-implement test cases and API connections
- Refactor testing interface capability to test such features

Before testing such refactored features

### For more details, please have a look at [TEST_CONDUCT.md](test/TEST_CONDUCT.md)