# Appearance tool

Status: planned user-facing tool; not implemented in the first shell.

ODW's appearance is intentionally configurable, but customization is not scattered through unrelated preferences. A dedicated **Appearance** tool owns visual customization and previews the same semantic tokens consumed by the live workspace.

## Goals

The tool must let a user change the visual character of ODW without changing application semantics or requiring a restart.

Primary controls:

- base/canvas surface;
- panel and raised surfaces;
- primary/secondary accent;
- text and muted-text contrast;
- subtle/active borders;
- selection color;
- focus/glow intensity;
- border radius;
- global density and spacing;
- grid row height;
- editor/grid font families and sizes;
- animation level;
- high-contrast/accessibility preset.

## Interaction model

Appearance changes are live and durable.

There is no Save button:

1. user changes a token/control;
2. workspace preview updates immediately;
3. the application records the new appearance state durably;
4. undo/reset remain explicit operations;
5. exporting a theme profile creates a separate external artifact.

Cancel is not a disguised Save model. If the tool eventually supports experimental editing sessions, those sessions must be represented explicitly as temporary previews and must not create ambiguous unsaved state.

## Profiles

The initial built-in profile is `ODW Dark / Cyan`.

Future built-ins may include:

- dark high contrast;
- light neutral;
- system-following profile;
- low-glow/zero-glow profile.

User profiles are data composed from semantic tokens. Importing a profile must validate its schema before application.

## Theme schema direction

A future serializable profile should resemble:

```text
appearance.profile
├── schemaVersion
├── metadata
├── colors
│   ├── surface.*
│   ├── text.*
│   ├── border.*
│   ├── accent.*
│   └── semantic status colors
├── geometry
│   ├── radius.*
│   ├── spacing.*
│   └── density
├── typography
│   ├── ui
│   ├── editor
│   └── grid
└── effects
    ├── glow
    └── animation
```

Exact serialization is intentionally deferred until the first UI prototype demonstrates which tokens are genuinely semantic.

## Guardrails

Customization must not make core states impossible to distinguish.

The Appearance tool should warn when a profile makes, for example:

- primary text indistinguishable from the canvas;
- active focus indistinguishable from inactive borders;
- destructive/error state indistinguishable from normal state;
- selection unreadable;
- editor text fail basic contrast checks.

Warnings should not turn the tool into an arbitrary style police system; advanced users may deliberately choose unusual profiles.

## Architectural boundary

Views request semantic roles. They do not know which concrete theme/profile supplies those roles.

```text
Appearance profile
      ↓
Semantic visual tokens
      ↓
Theme adapter
      ↓
Workspace/views
```

This separation is required so the same workspace can later be rendered through a different UI toolkit without changing the meaning of a stored appearance profile.
