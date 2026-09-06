# Workspace model

The ODW workspace is a persistent logical composition, not a collection of transient widget coordinates.

## Layout tree

A first-order layout can be represented as a tree of windows, splits, and tab groups:

```text
Workspace
└── Window
    └── Split(horizontal)
        ├── TabGroup
        │   ├── DatabaseNavigator
        │   └── SchemaExplorer
        └── Split(vertical)
            ├── QueryEditor
            └── TabGroup
                ├── ResultGrid
                └── ExplainPlan
```

The exact serialized schema is not frozen, but the logical model must be toolkit-independent enough to survive UI implementation changes.

## View identity

Moving a view changes presentation topology, not the identity of the underlying logical object.

For example, detaching a query editor onto a second monitor must not implicitly duplicate:

- the editor document;
- its connection binding;
- an active database session;
- a transaction;
- pending result state.

## State separation

A view may reference several categories of state:

### Shared logical state

Examples: editor document, selected database object, transformation plan.

### Managed session state

Examples: connection/session handles, server cursors, transaction bindings.

### View-local state

Examples: scroll position, selected grid cell, local zoom, column widths.

### Transaction-bound state

State whose meaning depends on a particular database transaction or session must be explicit and must not be duplicated by layout operations.

## Composition requirements

A first release target should support:

- tabbing views together;
- horizontal and vertical splits;
- moving views between groups;
- detaching views into independent native windows;
- moving windows across monitors;
- reattaching detached views;
- deterministic close/reopen behavior;
- restoring layout automatically.

## Display topology changes

If a monitor disappears, ODW must reconcile off-screen windows into the currently available display topology. Restoring an old workspace must never strand essential UI outside the visible desktop.

## Perspectives

Named or automatically remembered layout perspectives may be useful, but they are not a substitute for durable live state. Switching perspectives changes composition; it does not introduce a manual Save model.

## Toolkit boundary

The workspace tree is an ODW model. Qt docking APIs, web-layout libraries, or another UI toolkit are adapters that render this model. Toolkit-specific serialized geometry must not become the sole source of truth.
