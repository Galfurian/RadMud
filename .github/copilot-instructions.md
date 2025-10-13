---
applyTo: '**'
---

# General Instructions


## C++ Coding Guidelines

This project follows the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines). Key points:

- **Type Safety:** Prefer `auto` for type inference, avoid unsafe casts, and use strong types where possible.
- **Resource Management:** Use RAII and smart pointers (`std::unique_ptr`, `std::shared_ptr`) instead of raw pointers. Avoid manual memory management.
- **Initialization:** Always initialize variables upon declaration. Prefer constructor member initializer lists.
- **Const Correctness:** Use `const` wherever possible for variables, pointers, and member functions.
- **Error Handling:** Prefer exceptions for error handling. Avoid using raw pointers or error codes for signaling errors.
- **Functions:** Keep functions short and focused. Pass by value for small types, by (const) reference for large types. Use `noexcept` where appropriate.
- **Classes:** Use `class` for types with invariants, `struct` for passive objects. Make data members private.
- **Templates & Generics:** Prefer templates for generic code. Use concepts or `static_assert` for constraints.
- **Standard Library:** Prefer standard library facilities over custom implementations.
- **Naming:** Use descriptive names. Classes/types: `CamelCase`. Functions/variables: `snake_case`.
- **Comments & Documentation:** Write clear comments for non-obvious code. Document public APIs with Doxygen-style comments.
- **Code Style:** Follow consistent formatting (indentation, braces, spacing). Use tools like `clang-format`.

For more details, see the [C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines).


## GIT Repository Management

### Commits

Use the Conventional Commits format: `<type>(scope): short summary`

Examples:

- `feature(config): support dynamic environment loading`
- `fix(core): handle missing config file gracefully`
- `test(utils): add unit tests for retry logic`

Allowed types (use these as `<type>` in your commit messages):

- `feature` – New features
- `fix` – Bug fixes
- `documentation` – Documentation changes only
- `style` – Code style, formatting, missing semi-colons, etc. (no code meaning changes)
- `refactor` – Code changes that neither fix a bug nor add a feature
- `performance` – Code changes that improve performance
- `test` – Adding or correcting tests
- `build` – Changes to build system or external dependencies
- `ci` – Changes to CI configuration files and scripts
- `chore` – Maintenance tasks (e.g., updating dependencies, minor tooling)
- `revert` – Reverting previous commits
- `security` – Security-related improvements or fixes
- `ux` – User experience or UI improvements

Other Notes:

- Prefer simple, linear Git history. Use rebase over merge where possible.
- Use `pre-commit` hooks to enforce formatting, linting, and checks before commits.
- If unsure about a change, open a draft PR with a summary and rationale.

### Release Guidelines

We follow a simplified Git-flow model for releases:

#### Branches

- `main`: Represents the latest stable, released version. Only hotfixes and release merges are committed directly to `main`.
- `develop`: Integration branch for ongoing development. All new features and bug fixes are merged into `develop`.
- `feature/<feature-name>`: Used for developing new features. Branch off `develop` and merge back into `develop` upon completion.

Here is the release process:

1. Prepare `develop` for Release:
    - Ensure all desired features and bug fixes are merged into `develop`.
    - Update `CHANGELOG.md` with changes for the new version, with the help of the command: `git log --pretty=format:"- (%h) %s" ...`
    - Update version numbers in relevant project files (e.g., `pyproject.toml`, `package.json`).
2. Make sure we start from a clean state:
    - Make sure you are on the `develop`, and that we start from there.
    - Perform final testing and bug fixing on this branch.
3. Merge to `main` and Tag:
    - Once the develop branch is stable, merge it into `main`:
      1. `git checkout main`
      2. `git merge --no-ff develop`  
    - Tag the release on `main`: `git tag -a v<version-number> -m "Release v<version-number>"`
    - Ask the user to push the changes to the `main` branch, including tags: `git push origin main --tags`
4. Merge back to `develop`:
    - Merge the main branch back into `develop` to ensure `develop` has all release changes:
      1. `git checkout develop`
      2. `git merge --no-ff main`
    - Ask the user to push the changes to `develop` branch: `git push origin develop`