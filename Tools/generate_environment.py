#!/usr/bin/env python3
"""
generate_environment.py
=======================
Procedural environment block-out generator for PROJECT-VILNATURE.

Generates a JSON layout of modular building / street pieces that the
UE5 Python Editor Script (PythonScriptPlugin) can consume to auto-place
actors in the level.  Can also export a Datasmith-compatible CSV for
direct import.

Layout algorithm:
  - City grid with variable block sizes
  - Building footprints randomised per block
  - Street widths follow a configurable distribution
  - Destruction-target buildings tagged with DestructionComponent hint

Usage:
    python Tools/generate_environment.py [--blocks 5] [--seed 42] [--out layout.json]
"""

from __future__ import annotations

import argparse
import json
import math
import random
from dataclasses import dataclass, field, asdict
from pathlib import Path
from typing import Optional

from rich.console import Console

console = Console()

# ── Configuration ─────────────────────────────────────────────────────────────

BLOCK_SIZE_MIN   = 4000    # cm
BLOCK_SIZE_MAX   = 8000    # cm
STREET_WIDTH     = 1200    # cm  (12 m road)
BUILDING_PADDING = 300     # cm  setback from road edge

BUILDING_HEIGHT_RANGES = {
    "skyscraper": (15000, 40000),
    "midrise":    (4000, 14000),
    "lowrise":    (1000, 3999),
    "warehouse":  (800,  1500),
}

BUILDING_MESH_PRESETS = {
    "skyscraper": ["SM_Building_SkyscraperA", "SM_Building_SkyscraperB"],
    "midrise":    ["SM_Building_MidriseA",    "SM_Building_MidriseB", "SM_Building_MidriseC"],
    "lowrise":    ["SM_Building_LowriseA",    "SM_Building_LowriseB"],
    "warehouse":  ["SM_Building_WarehouseA"],
}

DESTRUCTION_PROBABILITY = 0.4   # fraction of buildings that get DestructionComponent


# ── Data classes ──────────────────────────────────────────────────────────────

@dataclass
class Transform:
    x: float
    y: float
    z: float
    yaw: float = 0.0
    scale_x: float = 1.0
    scale_y: float = 1.0
    scale_z: float = 1.0


@dataclass
class ActorEntry:
    actor_class:   str
    mesh_asset:    str
    transform:     Transform
    tags:          list[str] = field(default_factory=list)
    components:    list[str] = field(default_factory=list)
    label:         str = ""


@dataclass
class EnvironmentLayout:
    seed:         int
    grid_size:    int
    actor_count:  int
    actors:       list[ActorEntry] = field(default_factory=list)


# ── Generator ─────────────────────────────────────────────────────────────────

def choose_building_type(rng: random.Random) -> str:
    return rng.choices(
        ["skyscraper", "midrise", "lowrise", "warehouse"],
        weights=[0.10, 0.45, 0.35, 0.10],
    )[0]


def block_origin(col: int, row: int, block_w: float, block_h: float) -> tuple[float, float]:
    """World position of the bottom-left corner of a city block (in cm)."""
    x = col * (block_w + STREET_WIDTH)
    y = row * (block_h + STREET_WIDTH)
    return x, y


def generate_building(
    rng: random.Random,
    block_x: float,
    block_y: float,
    block_w: float,
    block_h: float,
) -> ActorEntry:
    btype  = choose_building_type(rng)
    h_min, h_max = BUILDING_HEIGHT_RANGES[btype]
    height = rng.uniform(h_min, h_max)
    mesh   = rng.choice(BUILDING_MESH_PRESETS[btype])

    # Random position within block bounds (with setback)
    pad = BUILDING_PADDING
    bx = rng.uniform(block_x + pad, block_x + block_w - pad)
    by = rng.uniform(block_y + pad, block_y + block_h - pad)
    yaw = rng.choice([0.0, 90.0, 180.0, 270.0])

    # Scale building to fill roughly the appropriate footprint
    footprint_scale = rng.uniform(0.8, 1.4)
    height_scale    = height / h_max   # normalised

    transform = Transform(
        x=bx, y=by, z=0.0,
        yaw=yaw,
        scale_x=footprint_scale,
        scale_y=footprint_scale,
        scale_z=height_scale,
    )

    tags       = [btype, "Building"]
    components = []

    if rng.random() < DESTRUCTION_PROBABILITY:
        tags.append("Destructible")
        components.append("DestructionComponent")

    return ActorEntry(
        actor_class="ADestructibleBuilding" if "Destructible" in tags else "AStaticMeshActor",
        mesh_asset=f"/Game/Environment/Buildings/{mesh}.{mesh}",
        transform=transform,
        tags=tags,
        components=components,
        label=f"{mesh}_{int(bx)}_{int(by)}",
    )


