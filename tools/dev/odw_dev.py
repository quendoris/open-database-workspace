#!/usr/bin/env python3
"""Small cross-platform development/publish helper for ODW."""

from __future__ import annotations

import argparse
import shutil
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
BUILD = ROOT / "build"
BLOCKED_PUBLIC_SUFFIXES = {".exe", ".msi", ".dll", ".7z", ".rar"}


def run(command: list[str], *, check: bool = True) -> subprocess.CompletedProcess[str]:
    print("+", " ".join(command))
    return subprocess.run(command, cwd=ROOT, text=True, check=check)


def output(command: list[str]) -> str:
    return subprocess.check_output(command, cwd=ROOT, text=True).strip()


def current_branch() -> str:
    return output(["git", "branch", "--show-current"])


def ensure_feature_branch() -> None:
    branch = current_branch()
    if not branch:
        raise SystemExit("Detached HEAD: publishing is intentionally refused.")
    if branch in {"main", "master"}:
        raise SystemExit(f"Refusing to create a development commit directly on {branch}.")


def guard_public_tree() -> None:
    tracked = output(["git", "ls-files"]).splitlines()
    blocked = [path for path in tracked if Path(path).suffix.lower() in BLOCKED_PUBLIC_SUFFIXES]
    if blocked:
        formatted = "\n  ".join(blocked)
        raise SystemExit(
            "Public-tree guard rejected research/binary-like files:\n  " + formatted
        )


def check() -> None:
    guard_public_tree()
    run(["git", "diff", "--check"])
    run(["cmake", "-S", ".", "-B", str(BUILD), "-DCMAKE_BUILD_TYPE=Debug"])
    run(["cmake", "--build", str(BUILD), "--parallel", "2"])
    run(["ctest", "--test-dir", str(BUILD), "--output-on-failure"])


def stage_and_commit(message: str) -> None:
    ensure_feature_branch()
    run(["git", "add", "-A"])
    staged = subprocess.run(["git", "diff", "--cached", "--quiet"], cwd=ROOT, check=False)
    if staged.returncode == 0:
        raise SystemExit("Nothing staged to commit.")
    run(["git", "commit", "-m", message])


def push() -> None:
    ensure_feature_branch()
    run(["git", "push", "-u", "origin", "HEAD"])


def open_pr() -> None:
    ensure_feature_branch()
    if shutil.which("gh") is None:
        raise SystemExit("GitHub CLI (gh) is required only for the `pr`/`publish` command.")

    existing = subprocess.run(
        ["gh", "pr", "view", "--json", "url", "--jq", ".url"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        check=False,
    )
    if existing.returncode == 0 and existing.stdout.strip():
        print(existing.stdout.strip())
        return
    run(["gh", "pr", "create", "--fill", "--base", "main"])


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    subparsers = parser.add_subparsers(dest="command", required=True)

    subparsers.add_parser("check", help="Run public-tree guard, configure, build and tests.")

    commit_parser = subparsers.add_parser("commit", help="Check, stage and commit on a feature branch.")
    commit_parser.add_argument("message")

    subparsers.add_parser("push", help="Push the current feature branch to origin.")
    subparsers.add_parser("pr", help="Open or print the current GitHub pull request using gh.")

    publish_parser = subparsers.add_parser(
        "publish",
        help="Check, commit, push and open a pull request as one reproducible flow.",
    )
    publish_parser.add_argument("message")

    args = parser.parse_args()

    if args.command == "check":
        check()
    elif args.command == "commit":
        check()
        stage_and_commit(args.message)
    elif args.command == "push":
        push()
    elif args.command == "pr":
        open_pr()
    elif args.command == "publish":
        check()
        stage_and_commit(args.message)
        push()
        open_pr()

    return 0


if __name__ == "__main__":
    raise SystemExit(main())
