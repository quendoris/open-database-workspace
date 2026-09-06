# UI capture and visual review

ODW treats screenshots as reproducible engineering artifacts rather than manual decoration.

## Native capture mode

The executable supports a deterministic capture path:

```bash
./build/odw \
  --screenshot artifacts/screenshots/default.png \
  --size 1600x1000 \
  --scenario default \
  --fresh-workspace
```

`--fresh-workspace` prevents a developer's persisted layout from contaminating the capture.

Current scenarios:

- `default` — normal Playground layout;
- `narrow-inspector` — deliberately narrow Inspector to exercise adaptive presentation;
- `automation` — Automation tab visible;
- `vcs` — Version control tab visible.

## Python runner

The convenience runner adds reproducibility metadata:

```bash
python3 tools/screenshots/capture.py default
python3 tools/screenshots/capture.py narrow-inspector
```

For headless CI:

```bash
python3 tools/screenshots/capture.py default --offscreen
```

For every PNG it writes a sibling JSON document containing:

- scenario;
- requested size;
- theme name;
- Git commit and branch;
- UTC capture time;
- platform and Qt platform mode;
- executable path.

Generated captures live under `artifacts/screenshots/` and are intentionally ignored by Git. A screenshot becomes a committed baseline only through an explicit review decision.

## Scope

The current capture mode grabs the ODW main window. It is sufficient for the integrated Playground and CI review. Multi-window composition, detached native windows and multi-monitor scenarios will need a later compositor/capture layer rather than pretending a single-window grab proves those cases.

## Persona Training Lab relationship

ODW reuses the proven principles from Persona Training Lab — screen/shell separation, semantic theme tokens, real-width text elision and reproducible UI states — but does not copy an unavailable or unverified screenshot runner implementation. The ODW runner is deliberately small and native to this project's C++/Qt architecture.
