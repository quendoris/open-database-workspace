# Persistence model

ODW treats durable workspace persistence as a core correctness property.

## Principle

If ODW presents a state to the user as existing, that state must either be durably represented or explicitly transient by design.

The application must not depend on a global manual Save command for normal workspace continuity.

## Three distinct operations

### Workspace persistence

Persists ODW-owned state such as:

- editor drafts;
- open views and their identities;
- layout topology;
- filters and sorting;
- navigation position;
- transformation definitions;
- automation graphs;
- pending change sets;
- recent execution context.

This should happen automatically and incrementally.

### Database commit/apply

Changes an external database according to that engine's transaction and consistency semantics.

ODW must preserve explicit transactions where the database supports them. Automatic workspace persistence must never silently commit an external transaction.

### Export

Creates an external artifact or representation. Export is explicit because it has an external destination, format, overwrite policy, and semantic-conversion boundary.

## Journal direction

The preferred model is a small durable operation/state journal backed by periodic compact snapshots.

Conceptually:

```text
workspace snapshot N
      ↓
operation journal
  + open view
  + edit draft
  + move panel
  + change filter
  + define transform
      ↓
workspace snapshot N+1
```

The exact storage engine is undecided. SQLite is an obvious candidate and will be benchmarked against simpler append-only formats.

## Crash behavior

After abnormal termination ODW should restore the latest durable workspace state without asking the user to reconstruct which editors, layouts, or pending plans were previously open.

External database operations are recovered according to the database's own semantics. ODW must never fabricate a successful commit because its local journal contains an intended operation.

## Undo/redo

Undo/redo is not equivalent to persistence.

Workspace-local actions may be reversible through the command/journal model. External database mutations require explicit inverse operations, transactions, or snapshots and must not be advertised as reversible unless ODW can prove that property.

## Secret handling

Credentials and other secrets must not be written directly into general workspace persistence. Secret storage will use a dedicated abstraction with platform keychain/credential-store support where available.
