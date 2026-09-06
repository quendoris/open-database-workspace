# ADR-0001 — UI platform evaluation

**Status:** Accepted for prototype, not yet frozen for production  
**Date:** 2026-09-06

## Context

ODW requires unusually strong desktop workspace behavior:

- arbitrary tab/split/dock composition;
- detachable native windows;
- multi-monitor movement and restoration;
- large editable data grids;
- low conceptual coupling between UI and database logic;
- Windows, macOS, Linux, Wayland/X11 support;
- localization and accessibility;
- predictable startup/resource behavior;
- GPL-compatible distribution.

The UI choice must be made from these requirements rather than visual preference alone.

## Candidates

### A. C++ + Qt 6 Widgets

Strengths:

- mature cross-platform desktop toolkit;
- native top-level windows and established docking primitives;
- strong model/view facilities for large data-oriented desktop applications;
- mature localization, accessibility, input, clipboard, drag/drop and high-DPI support;
- CMake-native ecosystem;
- open-source Qt is available under LGPLv3/GPLv3 terms suitable for a GPL ODW build when obligations are respected.

Risks:

- Qt APIs alone may not provide every SmartGit-like docking behavior ODW wants;
- platform-specific window-manager behavior still needs direct testing;
- some Qt modules have different open-source licensing availability;
- widget-heavy architecture can accumulate UI coupling if ODW does not keep its workspace model toolkit-independent.

### B. Rust + Tauri 2 + web frontend

Strengths:

- Rust backend and relatively small desktop shell compared with shipping a full browser runtime;
- permissive Tauri licensing (MIT/Apache-2.0);
- strong frontend ecosystem and rapid UI iteration;
- cross-platform desktop support via system webviews;
- natural process/IPC mindset for an isolated-provider architecture.

Risks:

- ODW-style arbitrary docking and detachable workspace behavior would largely be custom application code;
- behavior depends partly on platform webview implementations;
- very large editable grids and multi-window drag/drop need benchmarking rather than assumption;
- native desktop interaction can become split between web UI semantics and host-window semantics.

### C. Qt 6 + KDDockWidgets

KDDockWidgets is architecturally interesting because it directly targets advanced docking and supports Qt 6.

However, its current open-source source headers/build files advertise `GPL-2.0-only OR GPL-3.0-only`, with commercial licensing also available. ODW currently intends its own code to be `GPL-3.0-or-later`. This combination requires a deliberate licensing review before KDDockWidgets becomes a dependency; the project must not casually assume that “both are GPL” resolves the exact-version terms.

Therefore KDDockWidgets is an **evaluation dependency**, not an adopted foundation dependency.

## Decision

The **first executable UI prototype will use C++ + Qt 6 Widgets**.

This is a prototype decision, not permission to let Qt own ODW's architecture.

The workspace layout tree, persistent state model, connector contracts, transformation model, and automation model remain toolkit-independent.

A small Tauri 2 comparison prototype may be built specifically for measurements where webview behavior is a plausible advantage or risk.

KDDockWidgets will not be added to the production dependency graph until both its behavior and exact license interaction with ODW's chosen licensing policy are reviewed.

## Prototype exit criteria

Qt becomes the production UI platform only if the prototype demonstrates all of the following without architectural hacks:

1. tab, horizontal split and vertical split composition;
2. detach/re-attach into native top-level windows;
3. reliable multi-monitor placement and missing-monitor recovery;
4. persistence/restoration from an ODW-owned layout model;
5. large-grid scrolling/editing performance on representative datasets;
6. acceptable startup time and idle memory;
7. correct keyboard/IME/clipboard behavior;
8. acceptable Windows, macOS, X11 and Wayland behavior;
9. localization and accessibility paths compatible with ODW principles;
10. no requirement for UI-layer knowledge inside connector/transformation core logic.

## Measurements

The prototype should record rather than merely describe:

- cold/warm startup time;
- idle RSS/private working set;
- grid load and scroll latency at several row/column scales;
- layout save/restore latency;
- window detach/attach failure cases;
- monitor removal/re-add behavior;
- package size;
- platform-specific defects.

## Consequences

- Initial implementation work can begin with Qt 6 and CMake.
- ODW's core APIs must remain independent of Qt types unless a later ADR explicitly changes that boundary.
- No KDDockWidgets dependency is introduced by this ADR.
- Tauri remains a benchmark/control candidate, not a rejected technology.

## Evidence checked

- Qt 6 licensing documentation: https://doc.qt.io/qt-6/licensing.html
- Qt open-source obligations overview: https://www.qt.io/development/open-source-lgpl-obligations
- Tauri architecture and licensing: https://tauri.app/concept/architecture/
- KDDockWidgets source licensing declaration: https://github.com/KDAB/KDDockWidgets/blob/main/CMakeLists.txt
