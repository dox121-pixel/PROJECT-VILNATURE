#!/usr/bin/env python3
"""
asset_import_pipeline.py
========================
Automates the import of AI-generated 3D meshes and PBR texture sets into the
UE5 Content folder, enforcing naming conventions and generating a manifest.

Workflow:
  1. Scans a source drop folder (default: /tmp/ai_assets/) for meshes + textures.
  2. Renames files to UE5 conventions:
       SM_<Name>.fbx          (static mesh)
       SK_<Name>.fbx          (skeletal mesh)
       T_<Name>_D.png         (diffuse)
       T_<Name>_N.png         (normal)
       T_<Name>_M.png         (mask/ORM)
  3. Copies renamed assets into Content/Characters/, Content/Environment/, etc.
  4. Writes an import_manifest.json for the UE5 Python Editor Script to consume.

Usage:
    python Tools/asset_import_pipeline.py [--source /path/to/drop] [--dry-run]
                                          [--validate-only]
"""

from __future__ import annotations

import argparse
import json
import re
import shutil
import sys
from dataclasses import dataclass, asdict
from pathlib import Path
from typing import Optional

from rich.console import Console
from rich.table import Table

console = Console()

# ── Configuration ─────────────────────────────────────────────────────────────

CONTENT_ROOT = Path("Content")

MESH_EXTENSIONS    = {".fbx", ".obj", ".gltf", ".glb", ".abc"}
TEXTURE_EXTENSIONS = {".png", ".tga", ".exr", ".hdr", ".jpg", ".jpeg"}

# Heuristic keyword → UE5 prefix mapping
MESH_PREFIX_RULES = [
    (re.compile(r"skel|skeleton|rig|character|humanoid", re.I), "SK"),
    (re.compile(r".*"),                                          "SM"),  # default
]

TEXTURE_SUFFIX_RULES = [
    (re.compile(r"diffuse|diff|albedo|color|colour|_d$", re.I), "D"),
    (re.compile(r"normal|nrm|nor|_n$",                  re.I), "N"),
    (re.compile(r"roughness|metallic|orm|mask|_m$",     re.I), "M"),
    (re.compile(r"emissive|emit|glow|_e$",              re.I), "E"),
    (re.compile(r"height|disp|bump|_h$",                re.I), "H"),
]

CATEGORY_FOLDER_RULES = [
    (re.compile(r"character|hero|villain|npc|human",    re.I), "Characters"),
    (re.compile(r"building|structure|wall|floor|city",  re.I), "Environment/Buildings"),
    (re.compile(r"prop|object|item|debris",             re.I), "Environment/Props"),
    (re.compile(r"weapon|gun|sword",                    re.I), "Weapons"),
    (re.compile(r"vfx|particle|fx",                     re.I), "VFX"),
]
DEFAULT_CATEGORY = "Misc"


# ── Data classes ──────────────────────────────────────────────────────────────

@dataclass
class AssetEntry:
    source_path:  str
    dest_path:    str
    asset_type:   str   # "StaticMesh" | "SkeletalMesh" | "Texture2D"
    ue5_name:     str
    category:     str
    renamed:      bool
    issues:       list[str]


# ── Helpers ───────────────────────────────────────────────────────────────────

def slugify(name: str) -> str:
    """Convert arbitrary filename stem to PascalCase safe UE5 name."""
    parts = re.split(r"[\s\-_\.]+", name)
    return "".join(p.capitalize() for p in parts if p)


def infer_mesh_prefix(stem: str) -> str:
    for pattern, prefix in MESH_PREFIX_RULES:
        if pattern.search(stem):
            return prefix
    return "SM"


def infer_texture_suffix(stem: str) -> str:
    for pattern, suffix in TEXTURE_SUFFIX_RULES:
        if pattern.search(stem):
            return suffix
    return "D"  # default: diffuse


def infer_category(name: str) -> str:
    for pattern, cat in CATEGORY_FOLDER_RULES:
        if pattern.search(name):
            return cat
    return DEFAULT_CATEGORY


def validate_asset_name(name: str) -> list[str]:
    """Return a list of naming-convention violations."""
    issues: list[str] = []
    if not re.match(r"^(SM|SK|T)_[A-Z][A-Za-z0-9_]+", name):
        issues.append(f"Name '{name}' does not follow UE5 convention (SM_/SK_/T_ prefix)")
    if len(name) > 64:
        issues.append(f"Name '{name}' is too long ({len(name)} chars; max 64)")
    return issues


# ── Pipeline ──────────────────────────────────────────────────────────────────

