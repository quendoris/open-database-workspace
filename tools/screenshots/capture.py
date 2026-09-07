#!/usr/bin/env python3
"""Capture deterministic ODW workspace screenshots with build metadata."""

from __future__ import annotations

import argparse
import datetime as dt
import json
import os
import platform
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

SCENARIOS: dict[str, dict[str, str]] = {
    "default": {"size": "1600x1000", "scenario": "default"},
    "narrow-inspector": {"size": "1100x720", "scenario": "narrow-inspector"},
    "automation": {"size": "1440x900", "scenario": "automation"},
    "query": {"size": "1440x900", "scenario": "query"},
}


def git_value(*args: str) -> str:
    result = subprocess.run(
        ["git", *args],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.DEVNULL,
        check=False,
    )
    return result.stdout.strip() if result.returncode == 0 else "unknown"


def find_executable(explicit: str | None) -> Path:
    if explicit:
        path = Path(explicit).expanduser().resolve()
        if path.exists():
            return path
        raise SystemExit(f"ODW executable does not exist: {path}")

    candidates = [
        ROOT / "build" / "odw",
        ROOT / "build" / "odw.exe",
        ROOT / "build" / "Debug" / "odw.exe",
        ROOT / "build" / "Release" / "odw.exe",
    ]
    for candidate in candidates:
        if candidate.exists():
            return candidate
    raise SystemExit("ODW executable not found. Build the project first or use --executable.")


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("scenario", choices=sorted(SCENARIOS))
    parser.add_argument("--executable")
    parser.add_argument("--output")
    parser.add_argument("--size", help="Override the scenario size (WIDTHxHEIGHT).")
    parser.add_argument(
        "--offscreen",
        action="store_true",
        help="Use Qt's offscreen platform plugin; intended for CI captures.",
    )
    args = parser.parse_args()

    spec = SCENARIOS[args.scenario]
    size = args.size or spec["size"]
    executable = find_executable(args.executable)

    output = (
        Path(args.output).expanduser()
        if args.output
        else ROOT / "artifacts" / "screenshots" / f"{args.scenario}.png"
    )
    if not output.is_absolute():
        output = ROOT / output
    output = output.resolve()
    output.parent.mkdir(parents=True, exist_ok=True)

    env = os.environ.copy()
    if args.offscreen:
        env["QT_QPA_PLATFORM"] = "offscreen"

    command = [
        str(executable),
        "--screenshot",
        str(output),
        "--size",
        size,
        "--scenario",
        spec["scenario"],
        "--fresh-workspace",
    ]
    print("+", " ".join(command))
    subprocess.run(command, cwd=ROOT, env=env, check=True)

    metadata = {
        "schemaVersion": 1,
        "scenario": args.scenario,
        "presentationScenario": spec["scenario"],
        "size": size,
        "theme": "ODW Dark / Cyan",
        "commit": git_value("rev-parse", "HEAD"),
        "branch": git_value("branch", "--show-current"),
        "capturedAtUtc": dt.datetime.now(dt.timezone.utc).isoformat(),
        "platform": platform.platform(),
        "qtPlatform": env.get("QT_QPA_PLATFORM", "native"),
        "executable": str(executable),
    }
    metadata_path = output.with_suffix(".json")
    metadata_path.write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")

    print(f"captured: {output}")
    print(f"metadata: {metadata_path}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
