# Development automation

ODW keeps repetitive repository operations in a small, reviewable Python helper rather than relying on memorized shell sequences.

## Local gate

```bash
python3 tools/dev/odw_dev.py check
```

The gate currently runs:

1. public-tree guard (rejects obviously misplaced executable/archive research artifacts);
2. `git diff --check`;
3. CMake configure;
4. build;
5. CTest.

## Commit and push

```bash
python3 tools/dev/odw_dev.py commit "ui: improve adaptive inspector"
python3 tools/dev/odw_dev.py push
```

The helper refuses development commits directly on `main`/`master`.

## GitHub pull request

PR creation uses the official GitHub CLI only for the network action:

```bash
python3 tools/dev/odw_dev.py pr
```

If the current branch already has a PR, the helper prints it instead of opening a duplicate.

## One-command publish flow

```bash
python3 tools/dev/odw_dev.py publish "ui: improve adaptive inspector"
```

This performs check → stage → commit → push → PR.

The script never stores a GitHub token. Git authentication remains owned by Git/`gh` and the user's normal credential setup.
