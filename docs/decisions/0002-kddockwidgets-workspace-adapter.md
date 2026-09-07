# ADR 0002 — Advanced docking adapter for the document workspace

Status: accepted for the executable prototype

## Context

The first Qt Widgets playground used `QMainWindow` + `QDockWidget` directly. That prototype was useful because it exposed a structural mismatch early:

- the central area remained a special non-dockable canvas;
- document-like views were forced into peripheral dock areas;
- tab groups did not provide the detachable-tab behavior ODW requires;
- the resulting layout could collapse useful views into narrow strips around a large empty center;
- a view being a `QDockWidget` became too visible in the product model.

This violates the intended ODW workspace grammar. Tables, queries and automation runs are workspace documents. They must be able to live in the center, become tabs, split, move and detach without changing their logical identity.

## Decision

Use **KDDockWidgets 2.4.1** as the current Qt Widgets docking adapter.

The dependency is pinned and built with the Qt Widgets frontend only. KDDockWidgets is GPL-2.0-only OR GPL-3.0-only, which is compatible with ODW's GPL-3.0-or-later distribution policy when used under GPL-3.0.

ODW uses a document-style central group:

- `odw.documents` affinity — tables, views, queries and automation documents;
- `odw.tools` affinity — Connections, Inspector and other contextual tools.

Documents are first-class detachable tabs. Tool panes remain side docks and may also be moved or detached.

## Product consequences

The default workspace no longer reserves a large decorative central canvas.

Opening `public.users`, for example, creates or raises a document tab containing the object's data and structure. Opening a query creates a query document. Automation opens as a document only when requested.

Git/VCS is explicitly **not** a permanent document or permanent tool pane. Repository integration is contextual and will attach to a workspace/connection workflow only when configured.

## Persistence boundary

KDDockWidgets layout serialization is an adapter detail, not the canonical long-term ODW workspace schema.

For the prototype, its serialized layout is embedded inside ODW's atomically written workspace state so the no-unsaved-state invariant remains testable. The canonical ODW layout model remains the toolkit-independent tree under `src/core/workspace`.

Dynamic document reconstruction will later move behind an ODW view/document factory. The fixed mock document set in this prototype is registered up front so current layouts can be restored deterministically.

## Rejected alternative

Continuing to patch `QMainWindow`/`QDockWidget` was rejected. The prototype demonstrated that doing so would require reimplementing detachable document tabs, center docking and multi-window behavior that already exist in a maintained GPL-compatible library.

## Evidence

KDDockWidgets explicitly supports:

- center docking without a traditional central widget;
- detachable tabs in a center document group;
- arbitrary tab detachment into dock areas;
- grouped floating windows;
- docking across multiple main windows;
- Wayland, X11, Windows and macOS;
- layout save/restore.

The choice is still an adapter choice. If later benchmarking shows unacceptable behavior or cost, ODW's logical workspace model must remain replaceable without changing database/document semantics.
