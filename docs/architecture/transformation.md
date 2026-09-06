# Transformation engine and ODW-IR

ODW treats migration, import/export, and dataset transformation as one family of operations rather than unrelated wizards.

## Pipeline model

```text
Source
  ↓
Connector / Codec
  ↓
ODW-IR
  ↓
Transform / Mapping
  ↓
ODW-IR
  ↓
Connector / Codec
  ↓
Target
```

A source or target may be a database, query result, table subset, file, archive, or future provider type.

## ODW-IR goals

The intermediate representation must preserve enough semantics to reason about conversion quality. It is not merely a generic table structure.

It may represent:

- scalar and composite types;
- nullability;
- keys and constraints;
- relationships;
- indexes;
- generated/default expressions;
- schemas/namespaces;
- views and materialized views;
- engine-native metadata where portable representation is impossible;
- row/batch data streams.

## Fidelity classification

Every mapping should be classifiable before execution when enough metadata is available.

Initial vocabulary:

- `EXACT` — semantics preserved;
- `SAFE_COERCION` — representation changes without expected semantic loss;
- `LOSSY_COERCION` — some semantics/precision are lost;
- `REQUIRES_MAPPING` — user or policy choice is required;
- `UNSUPPORTED` — target cannot represent the source concept;
- `UNKNOWN` — ODW cannot prove the conversion quality.

A successful transfer with hidden semantic loss is considered a correctness failure.

## Planning before execution

The engine should produce a plan containing:

- discovered source objects;
- selected target;
- type/object mappings;
- conflicts;
- unsupported features;
- estimated data volume where possible;
- transactional guarantees;
- overwrite/create behavior;
- resumability guarantees.

Execution begins from an explicit plan, even though the plan itself is automatically persisted as workspace state.

## Streaming

Large datasets must not require complete materialization in application memory.

Connectors and codecs should support bounded batches, backpressure, cancellation, and progress reporting.

## Automation integration

A transformation plan should be serializable as an automation node or reproducible workflow. The user should not have to choose between “GUI import/export” and “automation” as separate conceptual systems.

## Round-trip honesty

ODW must not promise arbitrary round-trip equivalence between incompatible engines. Instead it must expose what was preserved, transformed, omitted, or made engine-specific.