def process_mesh(src: Path, dry_run: bool) -> AssetEntry:
    prefix   = infer_mesh_prefix(src.stem)
    ue5_name = f"{prefix}_{slugify(src.stem)}{src.suffix}"
    category = infer_category(src.stem)
    dest_dir = CONTENT_ROOT / category
    dest     = dest_dir / ue5_name

    issues = validate_asset_name(ue5_name.replace(src.suffix, ""))
    asset_type = "SkeletalMesh" if prefix == "SK" else "StaticMesh"

    if not dry_run:
        dest_dir.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, dest)

    return AssetEntry(
        source_path=str(src),
        dest_path=str(dest),
        asset_type=asset_type,
        ue5_name=ue5_name,
        category=category,
        renamed=(src.name != ue5_name),
        issues=issues,
    )


def process_texture(src: Path, dry_run: bool) -> AssetEntry:
    suffix   = infer_texture_suffix(src.stem)
    base     = re.sub(r"[_\-](diffuse|diff|albedo|color|colour|normal|nrm|nor|roughness|"
                      r"metallic|orm|mask|emissive|emit|glow|height|disp|bump|_[a-zA-Z])$",
                      "", src.stem, flags=re.I)
    ue5_name = f"T_{slugify(base)}_{suffix}{src.suffix}"
    category = infer_category(src.stem)
    dest_dir = CONTENT_ROOT / category / "Textures"
    dest     = dest_dir / ue5_name

    issues = validate_asset_name(ue5_name.replace(src.suffix, ""))

    if not dry_run:
        dest_dir.mkdir(parents=True, exist_ok=True)
        shutil.copy2(src, dest)

    return AssetEntry(
        source_path=str(src),
        dest_path=str(dest),
        asset_type="Texture2D",
        ue5_name=ue5_name,
        category=category,
        renamed=(src.name != ue5_name),
        issues=issues,
    )


def run_pipeline(source_dir: Path, dry_run: bool, validate_only: bool) -> list[AssetEntry]:
    if not source_dir.exists():
        console.print(f"[yellow]Source directory '{source_dir}' not found — nothing to import.[/yellow]")
        return []

    entries: list[AssetEntry] = []
    all_files = list(source_dir.rglob("*"))
    asset_files = [f for f in all_files if f.is_file() and
                   f.suffix.lower() in (MESH_EXTENSIONS | TEXTURE_EXTENSIONS)]

    console.print(f"[blue]Found {len(asset_files)} asset(s) in '{source_dir}'[/blue]")

    for src in asset_files:
        ext = src.suffix.lower()
        if ext in MESH_EXTENSIONS:
            entry = process_mesh(src, dry_run=dry_run or validate_only)
        else:
            entry = process_texture(src, dry_run=dry_run or validate_only)
        entries.append(entry)

    # ── Report ────────────────────────────────────────────────────────────────
    table = Table(title="Asset Import Plan", show_lines=True)
    table.add_column("Type")
    table.add_column("Source")
    table.add_column("→ Destination")
    table.add_column("Issues")

    has_errors = False
    for e in entries:
        issue_str = "\n".join(e.issues) if e.issues else "[green]✓[/green]"
        if e.issues:
            has_errors = True
        table.add_row(e.asset_type, e.source_path, e.dest_path, issue_str)

    console.print(table)

    prefix = "[DRY-RUN] " if (dry_run or validate_only) else ""
    console.print(f"{prefix}Processed {len(entries)} assets.")

    # Write manifest
    manifest_path = Path("import_manifest.json")
    with manifest_path.open("w") as f:
        json.dump([asdict(e) for e in entries], f, indent=2)
    console.print(f"Manifest written to [bold]{manifest_path}[/bold]")

    return entries


# ── Entry point ───────────────────────────────────────────────────────────────

def main() -> None:
    parser = argparse.ArgumentParser(description="VilNature AI asset import pipeline")
    parser.add_argument("--source",        default="/tmp/ai_assets",
                        help="Source drop folder containing raw AI-generated assets")
    parser.add_argument("--dry-run",       action="store_true",
                        help="Show what would happen without copying files")
    parser.add_argument("--validate-only", action="store_true",
                        help="Check naming only, do not copy (used in CI)")
    args = parser.parse_args()

    entries = run_pipeline(
        source_dir=Path(args.source),
        dry_run=args.dry_run,
        validate_only=args.validate_only,
    )

    has_errors = any(e.issues for e in entries)
    if has_errors and not args.dry_run:
        console.print("[red]Some assets have naming issues. Review above and fix before importing.[/red]")
        sys.exit(1)


if __name__ == "__main__":
    main()
