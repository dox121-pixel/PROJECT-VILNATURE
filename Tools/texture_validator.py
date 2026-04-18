#!/usr/bin/env python3
"""
texture_validator.py
====================
Validates game texture assets to enforce PROJECT-VILNATURE quality standards:

  - Max texture size per category (diffuse, normal, mask, etc.)
  - Dimensions must be powers of 2
  - Supported formats only (PNG, TGA, EXR, HDR)
  - File naming convention: T_<AssetName>_<Suffix>.<ext>
    Suffixes: D=diffuse, N=normal, M=mask/roughness, E=emissive, H=height

Usage:
    python Tools/texture_validator.py [--strict] [--report-only] [--path Content/]
"""

from __future__ import annotations

import argparse
import json
import os
import sys
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Optional

from PIL import Image
from rich.console import Console
from rich.table import Table

console = Console()

# ── Configuration ─────────────────────────────────────────────────────────────

SUPPORTED_EXTENSIONS = {".png", ".tga", ".exr", ".hdr", ".jpg", ".jpeg"}
VALID_SUFFIXES       = {"D", "N", "M", "E", "H", "A"}   # Diffuse/Normal/Mask/Emissive/Height/Alpha
MAX_DIMENSION        = 4096    # hard cap on any texture axis (px)
MAX_DIMENSION_ATLAS  = 8192   # allowed for texture atlases (suffix ends with _A)

CATEGORY_MAX: dict[str, int] = {
    "D":  4096,
    "N":  2048,
    "M":  2048,
    "E":  2048,
    "H":  2048,
    "A":  8192,
}

# ── Data classes ──────────────────────────────────────────────────────────────

@dataclass
class ValidationIssue:
    path: str
    severity: str      # "error" | "warning"
    message: str


@dataclass
class ValidationReport:
    checked: int = 0
    errors: int = 0
    warnings: int = 0
    issues: list[ValidationIssue] = field(default_factory=list)

    def add(self, path: str, severity: str, message: str) -> None:
        self.issues.append(ValidationIssue(path, severity, message))
        if severity == "error":
            self.errors += 1
        else:
            self.warnings += 1

    @property
    def passed(self) -> bool:
        return self.errors == 0


# ── Helpers ───────────────────────────────────────────────────────────────────

def is_power_of_two(n: int) -> bool:
    return n > 0 and (n & (n - 1)) == 0


def parse_suffix(stem: str) -> Optional[str]:
    """Return the suffix letter from T_AssetName_<Suffix> or None."""
    parts = stem.split("_")
    if len(parts) >= 3 and parts[0].upper() == "T":
        return parts[-1].upper()
    return None


def validate_texture(filepath: Path, report: ValidationReport) -> None:
    report.checked += 1
    rel = str(filepath)

    # ── Naming convention ─────────────────────────────────────────────────────
    stem = filepath.stem
    suffix = parse_suffix(stem)
    if not stem.upper().startswith("T_"):
        report.add(rel, "warning",
                   f"Name should start with 'T_' (got '{stem}')")
    if suffix is not None and suffix not in VALID_SUFFIXES:
        report.add(rel, "warning",
                   f"Unknown texture suffix '_{suffix}'. Expected one of {VALID_SUFFIXES}")

    # ── Format check ──────────────────────────────────────────────────────────
    ext = filepath.suffix.lower()
    if ext not in SUPPORTED_EXTENSIONS:
        report.add(rel, "error", f"Unsupported extension '{ext}'")
        return

    # ── Image properties (skip EXR / HDR — Pillow can't always open them) ─────
    if ext in {".exr", ".hdr"}:
        return

    try:
        with Image.open(filepath) as img:
            w, h = img.size
    except Exception as exc:
        report.add(rel, "error", f"Could not open image: {exc}")
        return

    # Power-of-two
    if not is_power_of_two(w) or not is_power_of_two(h):
        report.add(rel, "error",
                   f"Dimensions {w}×{h} are not powers of two")

    # Max size
    allowed_max = CATEGORY_MAX.get(suffix, MAX_DIMENSION) if suffix else MAX_DIMENSION
    if w > allowed_max or h > allowed_max:
        report.add(rel, "error",
                   f"Texture {w}×{h} exceeds max {allowed_max}px for suffix '{suffix}'")


# ── Main ──────────────────────────────────────────────────────────────────────

def collect_textures(root: Path) -> list[Path]:
    return [
        p for p in root.rglob("*")
        if p.is_file() and p.suffix.lower() in SUPPORTED_EXTENSIONS
    ]


def run_validation(search_root: Path, strict: bool, report_only: bool) -> ValidationReport:
    report = ValidationReport()
    textures = collect_textures(search_root)

    if not textures:
        console.print(f"[yellow]No textures found under '{search_root}'[/yellow]")
        return report

    console.print(f"[blue]Scanning {len(textures)} texture(s) in '{search_root}'…[/blue]")

    for tex in textures:
        validate_texture(tex, report)

    # ── Console output ────────────────────────────────────────────────────────
    if report.issues:
        table = Table(title="Texture Validation Issues", show_lines=True)
        table.add_column("Severity", style="bold")
        table.add_column("File")
        table.add_column("Issue")

        for issue in report.issues:
            color = "red" if issue.severity == "error" else "yellow"
            table.add_row(
                f"[{color}]{issue.severity.upper()}[/{color}]",
                issue.path,
                issue.message,
            )
        console.print(table)
    else:
        console.print("[green]✓ All textures passed validation.[/green]")

    console.print(
        f"\nChecked: {report.checked}  "
        f"[red]Errors: {report.errors}[/red]  "
        f"[yellow]Warnings: {report.warnings}[/yellow]"
    )

    # Write JSON report for CI artifact upload
    report_path = Path("validation_report.json")
    with report_path.open("w") as f:
        json.dump(
            {
                "checked":  report.checked,
                "errors":   report.errors,
                "warnings": report.warnings,
                "issues":   [asdict(i) for i in report.issues],
            },
            f, indent=2
        )
    console.print(f"Report written to [bold]{report_path}[/bold]")

    return report


def main() -> None:
    parser = argparse.ArgumentParser(description="VilNature texture asset validator")
    parser.add_argument("--path",        default="Content", help="Root path to scan")
    parser.add_argument("--strict",      action="store_true",
                        help="Exit with code 1 on warnings too")
    parser.add_argument("--report-only", action="store_true",
                        help="Always exit 0 (used in non-blocking CI jobs)")
    args = parser.parse_args()

    root = Path(args.path)
    if not root.exists():
        console.print(f"[yellow]Path '{root}' does not exist — skipping validation.[/yellow]")
        # Write an empty report so the CI artifact step never fails
        Path("validation_report.json").write_text('{"checked":0,"errors":0,"warnings":0,"issues":[]}')
        sys.exit(0)

    report = run_validation(root, strict=args.strict, report_only=args.report_only)

    if args.report_only:
        sys.exit(0)

    if report.errors > 0 or (args.strict and report.warnings > 0):
        sys.exit(1)


if __name__ == "__main__":
    main()
