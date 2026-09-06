# Adaptive views

Status: active design contract for the ODW Playground.

ODW must not force users to protect the interface from their own layout choices. If a view can be resized or detached, it must remain coherent throughout its supported size range.

## Core rule

> ODW never sacrifices information hierarchy for available space.

When space becomes scarce, a view changes presentation before it becomes visually broken.

The preferred order is:

1. preserve the primary meaning;
2. reduce secondary decoration;
3. shorten labels where the meaning remains clear;
4. elide long values according to the **actual rendered width**;
5. expose the full value through hover/tooltip or explicit expansion;
6. switch to a compact presentation when necessary;
7. enforce a semantic minimum only when going smaller would make the tool unusable.

A permanently scrolling marquee is not the default solution for technical UI. Moving text competes for attention during long work sessions and makes comparison harder.

## Presentation policy

Views may define three presentation bands:

- **Compact** — narrow but still semantically useful;
- **Normal** — the normal working representation;
- **Expanded** — additional labels/context when space permits.

The thresholds belong to the view's presentation policy, not to the current text contents and not to arbitrary global constants.

The first Adaptive Inspector prototype uses this model. Its long values are rendered through `ElidedLabel`, which calculates elision from the current container width and exposes the complete value only when clipping occurs.

## Minimum size

A minimum dimension is semantic, not aesthetic.

Examples:

- an Inspector may become a vertically stacked key/value view before it reaches its minimum width;
- a result grid may keep horizontal scrolling because columns are themselves meaningful dimensions;
- an editor must retain enough area for text navigation and caret context;
- a connection tree may elide object names while preserving hierarchy.

The workspace compositor should eventually read these policies from view descriptors rather than knowing concrete view classes.

## Automation and regression

Adaptive behavior needs reproducible visual scenarios. The Playground therefore supports deterministic presentation scenarios such as `narrow-inspector`, and the screenshot runner records both the image and metadata tied to the Git commit.

A later UI regression layer may add structural checks (no off-screen controls, no overlap, minimum readable hit targets) in addition to image review.
