# Automation model

Automation in ODW exists to make database and dataset workflows reproducible without turning the application into a general-purpose IDE.

## Graph model

A workflow is composed from typed nodes and edges:

```text
PostgreSQL query
      ↓
Python runner
      ↓
Validate / map
      ↓
C++ runner
      ↓
SQLite target
      ↓
CSV export
```

Database reads/writes, transformations, codecs, and external runners should participate in the same orchestration model.

## Runner isolation

User code must not execute inside the main ODW process.

Initial runner targets:

- Python;
- C++ build + execute through a configured toolchain.

Likely future runners include JavaScript, R, and shell environments when they serve real data workflows.

## Runner contract

A runner should expose:

- runtime/toolchain discovery;
- environment declaration;
- input/output bindings;
- stdout/stderr streaming;
- exit status;
- cancellation;
- time/resource limits where the platform permits;
- structured artifacts and diagnostics;
- reproducible invocation metadata.

## Data exchange

Small control values may use the normal control protocol. Large datasets should use the same high-throughput data-plane abstractions as connectors/transformation jobs where practical.

The user should not have to export a temporary CSV merely to pass a dataset from a query to a Python operation.

## Security boundary

Automation code is arbitrary local code. ODW must not pretend it is safe merely because it was launched from a graphical workflow.

The architecture should make capabilities explicit, including:

- filesystem access;
- network access;
- passed credentials/connections;
- environment variables;
- working directory;
- resource limits.

Sandboxing depth is platform-dependent and will require separate decisions.

## Reproducibility

A workflow definition should capture enough information to reproduce intent:

- connector/codec/runner identifiers and versions;
- scripts or script references;
- input/output bindings;
- parameters;
- transformation mappings;
- environment/toolchain requirements;
- explicit external dependencies.

Secrets are referenced, never embedded directly in portable workflow definitions.

## Editor boundary

ODW may provide a focused script editor with syntax support sufficient for data automation, but it must preserve a clean handoff to a user's preferred IDE/editor for serious software development.
