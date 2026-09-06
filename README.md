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

The architectural principles and decisions live under [`docs/`](docs/).

## Licensing

ODW is free software. The project is licensed under **GNU GPL-3.0-or-later**.

## Research separation

Competitive analysis, third-party binaries, screenshots, and other research material are intentionally kept outside this public repository. This repository contains only material that belongs in the distributable ODW project.