def generate_street_light(x: float, y: float, spacing: float) -> list[ActorEntry]:
    lights = []
    for i in range(int(spacing // 2000)):
        actor = ActorEntry(
            actor_class="AStaticMeshActor",
            mesh_asset="/Game/Environment/Props/SM_StreetLight.SM_StreetLight",
            transform=Transform(x=x + i * 2000, y=y, z=0.0),
            tags=["Prop", "StreetLight"],
            label=f"StreetLight_{int(x)}_{i}",
        )
        lights.append(actor)
    return lights


def generate_layout(grid_n: int, seed: int) -> EnvironmentLayout:
    rng = random.Random(seed)
    layout = EnvironmentLayout(seed=seed, grid_size=grid_n, actor_count=0)

    for row in range(grid_n):
        for col in range(grid_n):
            block_w = rng.uniform(BLOCK_SIZE_MIN, BLOCK_SIZE_MAX)
            block_h = rng.uniform(BLOCK_SIZE_MIN, BLOCK_SIZE_MAX)
            bx, by  = block_origin(col, row, block_w, block_h)

            # 2-4 buildings per block
            num_buildings = rng.randint(2, 4)
            for _ in range(num_buildings):
                layout.actors.append(generate_building(rng, bx, by, block_w, block_h))

            # Street lights along block edges
            layout.actors.extend(generate_street_light(bx, by - STREET_WIDTH * 0.5, block_w))

    layout.actor_count = len(layout.actors)
    return layout


# ── Export ────────────────────────────────────────────────────────────────────

def export_json(layout: EnvironmentLayout, out_path: Path) -> None:
    data = {
        "meta": {
            "seed":       layout.seed,
            "grid_size":  layout.grid_size,
            "actor_count": layout.actor_count,
        },
        "actors": [asdict(a) for a in layout.actors],
    }
    with out_path.open("w") as f:
        json.dump(data, f, indent=2)
    console.print(f"[green]Layout written to '{out_path}'[/green]")


def export_csv(layout: EnvironmentLayout, out_path: Path) -> None:
    """Datasmith-compatible CSV for direct UE5 import."""
    import csv

    if not layout.actors:
        console.print("[yellow]No actors to export to CSV.[/yellow]")
        return

    rows = []
    for a in layout.actors:
        t = a.transform
        rows.append({
            "Name":      a.label,
            "MeshAsset": a.mesh_asset,
            "PosX":      round(t.x, 2),
            "PosY":      round(t.y, 2),
            "PosZ":      round(t.z, 2),
            "RotYaw":    t.yaw,
            "ScaleX":    round(t.scale_x, 3),
            "ScaleY":    round(t.scale_y, 3),
            "ScaleZ":    round(t.scale_z, 3),
            "Tags":      "|".join(a.tags),
            "Components": "|".join(a.components),
        })

    with out_path.open("w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=list(rows[0].keys()))
        writer.writeheader()
        writer.writerows(rows)

    console.print(f"[green]CSV layout written to '{out_path}'[/green]")


# ── Main ──────────────────────────────────────────────────────────────────────

def main() -> None:
    parser = argparse.ArgumentParser(description="VilNature procedural environment generator")
    parser.add_argument("--blocks", type=int, default=5,   help="Grid dimension (NxN blocks)")
    parser.add_argument("--seed",   type=int, default=42,  help="Random seed")
    parser.add_argument("--out",    default="Content/Maps/layout.json",
                        help="Output JSON file path")
    parser.add_argument("--csv",    action="store_true",
                        help="Also export a Datasmith-compatible CSV")
    args = parser.parse_args()

    console.print(f"[blue]Generating {args.blocks}×{args.blocks} city grid (seed={args.seed})…[/blue]")
    layout = generate_layout(args.blocks, args.seed)
    console.print(f"Generated [bold]{layout.actor_count}[/bold] actors.")

    out_path = Path(args.out)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    export_json(layout, out_path)

    if args.csv:
        csv_path = out_path.with_suffix(".csv")
        export_csv(layout, csv_path)


if __name__ == "__main__":
    main()
