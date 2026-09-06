# ODW architectural principles

These principles are constraints on the product, not a feature wishlist. A feature that violates them must justify an architectural decision record (ADR) before implementation.

## A1 — No unsaved workspace state

ODW must not rely on a user pressing **Save** to preserve application/workspace state.

If the user can observe or continue from a state, ODW must either persist it durably or treat the action as not having happened.

This applies to workspace layout, open views, editor contents, filters, pending transformations, automation graphs, connection configuration, navigation state, and other ODW-owned state.

Database transaction semantics are separate from this rule.

## A2 — Persistence, database commit, and export are different operations

ODW must not conflate:

- **persistence** — preserving ODW-owned state;
- **commit/apply** — changing an external database according to that database engine's semantics;
- **export** — creating an external artifact or representation.

A database may still require explicit transactions and commits. Export remains an explicit creation of an external artifact. Neither is a substitute for saving workspace state.

## A3 — Database support is capability-based

ODW must never reduce support for a database engine to one boolean flag.

Each connector must expose a capability manifest describing what it can actually introspect, query, edit, administer, transform, stream, explain, migrate, and preserve.

The UI must be driven by capabilities rather than hard-coded engine-name conditionals wherever practical.

## A4 — Semantic loss must never be hidden

Conversions between databases or data formats must classify unsupported, lossy, approximated, and exact mappings before destructive execution whenever technically possible.

ODW must prefer an explicit incomplete conversion over a silently corrupted successful conversion.

## A5 — Workspace composition is arbitrary and durable

Views must be composable as tabs, splits, docked panels, detached windows, and multi-monitor layouts without changing the logical objects they represent.

Moving a view must not implicitly clone sessions, transactions, or data-processing state.

The workspace layout must survive normal restart and recover sensibly when display topology changes.

## A6 — Fault-prone extensions are isolated

Database connectors, automation runners, and similar extension points should execute outside the main UI/core process unless a documented reason requires otherwise.

A failed driver, Python process, or user C++ program must not take down the ODW workspace.

Control-plane communication and data-plane transport may use different protocols.

## A7 — Universality must not erase database semantics

ODW should provide common interaction patterns, but engine-specific capabilities must remain accessible.

PostgreSQL, SQLite, MySQL, Redis, document databases, and future systems are not interchangeable bags of tables. The abstraction layer must preserve native concepts rather than flattening them away.

## A8 — Integrate specialist tools instead of cloning them badly

ODW may integrate with Git, GitHub, SmartGit, native database tools, and other specialist software when that improves the workflow.

The project must resist recreating entire external products inside ODW without a clear database-workspace reason.

## A9 — Localization is architectural

User-visible strings, formatting, pluralization, shortcuts, layouts, and documentation hooks must be designed for localization from the start.

Internationalization is not a post-release string-replacement pass.

## A10 — ODW is a database workspace, not a general-purpose IDE

Automation, scripting, VCS integration, and extensibility exist to improve work with databases and datasets.

They must not turn ODW into a generic programming environment. When a specialist editor or IDE is better, ODW should hand off context cleanly instead of absorbing that product's entire scope.

## Decision rule

When two designs provide similar capability, prefer the one with:

1. less hidden state;
2. fewer required concepts for the user;
3. clearer failure and recovery behavior;
4. better preservation of source semantics;
5. lower coupling between UI, connectors, and data processing;
6. stronger cross-platform behavior;
7. easier testing and replacement of components.
