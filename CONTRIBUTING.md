# Contributing to ODW

ODW is in its architecture/foundation stage. Contributions are welcome, but early changes should protect the project's core invariants rather than maximize feature count.

## Before changing architecture

Read:

- `docs/architecture/principles.md`
- `docs/architecture/overview.md`
- relevant files under `docs/decisions/`

If a change intentionally breaks or weakens an accepted principle, propose an ADR rather than hiding the decision inside implementation code.

## Scope discipline

ODW is a database workspace, not a general-purpose IDE. New integrations should answer a concrete database/data workflow need.

Prefer:

- small replaceable components;
- explicit capabilities;
- visible failure modes;
- reproducible transformations;
- cross-platform behavior;
- tests around persistence and semantic conversion.

Avoid:

- silent semantic loss;
- engine-name conditionals spread through UI code;
- storing secrets in normal workspace files;
- UI-owned database state;
- adding third-party dependencies without license review;
- copying code/assets from the private competitive-research corpus.

## Build (current prototype)

Requirements:

- CMake 3.24+
- C++20 compiler
- Qt 6.5+ Core and Widgets development packages

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
ctest --test-dir build --output-on-failure
```

The prototype currently has no meaningful automated tests; `ctest` is already part of the build gate so tests can be added without changing the workflow contract.

## Commit style

Use short imperative messages with a subsystem prefix when useful, for example:

- `workspace: persist detached window topology`
- `connectors: add capability schema version`
- `docs: record transformation fidelity rules`

## Licensing

Contributions intended for the ODW repository must be compatible with the repository's GNU GPL licensing policy. Do not submit material whose redistribution rights are unclear.
