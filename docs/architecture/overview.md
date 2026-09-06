# ODW architecture overview

ODW is structured as a durable database workspace built around replaceable capabilities rather than a monolithic database client.

The architecture is intentionally split into a small trusted workspace/core layer and replaceable external capability providers.

```text
┌──────────────────────────────────────────────────────────────┐
│                           UI                                 │
│ views · docking · navigation · tables · graphs · editors    │
├──────────────────────────────────────────────────────────────┤
│                     Workspace Engine                         │
│ durable state · layout tree · commands · history · recovery │
├───────────────┬────────────────────┬─────────────────────────┤
│ DB Facade     │ Transformation     │ Automation / VCS        │
│ capabilities  │ planning + ODW-IR  │ orchestration + handoff │
├───────────────┴────────────────────┴─────────────────────────┤
│            Connector / Codec / Runner protocols              │
├──────────────────────────────────────────────────────────────┤
│       isolated providers and external specialist tools       │
│ PostgreSQL · SQLite · MySQL · files · Python · C++ · Git …   │
└──────────────────────────────────────────────────────────────┘
```

## Architectural layers

### 1. UI

The UI renders capabilities and workspace state. It must not own database semantics or become the source of truth for durable state.

A table view, query editor, schema graph, transformation plan, or automation console is a *view* onto logical state managed below it.

### 2. Workspace Engine

The Workspace Engine is responsible for:

- durable workspace state;
- view identity and layout topology;
- recovery after restart or crash;
- undo/redo where semantically valid;
- command routing;
- persistence of drafts and pending operations;
- display-topology reconciliation.

This layer implements the “no unsaved workspace state” invariant.

### 3. Database facade

The database facade presents connector capabilities to the rest of ODW without pretending all engines are identical.

It owns:

- connector discovery;
- capability manifests;
- connection/session lifecycle;
- engine metadata;
- typed database operations;
- engine-specific extension surfaces.

### 4. Transformation engine

The transformation engine plans movement between sources and targets.

Its responsibilities include:

- source and target discovery;
- schema/data mapping;
- ODW intermediate representation (ODW-IR);
- loss classification;
- preview and validation;
- streaming/batched execution;
- resumability where supported;
- reproducible transformation definitions.

### 5. Automation engine

Automation composes database/data operations with external runners such as Python and C++.

The engine orchestrates processes; it is not a general-purpose IDE.

### 6. VCS and external-tool bridge

ODW may understand the minimum VCS concepts needed for database work—repositories, branches, status, diff, history, commits, schema/migration artifacts—and hand off to specialist tools when deeper functionality is needed.

### 7. Providers

Connectors, codecs, and runners are replaceable providers. The preferred architecture isolates fault-prone providers from the main process.

## Control plane and data plane

ODW should not force large datasets through a verbose control protocol.

The architecture distinguishes:

- **control plane** — discovery, capabilities, commands, cancellation, progress, errors, metadata;
- **data plane** — rows, batches, binary values, files, Arrow-like streams/shared memory or another benchmarked high-throughput representation.

The exact protocols are not frozen yet.

## Non-goals

ODW is not intended to become:

- a general-purpose code IDE;
- a Git client replacement;
- a database server;
- a proprietary cloud account platform;
- a wrapper that hides every engine-specific concept behind the lowest common denominator.

## Current stage

This document describes the architecture direction, not a frozen implementation. Decisions that constrain the implementation substantially are recorded as ADRs under `docs/decisions/`.
