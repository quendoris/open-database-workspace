# Open Database Workspace (ODW)

**Open Database Workspace** is a free and open-source, cross-platform workspace for exploring, querying, transforming, automating, and managing databases.

ODW is being designed around a simple idea: working with data should be direct, durable, composable, and understandable. It should not require a heavyweight general-purpose IDE, hidden application state, or artificial product tiers.

> **Status:** architecture and foundation stage. ODW is not yet ready for production use.

## Core direction

ODW is intended to provide:

- a durable workspace with no manually saved application state;
- capability-aware support for different database engines instead of a binary “supported / unsupported” label;
- loss-aware conversion between databases and data formats;
- freely dockable, detachable, multi-window workspace composition;
- isolated database connectors and automation runners;
- reproducible data-processing pipelines;
- integration with Git and specialist external tools without becoming a general-purpose IDE;
- localization as a first-class architectural concern.

The foundation is documented in:

- [`docs/architecture/principles.md`](docs/architecture/principles.md) — project invariants;
- [`docs/architecture/overview.md`](docs/architecture/overview.md) — system boundaries;
- [`docs/architecture/persistence.md`](docs/architecture/persistence.md) — no-unsaved-state model;
- [`docs/architecture/connectors.md`](docs/architecture/connectors.md) — capability-based connectors;
- [`docs/architecture/transformation.md`](docs/architecture/transformation.md) — ODW-IR and conversion fidelity;
- [`docs/architecture/workspace.md`](docs/architecture/workspace.md) — detachable durable workspace;
- [`docs/architecture/automation.md`](docs/architecture/automation.md) — isolated automation runners;
- [`docs/decisions/`](docs/decisions/) — architecture decision records.

## Current executable prototype

The first shell intentionally tests architecture rather than visual design. It uses **C++20 + Qt 6 Widgets** and currently provides movable, tabbable, detachable database/query/result/inspector views.

There is deliberately no Save command. Window geometry, Qt docking state, and the SQL draft are persisted automatically through an atomic workspace-state write. This temporary prototype persistence format is not the final ODW workspace model.

### Build

Requirements:

- CMake 3.24+
- C++20 compiler
- Qt 6.4+ (`Core`, `Widgets`)

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
./build/odw
```

On multi-config generators the executable path may differ.

## Architecture rule of thumb

ODW does not try to collect the maximum number of features. A feature belongs when it makes database/data work clearer, safer, more reproducible, or more capable without destroying the simplicity of the workspace.

## Licensing

ODW is free software. Project code is intended to be licensed under **GNU GPL-3.0-or-later**. See [`LICENSE`](LICENSE).

Dependencies are reviewed separately for exact license compatibility before adoption.

## Research separation

Competitive analysis, third-party binaries, screenshots, and other research material are intentionally kept outside this public repository. This repository contains only material that belongs in the distributable ODW project.

See [`CONTRIBUTING.md`](CONTRIBUTING.md) before submitting code or third-party material.
