# ODW visual system

Status: active design direction for the first user-facing shell.

ODW is a human-first database workspace. Its visual system exists to make dense technical work calm, legible, composable, and pleasant for long sessions. It must not become a decorative skin over an old desktop layout, and it must not make AI the organizing principle of the product.

## 1. Default character

The default ODW appearance is:

- dark graphite, not pure black;
- cyan as the primary focus/accent color;
- restrained luminous edges rather than large neon surfaces;
- low-noise panels with clear depth;
- compact but breathable information density;
- minimal chrome;
- strong active/inactive distinction without distracting animation.

The desired impression is a modern technical workspace, not a gaming RGB theme and not a legacy IDE.

## 2. User-first rule

The primary navigation model is always built around the user's work:

- connections;
- schemas and objects;
- query/data views;
- transformations;
- automation;
- history and provenance;
- inspection and diagnostics.

AI, if present, is an optional capability. It does not own the shell, occupy permanent prime screen space, or redefine normal database workflows around an agent.

## 3. Appearance is data, not hard-coded decoration

The default graphite/cyan theme is only the default. Appearance is represented as a set of semantic tokens so a dedicated Appearance tool can later modify it without rewriting widgets.

Required configurable families:

- base/background surfaces;
- panel/surface elevation;
- primary and secondary accent;
- foreground and muted text;
- border and focus intensity;
- glow intensity;
- corner radius;
- spacing/density;
- row height;
- editor and grid typography;
- animation level;
- contrast mode.

Widgets consume semantic roles such as `surface.panel`, `border.active`, or `accent.primary`; they must not scatter literal colors throughout implementation code.

## 4. Default tokens

Initial dark/cyan direction (values are implementation defaults, not a frozen branding contract):

| Role | Default |
|---|---|
| `surface.canvas` | `#0B1014` |
| `surface.panel` | `#10171D` |
| `surface.raised` | `#151E25` |
| `surface.hover` | `#19252D` |
| `text.primary` | `#E7F1F5` |
| `text.secondary` | `#91A5AE` |
| `border.subtle` | `#21313A` |
| `border.active` | `#28D7E5` |
| `accent.primary` | `#28D7E5` |
| `accent.soft` | `#123941` |
| `selection` | `#174A52` |
| `danger` | `#FF6B7A` |
| `warning` | `#E7B85C` |
| `success` | `#63D69A` |

Cyan should identify focus, selection, active context, and important interactive affordances. It should not fill large areas of the UI.

## 5. Edges and glow

Glow is semantic feedback, not decoration.

Use glow for:

- active dock/view boundary;
- selected tab;
- keyboard focus;
- active drag target;
- important transient state.

Do not glow every panel simultaneously. Inactive boundaries remain quiet.

## 6. Workspace composition

Docking must visually feel spatial and deterministic.

During drag/rearrangement:

- valid insertion regions are obvious;
- the destination geometry is previewed before drop;
- neighboring content must not jump unpredictably;
- detached windows preserve the same visual language as the main shell;
- active window/view context remains obvious across multiple monitors.

The logical workspace model remains toolkit-independent. Visual chrome must never become the source of truth for layout state.

## 7. Data grid

The grid is a first-class surface, not a generic table widget.

Design requirements:

- restrained separators;
- high text contrast;
- clear selected cell / selected row / edited cell distinction;
- sticky, legible headers;
- predictable keyboard navigation;
- density control;
- column type cues that do not waste space;
- state and validation shown without turning every cell into a colored badge.

Large datasets must remain visually calm.

## 8. Editors

Query/script editors use a dedicated editor surface. Syntax color should be clear but less saturated than the primary cyan UI accent so code does not compete with shell focus state.

Editor tabs are workspace objects and follow the same detachable/composable rules as other views.

## 9. Appearance tool

Customization belongs in a dedicated Appearance tool rather than being distributed across random settings pages.

It should eventually provide:

- live preview;
- semantic color editing;
- density and spacing controls;
- radius and border controls;
- glow intensity;
- editor/grid-specific typography;
- import/export of theme profiles;
- reset to ODW defaults;
- accessible presets with stronger contrast.

Changes obey the ODW persistence invariant: there is no Save button. Editing appearance changes durable workspace/application state immediately; exporting a theme is a separate operation.

## 10. Reference policy

Existing products may demonstrate that a quality level or interaction is achievable. They are evidence, not templates.

ODW may learn from modern products such as Tabularis, and from the scalable workspace principles previously explored in our other projects, but must derive its own hierarchy, composition, visual grammar, and user model.

We do not clone another client's layout or brand language.
