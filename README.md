# PROJECT-VILNATURE

> **Goal:** Build a complete, production-ready 3D game using **only** GitHub AI tooling — from the first line of code to the final shipped build.
> **Graphics goal variant:** For AAA-quality photorealistic visuals, see the [Unreal Engine 5 — High-Quality Graphics](#unreal-engine-5--high-quality-graphics) sections below.

---

## Table of Contents

1. [Overview](#overview)
2. [GitHub AI Tools at a Glance](#github-ai-tools-at-a-glance)
3. [GitHub Copilot — Code Generation](#1-github-copilot--code-generation)
4. [GitHub Copilot Chat — Architecture & Debugging](#2-github-copilot-chat--architecture--debugging)
5. [GitHub Copilot in the CLI](#3-github-copilot-in-the-cli)
6. [GitHub Copilot Extensions & Workspace Agents](#4-github-copilot-extensions--workspace-agents)
7. [GitHub Models — In-Game AI & LLM Integration](#5-github-models--in-game-ai--llm-integration)
8. [GitHub Actions + AI — CI/CD & Automated Testing](#6-github-actions--ai--cicd--automated-testing)
9. [GitHub Codespaces — Cloud Development Environment](#7-github-codespaces--cloud-development-environment)
10. [GitHub Copilot for Pull Requests & Code Review](#8-github-copilot-for-pull-requests--code-review)
11. [Recommended 3D Engine Workflows](#recommended-3d-engine-workflows)
12. [End-to-End Game Development Workflow](#end-to-end-game-development-workflow)
13. [Prompt Engineering Guide for Game Development](#prompt-engineering-guide-for-game-development)
14. [Asset & Content Pipeline with GitHub AI](#asset--content-pipeline-with-github-ai)
15. [Multiplayer & Backend with GitHub AI](#multiplayer--backend-with-github-ai)
16. [Performance, Profiling & Optimization with AI](#performance-profiling--optimization-with-ai)
17. [Publishing & Deployment](#publishing--deployment)
18. [Resources & References](#resources--references)
19. **Unreal Engine 5 — High-Quality Graphics**
    - 19.1 [Why UE5 for AAA Graphics](#191-why-ue5-for-aaa-graphics)
    - 19.2 [UE5 Graphics Feature Setup](#192-ue5-graphics-feature-setup)
    - 19.3 [UE5 Material System with Copilot](#193-ue5-material-system-with-copilot)
    - 19.4 [Procedural Content Generation (PCG) with GitHub Models](#194-procedural-content-generation-pcg-with-github-models)
    - 19.5 [UE5 Python Editor Scripting](#195-ue5-python-editor-scripting)
    - 19.6 [Self-Hosted GitHub Actions Runner for UE5](#196-self-hosted-github-actions-runner-for-ue5)
    - 19.7 [Git LFS Strategy for Large Binary Assets](#197-git-lfs-strategy-for-large-binary-assets)
    - 19.8 [MetaHuman + Copilot](#198-metahuman--copilot)
    - 19.9 [UE5 Cinematic Pipeline](#199-ue5-cinematic-pipeline)
    - 19.10 [UE5 End-to-End Graphics Workflow](#1910-ue5-end-to-end-graphics-workflow)
    - 19.11 [Godot 4 vs UE5 — Key Trade-offs](#1911-godot-4-vs-ue5--key-trade-offs)

---

## Overview

PROJECT-VILNATURE is a 3D game project that serves as both a playable experience and a **living demonstration** of how every GitHub AI capability can be applied to real-world game development.

Every system in this game — engine scaffolding, physics, shaders, NPC dialogue, procedural world generation, CI/CD pipelines, and more — is authored with GitHub AI assistance.

---

## GitHub AI Tools at a Glance

| Tool | Primary Game Dev Use |
|---|---|
| **GitHub Copilot (IDE)** | Real-time code completion for game logic, physics, shaders |
| **GitHub Copilot Chat** | Architecture decisions, refactoring, step-by-step debugging |
| **Copilot in the CLI** | Terminal commands, build scripts, Git workflows |
| **Copilot Extensions** | Domain-specific agents (Godot, Unity, Unreal, Three.js) |
| **GitHub Models** | In-game LLM-powered NPCs, procedural story, dynamic events |
| **GitHub Actions + AI** | Automated testing, build matrix, deployment pipelines |
| **GitHub Codespaces** | Zero-setup cloud IDE preconfigured for 3D development |
| **Copilot PR summaries** | Automated PR descriptions, review suggestions, diff analysis |

---

## 1. GitHub Copilot — Code Generation

GitHub Copilot is the foundation. It generates code inline as you type, reducing boilerplate and letting you focus on game design.

### 1.1 Game Logic & Core Systems

Write a comment describing what you want, and Copilot writes the implementation:

```gdscript
# GDScript (Godot 4) — Character controller with gravity and jumping
# Copilot prompt: "3D character controller with coyote time and variable jump height"
extends CharacterBody3D

const SPEED       = 8.0
const JUMP_FORCE  = 6.0
const GRAVITY     = -20.0
const COYOTE_TIME = 0.12

var _coyote_timer  := 0.0
var _jump_buffered := false

func _physics_process(delta: float) -> void:
    if not is_on_floor():
        velocity.y += GRAVITY * delta
        _coyote_timer -= delta
    else:
        _coyote_timer = COYOTE_TIME

    if Input.is_action_just_pressed("jump"):
        if is_on_floor() or _coyote_timer > 0:
            velocity.y = JUMP_FORCE
            _coyote_timer = 0

    var dir := Input.get_vector("move_left", "move_right", "move_forward", "move_back")
    var move := (transform.basis * Vector3(dir.x, 0, dir.y)).normalized()
    velocity.x = move.x * SPEED
    velocity.z = move.z * SPEED
    move_and_slide()
```

### 1.2 Physics & Collision

```gdscript
# Copilot prompt: "Ragdoll death animation using physics bodies in Godot 4"
func activate_ragdoll() -> void:
    for bone_idx in skeleton.get_bone_count():
        var pb := PhysicalBone3D.new()
        pb.bone_name = skeleton.get_bone_name(bone_idx)
        skeleton.add_child(pb)
    skeleton.physical_bones_start_simulation()
```

### 1.3 Shader Code (GLSL / Godot Shader Language)

```glsl
// Copilot prompt: "Stylized toon water shader with animated normals and foam at edges"
shader_type spatial;
render_mode unshaded;

uniform sampler2D normal_map : hint_normal;
uniform vec4 water_color : source_color = vec4(0.1, 0.4, 0.7, 0.85);
uniform float wave_speed  = 1.5;
uniform float foam_cutoff = 0.92;

void fragment() {
    vec2 uv     = UV + vec2(TIME * wave_speed * 0.02, TIME * wave_speed * 0.015);
    vec3 normal = texture(normal_map, uv).rgb * 2.0 - 1.0;
    float depth = texture(DEPTH_TEXTURE, SCREEN_UV).r;
    float foam  = 1.0 - step(foam_cutoff, depth); // foam at shallow/near-surface edges
    ALBEDO      = mix(water_color.rgb, vec3(1.0), foam);
    ALPHA       = water_color.a;
    NORMAL      = normalize(normal);
}
```

### 1.4 AI / NPC Behavior (Finite State Machine)

```gdscript
# Copilot prompt: "Enemy FSM with patrol, chase, and attack states using NavigationAgent3D"
class_name EnemyFSM
extends Node

enum State { PATROL, CHASE, ATTACK, DEAD }

var state       : State = State.PATROL
var nav_agent   : NavigationAgent3D
var player      : Node3D
var attack_range := 1.8
var chase_range  := 12.0

func _physics_process(delta: float) -> void:
    match state:
        State.PATROL: _patrol(delta)
        State.CHASE:  _chase(delta)
        State.ATTACK: _attack(delta)

func _patrol(_delta: float) -> void:
    if player and owner.global_position.distance_to(player.global_position) < chase_range:
        state = State.CHASE

func _chase(_delta: float) -> void:
    nav_agent.set_target_position(player.global_position)
    if owner.global_position.distance_to(player.global_position) <= attack_range:
        state = State.ATTACK
    elif owner.global_position.distance_to(player.global_position) > chase_range * 1.5:
        state = State.PATROL
```

### 1.5 Procedural World Generation

```gdscript
# Copilot prompt: "Chunk-based infinite terrain using FastNoiseLite in Godot 4"
var noise := FastNoiseLite.new()

func generate_chunk(chunk_pos: Vector2i) -> ArrayMesh:
    noise.noise_type  = FastNoiseLite.TYPE_PERLIN
    noise.frequency   = 0.04
    var st := SurfaceTool.new()
    st.begin(Mesh.PRIMITIVE_TRIANGLES)
    for x in range(CHUNK_SIZE):
        for z in range(CHUNK_SIZE):
            var wx := chunk_pos.x * CHUNK_SIZE + x
            var wz := chunk_pos.y * CHUNK_SIZE + z
            var h  := noise.get_noise_2d(wx, wz) * HEIGHT_SCALE
            st.add_vertex(Vector3(x, h, z))
    st.generate_indices()
    st.generate_normals()
    return st.commit()
```

### 1.6 Inventory & Item Systems

```gdscript
# Copilot prompt: "Generic inventory system with stacking, equipping, and serialization"
class_name Inventory
extends Resource

@export var slots: Array[ItemStack] = []
const MAX_SLOTS := 24

func add_item(item: ItemData, amount: int = 1) -> bool:
    for slot in slots:
        if slot.item == item and slot.count < item.max_stack:
            slot.count = min(slot.count + amount, item.max_stack)
            return true
    if slots.size() < MAX_SLOTS:
        slots.append(ItemStack.new(item, amount))
        return true
    return false
```

### 1.7 Save / Load System

```gdscript
# Copilot prompt: "JSON save/load system for player progress in Godot 4"
func save_game(slot: int = 0) -> void:
    var data := {
        "player_pos":   {"x": player.global_position.x, "y": player.global_position.y, "z": player.global_position.z},
        "health":       player.health,
        "inventory":    player.inventory.serialize(),
        "timestamp":    Time.get_unix_time_from_system(),
    }
    var file := FileAccess.open("user://save_%d.json" % slot, FileAccess.WRITE)
    file.store_string(JSON.stringify(data, "\t"))
```

---

## 2. GitHub Copilot Chat — Architecture & Debugging

Use Copilot Chat directly inside your IDE (VS Code, JetBrains, Neovim) to have a conversation about your game's design and bugs.

### 2.1 Architecture Decisions

Ask Copilot Chat:

```
@workspace What's the best architecture for a 3D open-world game in Godot 4?
Compare ECS, scene-tree composition, and signals-based patterns.
```

```
@workspace How should I structure my project directories for a Godot 4 3D RPG?
```

### 2.2 Step-by-Step Debugging

```
I'm getting jittery movement in my CharacterBody3D at high frame rates.
Here is my _physics_process code: [paste code]
What is causing this, and how do I fix it?
```

### 2.3 Refactoring & Code Review

```
/refactor  — Simplify this shader, eliminate redundant texture samples
/explain   — Walk me through how this NavigationServer3D mesh baking works
/fix       — This method throws a null reference when the player respawns
/tests     — Generate unit tests for my Inventory class
/doc       — Generate GDDoc comments for all public methods in this script
```

### 2.4 Performance Analysis

```
@workspace My game drops below 60 fps when 50+ enemies are active.
Suggest profiling strategies and optimizations for Godot 4.
```

### 2.5 Shader Debugging

```
My water shader produces z-fighting artifacts on slopes.
Here is the GLSL code: [paste shader]
Why does this happen, and what's the fix?
```

---

## 3. GitHub Copilot in the CLI

Use the `gh copilot` CLI extension to get AI assistance directly in your terminal.

### Installation

```bash
gh extension install github/gh-copilot
```

### Common Game Dev Uses

```bash
# Generate a Godot export preset for Linux/Windows/Web
gh copilot suggest "create a Godot 4 export_presets.cfg for Linux, Windows, and HTML5"

# Explain a complex build error
gh copilot explain "error: undefined reference to 'GDNativeExtensionClassLibraryPtr'"

# Write a Makefile target that builds and packages the game
gh copilot suggest "Makefile target to build Godot 4 PCK file and zip with assets"

# Generate a git pre-commit hook that validates GDScript syntax
gh copilot suggest "git pre-commit hook that runs gdscript linter on staged .gd files"

# Ask how to set up a Godot headless CI server
gh copilot suggest "Docker command to run Godot 4 headless export in GitHub Actions"
```

---

## 4. GitHub Copilot Extensions & Workspace Agents

### 4.1 `@workspace` Agent

The `@workspace` agent has full context of your entire repository.

```
@workspace List all scripts that use NavigationAgent3D and check for missing await calls.
```

```
@workspace The player's health bar is not updating after taking damage.
Trace the signal chain from Player.take_damage() to HUD.update_health_bar().
```

### 4.2 Third-Party Copilot Extensions

Install domain-specific Copilot Extensions from GitHub Marketplace:

| Extension | Use Case |
|---|---|
| **Godot Copilot** | GDScript autocompletion, scene tree awareness |
| **Shader Forge AI** | GLSL/HLSL generation and optimization |
| **Blender AI** | Python scripting for Blender asset pipelines |
| **Unity Copilot** | C# MonoBehaviour generation, Editor scripting |
| **Unreal AI** | Blueprints to C++ conversion, gameplay tags |

### 4.3 Custom Copilot Extension (Build Your Own)

Create a `.github/copilot-instructions.md` to give Copilot permanent context about your game:

```markdown
# Copilot Instructions — PROJECT-VILNATURE

## Project
A 3D open-world nature exploration game built in Godot 4.3.

## Conventions
- Use GDScript (typed, @export annotations required on all public properties)
- All nodes follow snake_case naming
- Signals use past-tense verbs: `health_changed`, `item_collected`
- Physics objects must call move_and_slide() in _physics_process only
- Every public function must have a GDDoc comment

## Architecture
- Autoloads: GameManager, EventBus, SaveManager, AudioManager
- Scenes are composed, never inherited (prefer composition over inheritance)
- No @onready assignments outside _ready()
```

---

## 5. GitHub Models — In-Game AI & LLM Integration

[GitHub Models](https://github.com/marketplace/models) gives you access to frontier LLMs (GPT-4o, o1, Llama, Mistral, Phi, etc.) via a free API endpoint — perfect for powering in-game AI features.

### 5.1 Setup

```bash
# Install the OpenAI-compatible SDK
pip install openai          # Python
npm install openai          # JavaScript/TypeScript
```

```python
# GitHub Models uses the OpenAI-compatible API
from openai import OpenAI

client = OpenAI(
    base_url="https://models.inference.ai.azure.com",
    api_key=os.environ["GITHUB_TOKEN"],
)
```

### 5.2 LLM-Powered NPC Dialogue

```python
# Copilot prompt: "NPC dialogue system backed by GitHub Models GPT-4o"
def get_npc_response(npc_name: str, personality: str, player_message: str, history: list) -> str:
    system_prompt = f"""You are {npc_name}, a character in a 3D nature game.
Personality: {personality}
Stay in character. Keep responses under 3 sentences.
Reference the player's surroundings (forests, rivers, wildlife) naturally."""

    messages = [{"role": "system", "content": system_prompt}] + history
    messages.append({"role": "user", "content": player_message})

    response = client.chat.completions.create(
        model="gpt-4o",
        messages=messages,
        max_tokens=150,
        temperature=0.8,
    )
    return response.choices[0].message.content
```

### 5.3 Procedural Quest Generation

```python
# Copilot prompt: "Generate dynamic quests using an LLM based on world state"
def generate_quest(world_state: dict) -> dict:
    prompt = f"""World state: {json.dumps(world_state)}
Generate a unique side quest as JSON:
{{
  "title": "...",
  "description": "...",
  "objectives": ["...", "..."],
  "reward": {{"gold": N, "item": "..."}}
}}"""
    response = client.chat.completions.create(
        model="gpt-4o-mini",
        messages=[{"role": "user", "content": prompt}],
        response_format={"type": "json_object"},
    )
    return json.loads(response.choices[0].message.content)
```

### 5.4 Dynamic Environmental Narration

```python
# Copilot prompt: "AI narrator that describes the player's surroundings in real time"
def narrate_environment(biome: str, time_of_day: str, weather: str, nearby_objects: list) -> str:
    prompt = f"""Describe the following game environment in 1–2 vivid, atmospheric sentences.
Biome: {biome}, Time: {time_of_day}, Weather: {weather}, Nearby: {", ".join(nearby_objects)}"""
    response = client.chat.completions.create(
        model="phi-3.5-mini-instruct",
        messages=[{"role": "user", "content": prompt}],
        max_tokens=80,
    )
    return response.choices[0].message.content
```

### 5.5 AI Game Master (Dynamic Difficulty)

```python
# Copilot prompt: "AI Game Master that adjusts difficulty based on player performance metrics"
def adjust_difficulty(metrics: dict) -> dict:
    prompt = f"""Player metrics: {json.dumps(metrics)}
Based on these metrics, suggest difficulty adjustments as JSON:
{{"enemy_health_multiplier": N, "spawn_rate": N, "hint_frequency": "low|medium|high"}}"""
    response = client.chat.completions.create(
        model="gpt-4o-mini",
        messages=[{"role": "user", "content": prompt}],
        response_format={"type": "json_object"},
    )
    return json.loads(response.choices[0].message.content)
```

### 5.6 Supported Models on GitHub Models

| Model | Best For |
|---|---|
| `gpt-4o` | Complex NPC dialogue, story generation |
| `gpt-4o-mini` | Real-time quest/event generation (low latency) |
| `o1-mini` | Game balance calculations, strategy AI |
| `Llama-3.3-70B-Instruct` | Open-source alternative for NPC AI |
| `Mistral-large` | Multilingual game content |
| `Phi-3.5-mini-instruct` | Lightweight on-device/edge AI |
| `Cohere-command-r-plus` | Long-context lore and world-building |

---

## 6. GitHub Actions + AI — CI/CD & Automated Testing

Automate your entire build, test, and release pipeline with GitHub Actions, enhanced by AI.

### 6.1 Automated Godot Build & Export

```yaml
# .github/workflows/build.yml
name: Build & Export Game

on:
  push:
    branches: [main, develop]
  pull_request:

jobs:
  export:
    runs-on: ubuntu-latest
    container:
      image: barichello/godot-ci:4.3

    strategy:
      matrix:
        platform: [linux, windows, web]

    steps:
      - uses: actions/checkout@v4
        with:
          lfs: true

      - name: Setup export templates
        run: |
          mkdir -p ~/.local/share/godot/export_templates/4.3.stable
          cp /usr/local/lib/godot/export_templates/* \
             ~/.local/share/godot/export_templates/4.3.stable/

      - name: Export for ${{ matrix.platform }}
        run: |
          mkdir -p builds/${{ matrix.platform }}
          godot --headless --export-release "${{ matrix.platform }}" \
                builds/${{ matrix.platform }}/PROJECT-VILNATURE

      - name: Upload artifact
        uses: actions/upload-artifact@v4
        with:
          name: game-${{ matrix.platform }}
          path: builds/${{ matrix.platform }}/
```

### 6.2 GDScript Linting

```yaml
# .github/workflows/lint.yml
name: Lint GDScript

on: [push, pull_request]

jobs:
  gdscript-lint:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Run gdtoolkit
        run: |
          pip install gdtoolkit==4.*
          find . -name "*.gd" -print0 | xargs -0 gdlint
          find . -name "*.gd" -print0 | xargs -0 gdformat --check
```

### 6.3 Automated Scene & Unit Tests

```yaml
# .github/workflows/test.yml
name: Run Game Tests

on: [push, pull_request]

jobs:
  tests:
    runs-on: ubuntu-latest
    container:
      image: barichello/godot-ci:4.3
    steps:
      - uses: actions/checkout@v4
      - name: Run GUT tests (Godot Unit Testing)
        run: |
          godot --headless -s addons/gut/gut_cmdln.gd \
            -gdir=res://tests -gprefix=test_ -gsuffix=.gd -gexit
```

### 6.4 Copilot-Powered PR Review in CI

```yaml
# .github/workflows/ai-review.yml
name: AI Code Review

on:
  pull_request:
    paths: ['**.gd', '**.glsl', '**.gdshader']

jobs:
  copilot-review:
    runs-on: ubuntu-latest
    permissions:
      pull-requests: write
    steps:
      - uses: actions/checkout@v4
      - name: GitHub Copilot review
        uses: github/copilot-code-review-action@v1
        with:
          github-token: ${{ secrets.GITHUB_TOKEN }}
```

### 6.5 Automated Release & Itch.io Deployment

```yaml
# .github/workflows/release.yml
name: Release to itch.io

on:
  push:
    tags: ['v*']

jobs:
  deploy:
    runs-on: ubuntu-latest
    needs: [export]
    steps:
      - uses: actions/download-artifact@v4
      - name: Deploy to itch.io
        uses: josephbmanley/butler-publish-itchio-action@v1.0.3
        env:
          BUTLER_CREDENTIALS: ${{ secrets.BUTLER_CREDENTIALS }}
          CHANNEL: linux
          ITCH_GAME: project-vilnature
          ITCH_USER: ${{ secrets.ITCH_USER }}
          PACKAGE: game-linux/
```

---

## 7. GitHub Codespaces — Cloud Development Environment

Develop the entire game in the browser or VS Code without any local setup.

### 7.1 Devcontainer Configuration

```jsonc
// .devcontainer/devcontainer.json
{
  "name": "PROJECT-VILNATURE Dev",
  "image": "mcr.microsoft.com/devcontainers/base:ubuntu-24.04",
  "features": {
    "ghcr.io/devcontainers/features/python:1": { "version": "3.12" },
    "ghcr.io/devcontainers/features/node:1": { "version": "20" },
    "ghcr.io/devcontainers/features/github-cli:1": {}
  },
  "postCreateCommand": "bash .devcontainer/setup.sh",
  "customizations": {
    "vscode": {
      "extensions": [
        "geequlim.godot-tools",
        "GitHub.copilot",
        "GitHub.copilot-chat",
        "ms-python.python",
        "slevesque.shader"
      ],
      "settings": {
        "godot_tools.editor_path": "/usr/local/bin/godot",
        "github.copilot.enable": { "*": true }
      }
    }
  },
  "forwardPorts": [6007, 8080],
  "portAttributes": {
    "6007": { "label": "Godot LSP" },
    "8080": { "label": "Game Web Export Preview" }
  }
}
```

```bash
#!/usr/bin/env bash
# .devcontainer/setup.sh — installs Godot 4 + tools in the Codespace
GODOT_VERSION="4.3"
GODOT_ZIP="/tmp/godot.zip"
wget -q "https://github.com/godotengine/godot/releases/download/${GODOT_VERSION}-stable/Godot_v${GODOT_VERSION}-stable_linux.x86_64.zip" -O "$GODOT_ZIP"
if [ ! -s "$GODOT_ZIP" ]; then echo "❌ Download failed"; exit 1; fi
unzip -q "$GODOT_ZIP" -d /tmp
mv /tmp/Godot_v${GODOT_VERSION}-stable_linux.x86_64 /usr/local/bin/godot
chmod +x /usr/local/bin/godot
pip install gdtoolkit==4.*
echo "✅ Godot ${GODOT_VERSION} + gdtoolkit installed"
```

### 7.2 Codespaces Benefits for Game Dev

- **No GPU required locally** — use a browser to code; run game on the server
- **Pre-installed Copilot** — AI coding assistance available instantly
- **Shared environments** — collaborate with teammates in the same Codespace
- **Persistent game data** — assets and project files persist across sessions
- **Secrets management** — `GITHUB_TOKEN` automatically available for GitHub Models API

---

## 8. GitHub Copilot for Pull Requests & Code Review

### 8.1 Auto-Generated PR Summaries

When you open a PR, Copilot can automatically generate:
- A concise summary of what changed
- A list of affected game systems
- Potential risks or side effects
- Suggested reviewers

Enable in your repo settings: **Settings → Copilot → Pull Request Summaries → Enable**.

### 8.2 Copilot Review Comments

Copilot adds inline suggestions during code review:
- Spots null-reference bugs in GDScript before merge
- Flags missing `await` on async calls
- Suggests performance improvements in hot paths
- Detects inconsistent signal naming conventions

### 8.3 Commit Message Generation

In VS Code with Copilot, click the ✨ sparkle icon in the Source Control panel to auto-generate a descriptive commit message based on your staged changes.

---

## Recommended 3D Engine Workflows

### Godot 4 (Recommended — Open Source)

```
Copilot strengths with Godot:
✓ GDScript is well-represented in Copilot's training data
✓ @workspace agent understands scene tree structure
✓ gdtoolkit linting integrates natively in CI
✓ GitHub Models API easily called from GDScript via HTTPRequest
```

**Integrate GitHub Models into Godot:**

```gdscript
# Copilot prompt: "Call GitHub Models API from GDScript using HTTPRequest node"
extends Node

const API_URL := "https://models.inference.ai.azure.com/chat/completions"
var _http := HTTPRequest.new()

func _ready() -> void:
    add_child(_http)
    _http.request_completed.connect(_on_response)

func ask_npc(message: String) -> void:
    var body := JSON.stringify({
        "model": "gpt-4o-mini",
        "messages": [{"role": "user", "content": message}],
        "max_tokens": 100
    })
    var headers := [
        "Content-Type: application/json",
        "Authorization: Bearer " + OS.get_environment("GITHUB_TOKEN")
    ]
    _http.request(API_URL, headers, HTTPClient.METHOD_POST, body)

func _on_response(_result, _code, _headers, body: PackedByteArray) -> void:
    var json    := JSON.parse_string(body.get_string_from_utf8())
    var reply   := json["choices"][0]["message"]["content"]
    DialogueManager.show(reply)
```

### Unity (C#)

```csharp
// Copilot prompt: "Singleton GameManager with scene loading and event system in Unity 6"
public class GameManager : MonoBehaviour
{
    public static GameManager Instance { get; private set; }
    public event Action<float> OnHealthChanged;

    private void Awake()
    {
        if (Instance != null) { Destroy(gameObject); return; }
        Instance = this;
        DontDestroyOnLoad(gameObject);
    }

    public void LoadScene(string sceneName) =>
        StartCoroutine(LoadSceneAsync(sceneName));

    private IEnumerator LoadSceneAsync(string sceneName)
    {
        var op = SceneManager.LoadSceneAsync(sceneName);
        op.allowSceneActivation = false;
        while (op.progress < 0.9f) yield return null;
        op.allowSceneActivation = true;
    }
}
```

### Unreal Engine 5 (C++ / Blueprints)

```
Copilot strengths with UE5:
✓ C++ UObject/AActor boilerplate generated in seconds
✓ HLSL custom nodes and material expressions written inline
✓ Build.cs module files, .uproject targets — no hand-editing
✓ UnrealAutomationTool CLI commands explained by Copilot Chat
✓ See Section 19 for the full UE5 AAA-graphics workflow
```

```cpp
// Copilot prompt: "UE5 C++ ACharacter subclass with Enhanced Input, Lumen-compatible camera,
//                  and modular ability component using Gameplay Ability System (GAS)"
UCLASS()
class VILNATURE_API AVilNatureCharacter : public ACharacter
{
    GENERATED_BODY()

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess))
    USpringArmComponent* CameraBoom;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess))
    UCameraComponent* FollowCamera;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities", meta = (AllowPrivateAccess))
    UAbilitySystemComponent* AbilitySystem;

public:
    AVilNatureCharacter();
    virtual void SetupPlayerInputComponent(UInputComponent* Input) override;

protected:
    UPROPERTY(EditAnywhere, Category = "Input") UInputMappingContext* DefaultMappingContext;
    UPROPERTY(EditAnywhere, Category = "Input") UInputAction* MoveAction;
    UPROPERTY(EditAnywhere, Category = "Input") UInputAction* LookAction;
    UPROPERTY(EditAnywhere, Category = "Input") UInputAction* JumpAction;

private:
    void Move(const FInputActionValue& Value);
    void Look(const FInputActionValue& Value);
};
```

### Three.js / Babylon.js (Browser 3D)

```typescript
// Copilot prompt: "Three.js scene with PBR terrain, dynamic sky, and post-processing"
import * as THREE from 'three';
import { Sky } from 'three/examples/jsm/objects/Sky.js';
import { EffectComposer } from 'three/examples/jsm/postprocessing/EffectComposer.js';

const renderer = new THREE.WebGLRenderer({ antialias: true });
const scene    = new THREE.Scene();
const camera   = new THREE.PerspectiveCamera(60, innerWidth / innerHeight, 0.1, 2000);

const sky = new Sky();
sky.scale.setScalar(10000);
scene.add(sky);

const terrain = new THREE.Mesh(
    new THREE.PlaneGeometry(500, 500, 128, 128),
    new THREE.MeshStandardMaterial({ color: 0x3a7d44, roughness: 0.9 })
);
terrain.rotation.x = -Math.PI / 2;
scene.add(terrain);
```

---

## End-to-End Game Development Workflow

This is the recommended step-by-step workflow for building PROJECT-VILNATURE entirely with GitHub AI:

```
Phase 1: Concept & Planning
  └── Copilot Chat: "Design a 3D nature exploration game with survival mechanics"
  └── Copilot Chat: "Create a detailed GDD (Game Design Document) outline"
  └── GitHub Models: Generate lore, world-building details, character backstories

Phase 2: Project Setup
  └── Copilot CLI: "scaffold a Godot 4 project with recommended directory structure"
  └── Codespaces: Open devcontainer — Godot + Copilot pre-installed
  └── Copilot: Generate .gitignore, .editorconfig, project.godot settings

Phase 3: Core Systems (Copilot code generation)
  └── Player controller (movement, camera, stamina)
  └── Inventory & crafting system
  └── Save/load system
  └── Audio manager
  └── Event bus / signal architecture

Phase 4: World Building
  └── Copilot: Procedural terrain generation (noise-based chunks)
  └── Copilot: Biome system (forest, river, mountain, cave)
  └── Copilot: Day/night cycle with dynamic lighting
  └── GitHub Models: Generate dynamic weather narration text

Phase 5: AI & NPC Systems
  └── Copilot: FSM-based enemy/animal AI
  └── GitHub Models: LLM-powered NPC dialogue (GPT-4o-mini)
  └── GitHub Models: Dynamic quest generation
  └── Copilot Chat: Debug pathfinding issues with NavigationServer3D

Phase 6: Visual Polish
  └── Copilot: Toon/stylized water shader
  └── Copilot: Foliage wind shader
  └── Copilot: Post-processing stack (bloom, fog, color grading)
  └── Copilot: LOD (Level of Detail) system for performance

Phase 7: UI & HUD
  └── Copilot: Health/stamina bars, minimap, dialogue box
  └── Copilot: Main menu, pause menu, settings screen
  └── Copilot Chat: Accessibility options (colorblind mode, subtitles)

Phase 8: Audio
  └── Copilot: Adaptive music system (calm/tension/combat layers)
  └── Copilot: Spatial 3D audio for ambient sounds
  └── GitHub Models: Generate dynamic ambient soundscape descriptions

Phase 9: Testing & QA
  └── Copilot /tests: Generate unit tests for all core systems
  └── GitHub Actions: CI runs GUT tests on every push
  └── GitHub Actions: Lint GDScript with gdtoolkit
  └── Copilot PR review: AI catches bugs before merge

Phase 10: Build & Release
  └── GitHub Actions: Matrix build (Linux, Windows, Web, macOS, Android)
  └── GitHub Actions: Auto-tag releases on merge to main
  └── GitHub Actions: Deploy to itch.io, GitHub Pages (web build)
  └── Copilot: Write release notes from commit history
```

---

## Prompt Engineering Guide for Game Development

Getting the most out of GitHub Copilot requires good prompts. Here are patterns that work best for 3D game development:

### Be Specific About the Engine & Language

```
❌  "Write a player controller"
✅  "Write a Godot 4 GDScript CharacterBody3D player controller 
     with WASD movement, mouse-look camera, jumping, and sprinting"
```

### Include Constraints

```
✅  "Write a shader that works in Godot 4's spatial shader pipeline,
     uses only 2 texture samples, and runs at 60fps on mobile"
```

### Reference Existing Code

```
✅  "Following the same pattern as my EnemyFSM class above, 
     create a BossFSM with 4 phases and special attacks"
```

### Ask for Explanations

```
✅  "Write the LOD system AND explain each design decision,
     especially why you chose this distance threshold"
```

### Iterative Refinement

```
Step 1: "Generate a basic inventory system"
Step 2: "Add item stacking to the inventory above"
Step 3: "Now add drag-and-drop UI using Control nodes"
Step 4: "Add serialization so the inventory saves to JSON"
```

---

## Asset & Content Pipeline with GitHub AI

### Texture & Material Generation

```bash
# Use GitHub Models + DALL-E 3 for concept textures
curl -X POST https://models.inference.ai.azure.com/images/generations \
  -H "Authorization: Bearer $GITHUB_TOKEN" \
  -H "Content-Type: application/json" \
  -d '{
    "model": "dall-e-3",
    "prompt": "seamless PBR rock texture, top-down view, 1024x1024, photorealistic",
    "size": "1024x1024"
  }'
```

### Blender Python Scripting with Copilot

```python
# Copilot prompt: "Blender Python script to batch-export all mesh objects as GLB for Godot 4"
import bpy, os

EXPORT_DIR = "/exports/models/"
os.makedirs(EXPORT_DIR, exist_ok=True)

for obj in bpy.data.objects:
    if obj.type != 'MESH':
        continue
    bpy.ops.object.select_all(action='DESELECT')
    obj.select_set(True)
    bpy.context.view_layer.objects.active = obj
    path = os.path.join(EXPORT_DIR, f"{obj.name}.glb")
    bpy.ops.export_scene.gltf(
        filepath=path,
        use_selection=True,
        export_format='GLB',
        export_draco_mesh_compression_enable=True,
    )
    print(f"Exported: {path}")
```

### Procedural Audio with AI

```python
# Copilot prompt: "Generate adaptive music parameters using GitHub Models based on game state"
def get_music_parameters(tension: float, biome: str) -> dict:
    prompt = f"""Game state: tension={tension:.2f}, biome={biome}
Return music parameters as JSON:
{{"bpm": N, "key": "C_minor", "instruments": ["..."], "intensity": "low|medium|high"}}"""
    response = client.chat.completions.create(
        model="gpt-4o-mini",
        messages=[{"role": "user", "content": prompt}],
        response_format={"type": "json_object"},
    )
    return json.loads(response.choices[0].message.content)
```

---

## Multiplayer & Backend with GitHub AI

```gdscript
# Copilot prompt: "Godot 4 ENet multiplayer lobby with host/join and player sync"
extends Node

const PORT     := 7777
const MAX_PEERS := 8

func host_game() -> void:
    var peer := ENetMultiplayerPeer.new()
    peer.create_server(PORT, MAX_PEERS)
    multiplayer.multiplayer_peer = peer
    multiplayer.peer_connected.connect(_on_peer_connected)

func join_game(ip: String) -> void:
    var peer := ENetMultiplayerPeer.new()
    peer.create_client(ip, PORT)
    multiplayer.multiplayer_peer = peer

@rpc("any_peer", "call_local", "reliable")
func sync_player_position(id: int, pos: Vector3) -> void:
    if has_node(str(id)):
        get_node(str(id)).global_position = pos
```

```python
# Copilot prompt: "FastAPI leaderboard backend with GitHub Models for generating player titles"
from fastapi import FastAPI
from openai import OpenAI

app    = FastAPI()
client = OpenAI(
    base_url="https://models.inference.ai.azure.com",
    api_key=os.environ["GITHUB_TOKEN"],
)

@app.get("/leaderboard/{player_name}/title")
async def generate_title(player_name: str, score: int, playtime_hours: float) -> dict:
    prompt = f"""Player: {player_name}, Score: {score}, Hours played: {playtime_hours:.1f}
Generate a creative in-world title for this player (max 4 words)."""
    resp = client.chat.completions.create(
        model="gpt-4o-mini",
        messages=[{"role": "user", "content": prompt}],
        max_tokens=20,
    )
    return {"title": resp.choices[0].message.content.strip()}
```

---

## Performance, Profiling & Optimization with AI

### Copilot Chat Prompts for Optimization

```
@workspace Identify all O(n²) loops in scripts under res://systems/ 
and suggest more efficient alternatives.
```

```
My game uses 2.1 GB of RAM at runtime. 
Analyze my resource loading pattern and suggest where to use 
load() vs preload() vs ResourceLoader.load_threaded_request().
```

```
This Godot 4 shader is running at 45fps on a GTX 1060.
[paste shader]
Profile it and rewrite it to hit 60fps by reducing texture samples
and using approximate math functions where precision isn't critical.
```

### Automated Performance Regression Testing

```yaml
# .github/workflows/perf.yml
name: Performance Benchmarks

on:
  push:
    branches: [main]

jobs:
  benchmark:
    runs-on: ubuntu-latest
    container:
      image: barichello/godot-ci:4.3
    steps:
      - uses: actions/checkout@v4
      - name: Run performance benchmark scene
        run: |
          godot --headless -s tests/bench/bench_main.gd \
            --benchmark-output=perf_results.json
      - name: Comment results on commit
        uses: actions/github-script@v7
        with:
          script: |
            const fs   = require('fs');
            const data = JSON.parse(fs.readFileSync('perf_results.json'));
            github.rest.repos.createCommitComment({
              owner: context.repo.owner, repo: context.repo.repo,
              commit_sha: context.sha,
              body: `## Performance Results\n\`\`\`json\n${JSON.stringify(data, null, 2)}\n\`\`\``
            });
```

---

## Publishing & Deployment

### GitHub Pages (Web Export)

```yaml
# .github/workflows/pages.yml
name: Deploy Web Build to GitHub Pages

on:
  push:
    branches: [main]

permissions:
  contents: read
  pages: write
  id-token: write

jobs:
  deploy:
    environment:
      name: github-pages
      url: ${{ steps.deployment.outputs.page_url }}
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Export Web build
        uses: ./.github/actions/godot-export
        with:
          platform: web
          output: builds/web
      - name: Upload Pages artifact
        uses: actions/upload-pages-artifact@v3
        with:
          path: builds/web/
      - name: Deploy to GitHub Pages
        id: deployment
        uses: actions/deploy-pages@v4
```

### Versioning with AI-Generated Changelogs

```yaml
# .github/workflows/release.yml (changelog generation)
- name: Generate changelog with Copilot
  run: |
    gh copilot suggest \
      "Generate a user-friendly changelog from these git commits: $(git log --oneline v$PREV_TAG..HEAD)" \
      > CHANGELOG_ENTRY.md
```

---

## Resources & References

| Resource | Link |
|---|---|
| GitHub Copilot Docs | https://docs.github.com/en/copilot |
| GitHub Models Docs | https://docs.github.com/en/github-models |
| GitHub Models Marketplace | https://github.com/marketplace/models |
| GitHub Actions Docs | https://docs.github.com/en/actions |
| GitHub Codespaces Docs | https://docs.github.com/en/codespaces |
| Godot 4 Documentation | https://docs.godotengine.org/en/stable/ |
| GUT (Godot Unit Testing) | https://github.com/bitwes/Gut |
| gdtoolkit Linter | https://github.com/Scony/godot-gdscript-toolkit |
| Godot CI Docker Image | https://github.com/abarichello/godot-ci |
| OpenAI Python SDK | https://github.com/openai/openai-python |
| Unreal Engine 5 Docs | https://dev.epicgames.com/documentation/en-us/unreal-engine |
| UE5 Lumen Documentation | https://dev.epicgames.com/documentation/en-us/unreal-engine/lumen-global-illumination-and-reflections |
| UE5 Nanite Documentation | https://dev.epicgames.com/documentation/en-us/unreal-engine/nanite-virtualized-geometry |
| UE5 PCG Documentation | https://dev.epicgames.com/documentation/en-us/unreal-engine/procedural-content-generation |
| MetaHuman Creator | https://metahuman.unrealengine.com/ |
| UE5 Python Scripting | https://dev.epicgames.com/documentation/en-us/unreal-engine/scripting-the-unreal-editor-using-python |
| UnrealAutomationTool | https://dev.epicgames.com/documentation/en-us/unreal-engine/unreal-automation-tool |
| Git LFS | https://git-lfs.com/ |

---

## Quick Start

```bash
# 1. Fork & clone this repo
git clone https://github.com/dox121-pixel/PROJECT-VILNATURE
cd PROJECT-VILNATURE

# 2. Open in GitHub Codespaces (recommended)
#    Click "Code" → "Codespaces" → "Create codespace on main"
#    Godot 4 + Copilot are pre-installed automatically

# 3. Set your GitHub Token for GitHub Models API
export GITHUB_TOKEN="your_token_here"   # already set in Codespaces

# 4. Open the Godot project
godot project.godot

# 5. Start coding with Copilot!
#    - Press Ctrl+I in VS Code to open Copilot Chat
#    - Use @workspace for full project context
#    - Type a comment and let Copilot complete the implementation
```

---

*This README was authored with GitHub Copilot Chat and serves as a living reference for the PROJECT-VILNATURE game development journey.*

---

## Unreal Engine 5 — High-Quality Graphics

> Use this section when the primary project goal is **AAA-quality photorealistic visuals**. All GitHub AI tools remain the same; the focus shifts to C++, HLSL, and UE5's rendering pipeline.

---

### 19.1 Why UE5 for AAA Graphics

| UE5 Feature | What it gives you |
|---|---|
| **Lumen** | Fully dynamic global illumination — no baking, no lightmaps |
| **Nanite** | Virtualized geometry — billions of triangles, no manual LODs |
| **Temporal Super Resolution (TSR)** | Native upscaling to 4K at a fraction of the GPU cost |
| **Substrate** | New layered material system for physically accurate surfaces |
| **Path Tracing** | Reference-quality offline renders and in-game ray tracing |
| **MetaHuman** | Photorealistic human characters out of the box |
| **World Partition** | Infinitely large open worlds without manual streaming zones |
| **Procedural Content Generation (PCG)** | Rule-based foliage, rocks, buildings — AI-assisted layout |

---

### 19.2 UE5 Graphics Feature Setup

Enable UE5's core rendering features via `DefaultEngine.ini` — Copilot can generate the full ini block from a comment:

```ini
; Copilot prompt: "DefaultEngine.ini settings to enable Lumen GI, Hardware Ray Tracing,
;                  Nanite, Temporal Super Resolution, and film grain post-process in UE5"
[/Script/Engine.RendererSettings]
r.Lumen.Reflections.Allow=1
r.Lumen.DiffuseIndirect.Allow=1
r.RayTracing=1
r.RayTracing.Shadows=1
r.Nanite.ProjectEnabled=1
r.TemporalAA.Method=4          ; 4 = TSR
r.FilmGrain=1
r.FilmGrain.Intensity=0.4
r.PostProcessAAQuality=6
r.Lumen.TraceMeshSDFs=1
r.Lumen.HardwareRayTracing=1

[/Script/Engine.Engine]
bEnableRayTracing=True
```

**Copilot Chat usage for rendering settings:**

```
Explain the difference between Lumen Software Tracing and Hardware Ray Tracing.
When should I use each for a dense forest open-world game?
```

```
My Lumen scene shows light leaking through thin walls.
What DefaultEngine.ini or r.Lumen.* cvars should I tweak to fix this?
```

```
I'm targeting 60fps on RTX 3070 at 1440p. Recommend TSR quality settings
and Lumen tracing budgets that balance quality vs. performance.
```

---

### 19.3 UE5 Material System with Copilot

#### Substrate Master Material (HLSL Custom Node)

```hlsl
// Copilot prompt: "HLSL custom node for Substrate layered material:
//                  moss blend based on world-space normal and height"
// Node type: Custom (HLSL), Inputs: BaseColor, MossColor, WorldNormal, WorldHeight, MossAmount

float upFacing   = saturate(dot(WorldNormal, float3(0, 0, 1)));
float heightMask = saturate((WorldHeight - 50.0f) / 200.0f);
float mossMask   = upFacing * heightMask * MossAmount;
return lerp(BaseColor, MossColor, mossMask);
```

#### Landscape Auto-Material (8 Biome Layers)

```
// Copilot Chat prompt:
"Generate a UE5 Material Function for a landscape auto-material with 8 biome layers:
 snow (high altitude), rock, alpine grass, forest floor, wet mud, sand, shallow water, deep water.
 Use world-space height and slope angle for automatic layer blending.
 Output: BaseColor, Normal, Roughness, Metallic for Substrate."
```

#### Post-Process Color Grading

```cpp
// Copilot prompt: "UE5 C++ function to apply stylized color grading post-process
//                  material at runtime using a FPostProcessSettings struct"
void AVilNaturePlayerController::ApplyColorGrading(float Saturation, float Contrast, UMaterialInterface* LUT)
{
    FPostProcessSettings PP;
    PP.bOverride_ColorSaturation     = true;
    PP.ColorSaturation               = FVector4(Saturation, Saturation, Saturation, 1.0f);
    PP.bOverride_ColorContrast       = true;
    PP.ColorContrast                 = FVector4(Contrast, Contrast, Contrast, 1.0f);
    PP.bOverride_ColorGradingLUT     = true;
    PP.ColorGradingLUT               = Cast<UTexture>(LUT);
    GetPlayerCameraManager()->SetManualCameraFade(0.0f, FLinearColor::Black, false);
    // Apply via post-process volume or camera override
}
```

---

### 19.4 Procedural Content Generation (PCG) with GitHub Models

#### PCG Graph for Procedural Forest

```cpp
// Copilot prompt: "UE5 PCG graph C++ component: scatter foliage with
//                  density driven by slope, altitude, and biome mask texture"
UCLASS()
class VILNATURE_API UVilNaturePCGForest : public UPCGBlueprintElement
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, Category = "Forest") float MaxSlope      = 35.0f;
    UPROPERTY(EditAnywhere, Category = "Forest") float MinAltitude   = 50.0f;
    UPROPERTY(EditAnywhere, Category = "Forest") float MaxAltitude   = 800.0f;
    UPROPERTY(EditAnywhere, Category = "Forest") float TreeDensity   = 0.6f;

    virtual void ExecuteWithContext_Implementation(
        UPARAM(ref) FPCGContext&        Context,
        const FPCGDataCollection&       InputData,
        FPCGDataCollection&             OutputData) override;
};
```

#### LLM-Generated Biome Placement Rules

```python
# Copilot prompt: "Use GitHub Models GPT-4o to generate PCG biome placement rules
#                  from a natural language world description"
def generate_biome_rules(world_description: str) -> dict:
    prompt = f"""World description: {world_description}

Generate PCG biome placement rules as JSON:
{{
  "biomes": [
    {{
      "name": "...",
      "altitude_min": N,
      "altitude_max": N,
      "slope_max": N,
      "moisture": "dry|moderate|wet",
      "foliage": ["tree_type_1", "..."],
      "ground_cover": "..."
    }}
  ]
}}"""
    response = client.chat.completions.create(
        model="gpt-4o",
        messages=[{"role": "user", "content": prompt}],
        response_format={"type": "json_object"},
    )
    return json.loads(response.choices[0].message.content)

# Example usage
rules = generate_biome_rules(
    "A temperate mountain range with dense pine forests below the snowline, "
    "alpine meadows mid-elevation, and bare granite peaks above 1200m."
)
```

---

### 19.5 UE5 Python Editor Scripting

Use Copilot to write Editor Python scripts that automate tedious UE5 asset operations.

#### Enable Nanite on All Static Meshes

```python
# Copilot prompt: "Unreal Editor Python script to enable Nanite on all StaticMesh
#                  assets in the /Game/Environment folder and save them"
import unreal

asset_reg  = unreal.AssetRegistryHelpers.get_asset_registry()
mesh_class = unreal.StaticMesh.static_class()
assets     = asset_reg.get_assets_by_path("/Game/Environment", recursive=True)

with unreal.ScopedEditorTransaction("Enable Nanite on Environment Meshes") as t:
    for asset_data in assets:
        if asset_data.asset_class_path.asset_name != "StaticMesh":
            continue
        mesh = asset_data.get_asset()
        if not mesh:
            continue
        nanite_settings = mesh.get_editor_property("nanite_settings")
        nanite_settings.set_editor_property("enabled", True)
        mesh.set_editor_property("nanite_settings", nanite_settings)
        unreal.EditorAssetLibrary.save_asset(mesh.get_path_name())

print(f"Nanite enabled on {len(assets)} assets")
```

#### Batch LOD Automation

```python
# Copilot prompt: "UE5 Python script to set automatic LOD reduction on all meshes
#                  in /Game/Characters with 4 LOD levels and 50% reduction per level"
import unreal

lod_settings = unreal.EditorStaticMeshLibrary

for asset_data in asset_reg.get_assets_by_path("/Game/Characters", recursive=True):
    if asset_data.asset_class_path.asset_name != "StaticMesh":
        continue
    mesh = asset_data.get_asset()
    reduction = unreal.StaticMeshReductionSettings()
    reduction.set_editor_property("percent_triangles", 0.5)
    for lod_idx in range(1, 4):
        lod_settings.set_lods_with_notification(mesh, [reduction] * lod_idx, True)
    unreal.EditorAssetLibrary.save_asset(mesh.get_path_name())
```

---

### 19.6 Self-Hosted GitHub Actions Runner for UE5

UE5 CI/CD requires a dedicated machine with GPU — standard GitHub-hosted runners cannot install the engine.

#### Runner Setup Guide

```bash
# On your build machine (Windows or Linux with NVIDIA GPU):

# 1. Download and configure the Actions runner
mkdir actions-runner && cd actions-runner
curl -o actions-runner-linux-x64.tar.gz -L \
  https://github.com/actions/runner/releases/download/v2.317.0/actions-runner-linux-x64-2.317.0.tar.gz
tar xzf ./actions-runner-linux-x64.tar.gz
./config.sh --url https://github.com/dox121-pixel/PROJECT-VILNATURE \
            --token YOUR_RUNNER_TOKEN --labels ue5-gpu --name ue5-build-01

# 2. Install as a service
sudo ./svc.sh install
sudo ./svc.sh start
```

#### UE5 Build Workflow

```yaml
# .github/workflows/ue5-build.yml
name: UE5 Build & Package

on:
  push:
    branches: [main, develop]
  pull_request:

jobs:
  build:
    runs-on: [self-hosted, ue5-gpu]   # targets your dedicated build machine
    env:
      UE_ROOT: "C:/Program Files/Epic Games/UE_5.4"   # adjust to your install

    steps:
      - uses: actions/checkout@v4
        with:
          lfs: true

      - name: Warm Derived Data Cache (DDC)
        run: |
          & "$env:UE_ROOT/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" `
            "${{ github.workspace }}/VilNature.uproject" `
            -run=DerivedDataCache -fill -DDC=DerivedDataBackendGraph

      - name: Compile C++ (Development Editor)
        run: |
          & "$env:UE_ROOT/Engine/Build/BatchFiles/Build.bat" `
            VilNatureEditor Win64 Development `
            "${{ github.workspace }}/VilNature.uproject" -waitmutex

      - name: Run Automation Tests
        run: |
          & "$env:UE_ROOT/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" `
            "${{ github.workspace }}/VilNature.uproject" `
            -ExecCmds="Automation RunTests VilNature;Quit" `
            -log -unattended -nullrhi

      - name: Package for Windows
        run: |
          & "$env:UE_ROOT/Engine/Build/BatchFiles/RunUAT.bat" BuildCookRun `
            -project="${{ github.workspace }}/VilNature.uproject" `
            -noP4 -platform=Win64 -clientconfig=Shipping `
            -cook -build -stage -pak -archive `
            -archivedirectory="${{ github.workspace }}/Packaged"

      - name: Upload package artifact
        uses: actions/upload-artifact@v4
        with:
          name: VilNature-Win64-Shipping
          path: Packaged/
```

---

### 19.7 Git LFS Strategy for Large Binary Assets

UE5 projects contain gigabytes of `.uasset`, `.umap`, textures, and audio files. Configure Git LFS from day one.

```bash
# Initialize Git LFS in your UE5 project
git lfs install

# Track all UE5 binary asset types
git lfs track "*.uasset"
git lfs track "*.umap"
git lfs track "*.fbx"
git lfs track "*.png" "*.tga" "*.exr" "*.hdr"
git lfs track "*.wav" "*.ogg" "*.mp3"
git lfs track "*.mp4" "*.mov"
git lfs track "*.psd" "*.xcf"

# Commit the .gitattributes file
git add .gitattributes
git commit -m "chore: configure Git LFS tracking for UE5 binary assets"
```

Generated `.gitattributes` (Copilot can produce this from a comment):

```gitattributes
# Copilot prompt: "Git LFS .gitattributes for a UE5 project — all binary asset types"
*.uasset  filter=lfs diff=lfs merge=lfs -text
*.umap    filter=lfs diff=lfs merge=lfs -text
*.fbx     filter=lfs diff=lfs merge=lfs -text
*.png     filter=lfs diff=lfs merge=lfs -text
*.tga     filter=lfs diff=lfs merge=lfs -text
*.exr     filter=lfs diff=lfs merge=lfs -text
*.hdr     filter=lfs diff=lfs merge=lfs -text
*.wav     filter=lfs diff=lfs merge=lfs -text
*.ogg     filter=lfs diff=lfs merge=lfs -text
*.mp4     filter=lfs diff=lfs merge=lfs -text
*.psd     filter=lfs diff=lfs merge=lfs -text
```

**Copilot CLI for LFS management:**

```bash
gh copilot suggest "list all Git LFS objects larger than 100 MB in this repo"
gh copilot suggest "prune old Git LFS objects that are no longer referenced by any branch"
gh copilot explain "git lfs migrate import --include='*.png' --everything"
```

---

### 19.8 MetaHuman + Copilot

#### Control Rig — Procedural IK Adjustments

```python
# Copilot prompt: "UE5 Control Rig Python script for foot IK:
#                  trace ground below each foot and adjust foot bone rotation to match slope"
import unreal

@unreal.uclass()
class VilNatureFootIK(unreal.RigUnit_FullbodyIK):

    foot_bones = ["foot_l", "foot_r"]
    trace_dist  = 100.0   # cm below foot

    def solve(self) -> None:
        for bone_name in self.foot_bones:
            bone_loc  = self.hierarchy.get_global_transform(
                unreal.RigElementKey(type=unreal.RigElementType.BONE, name=bone_name)
            ).translation
            start     = bone_loc + unreal.Vector(0, 0, 50)
            end       = bone_loc - unreal.Vector(0, 0, self.trace_dist)
            hit_loc, hit_normal, hit = self._line_trace(start, end)
            if hit:
                self._align_foot_to_normal(bone_name, hit_normal)
```

#### NPC Facial Animation from GitHub Models

```python
# Copilot prompt: "Generate UE5 MetaHuman facial animation curve values
#                  from an emotion + dialogue line using GitHub Models"
def get_facial_animation(emotion: str, dialogue: str) -> dict:
    prompt = f"""NPC emotion: {emotion}
Dialogue line: "{dialogue}"

Generate MetaHuman facial animation curve intensities (0.0–1.0) as JSON:
{{
  "BrowsIn_L": N, "BrowsIn_R": N,
  "BrowsOuterLower_L": N, "BrowsOuterLower_R": N,
  "EyesSquint_L": N, "EyesSquint_R": N,
  "MouthSmile_L": N, "MouthSmile_R": N,
  "JawOpen": N,
  "CheekPuff_L": N, "CheekPuff_R": N
}}"""
    response = client.chat.completions.create(
        model="gpt-4o",
        messages=[{"role": "user", "content": prompt}],
        response_format={"type": "json_object"},
    )
    return json.loads(response.choices[0].message.content)
```

#### Copilot Chat Prompts for MetaHuman

```
How do I blend a MetaHuman facial pose into a Sequencer animation track
without overwriting the base pose in UE5.4?
```

```
My MetaHuman NPC has mouth penetration artifacts during lip sync.
What Control Rig or post-process constraints can I add to fix this?
```

---

### 19.9 UE5 Cinematic Pipeline

#### Sequencer Python Automation

```python
# Copilot prompt: "Unreal Editor Python script to create a cinematic camera
#                  flythrough in Sequencer along a predefined spline path"
import unreal

def create_cinematic_flythrough(level_sequence_path: str, spline_actor_name: str) -> None:
    seq     = unreal.load_asset(level_sequence_path)
    editor  = unreal.LevelSequenceEditorBlueprintLibrary
    spline  = unreal.EditorLevelLibrary.get_actor_reference(spline_actor_name)
    cine_cam = unreal.EditorLevelLibrary.spawn_actor_from_class(
        unreal.CineCameraActor, unreal.Vector(0, 0, 0)
    )

    # Add the camera to the sequence
    binding = seq.add_possessable(cine_cam)
    track   = binding.add_track(unreal.MovieScene3DTransformTrack)
    section = track.add_section()
    section.set_range(unreal.SequencerScriptingRange(has_start=True, inclusive_start=0,
                                                     has_end=True, exclusive_end=300))

    # Key position along spline at each frame
    points = spline.get_editor_property("spline_component").get_number_of_spline_points()
    for i in range(points):
        t   = i / (points - 1)
        loc = spline.get_editor_property("spline_component").get_location_at_time(
            t * spline.get_editor_property("spline_component").duration,
            unreal.SplineCoordinateSpace.WORLD
        )
        section.add_key(unreal.FrameNumber(i * 10), unreal.MovieSceneDoubleChannel(), loc.x)

    unreal.EditorAssetLibrary.save_asset(level_sequence_path)
    print(f"Cinematic flythrough created: {points} keyframes")
```

#### High-Res Path-Traced Screenshots

```python
# Copilot prompt: "UE5 Python automation script to render a sequence of
#                  path-traced screenshots at 4K using Movie Render Queue"
import unreal

def render_path_traced_screenshots(sequence_path: str, output_dir: str) -> None:
    queue    = unreal.MoviePipelineQueueEngineSubsystem()
    job      = queue.allocate_new_job(unreal.MoviePipelineExecutorJob)
    job.sequence = unreal.SoftObjectPath(sequence_path)
    job.map      = unreal.EditorLevelLibrary.get_editor_world()

    config   = job.get_configuration()
    settings = config.find_or_add_setting_by_class(unreal.MoviePipelineDeferredPassBase)

    # Enable Path Tracing
    pt = config.find_or_add_setting_by_class(unreal.MoviePipelinePathTracerPass)
    pt.set_editor_property("spatial_sample_count", 64)

    # 4K output
    output   = config.find_or_add_setting_by_class(unreal.MoviePipelineOutputSetting)
    output.set_editor_property("output_resolution", unreal.IntPoint(3840, 2160))
    output.set_editor_property("output_directory", unreal.DirectoryPath(output_dir))
    output.set_editor_property("file_name_format", "{sequence_name}_{frame_number}")

    # EXR output for compositing
    exr = config.find_or_add_setting_by_class(unreal.MoviePipelineImageSequenceOutput_EXR)
    exr.set_editor_property("compression", unreal.EXRCompressionFormat.PIZ)

    executor = unreal.MoviePipelinePIEExecutor()
    queue.render_queue_with_executor_instance(executor)
```

#### Automated Trailer Rendering on Tag Push

```yaml
# .github/workflows/ue5-trailer.yml
name: Render Cinematic Trailer

on:
  push:
    tags: ['v*']   # triggers on version tags like v1.0.0

jobs:
  render:
    runs-on: [self-hosted, ue5-gpu]
    steps:
      - uses: actions/checkout@v4
        with:
          lfs: true

      - name: Render trailer frames (path tracing)
        run: |
          & "$env:UE_ROOT/Engine/Binaries/Win64/UnrealEditor-Cmd.exe" `
            "${{ github.workspace }}/VilNature.uproject" `
            -MovieSceneCapture -LevelSequence="/Game/Cinematics/Trailer_Main" `
            -MovieFolder="${{ github.workspace }}/TrailerFrames" `
            -MovieFormat=PNG -MovieQuality=100 `
            -notexturestreaming -unattended

      - name: Upload trailer frames
        uses: actions/upload-artifact@v4
        with:
          name: trailer-frames-${{ github.ref_name }}
          path: TrailerFrames/
          retention-days: 30
```

---

### 19.10 UE5 End-to-End Graphics Workflow

```
Phase 1: Project Setup
  └── Copilot CLI: scaffold UE5 C++ project (Game, Blank template)
  └── Copilot: Generate .gitignore for UE5 (excludes Binaries, DerivedDataCache, Saved)
  └── GitHub LFS: track *.uasset, *.umap, *.fbx, *.png, *.wav (see Section 19.7)
  └── Copilot: Generate VilNature.Build.cs with required modules

Phase 2: Core Rendering Settings
  └── Copilot: Generate DefaultEngine.ini with Lumen + Nanite + TSR settings
  └── Copilot Chat: Explain Lumen software vs hardware tracing trade-offs
  └── Copilot Python: Enable Nanite on all existing static meshes in bulk

Phase 3: Materials & Shaders
  └── Copilot: Substrate master material (rock, bark, wet mud, metal, moss)
  └── Copilot: Landscape auto-material with 8 biome layers
  └── Copilot Chat: Debug Lumen artifacts and light leaking in cave interiors
  └── Copilot: Custom post-process material for color grading

Phase 4: World Building (PCG + GitHub Models)
  └── Copilot: UE5 PCG graph component for procedural forest density
  └── GitHub Models (GPT-4o): Generate biome placement rules from world description
  └── Copilot: World Partition streaming volume placement automation
  └── Copilot: Foliage interaction system (grass bending, snow accumulation)

Phase 5: Lighting
  └── Copilot: Time-of-day system driving sky atmosphere + exponential height fog
  └── Copilot: Lumen sky light recapture on weather transitions
  └── Copilot Chat: "Optimize Lumen screen-space tracing budget for open world at 60fps"

Phase 6: Characters & Animation
  └── MetaHuman Creator: create photorealistic characters
  └── Copilot: Control Rig Python for procedural foot IK on uneven terrain
  └── Copilot: Motion Matching setup for fluid locomotion blending
  └── GitHub Models (GPT-4o): Generate facial animation curve data for NPC dialogue

Phase 7: Performance
  └── Copilot: stat GPU / stat RHI profiling scripts to capture per-pass timings
  └── Copilot Chat: "My Nanite scene has 60ms GPU frame time — where to start?"
  └── GitHub Actions (self-hosted): Nightly builds with shader compilation timing reports

Phase 8: Cinematic & Trailer
  └── Copilot Python: Sequencer camera flythrough along spline path
  └── Copilot Python: Movie Render Queue with path tracing at 4K EXR
  └── GitHub Actions: Automated trailer frame rendering on release tag push
```

---

### 19.11 Godot 4 vs UE5 — Key Trade-offs

| Aspect | Godot 4 | Unreal Engine 5 |
|---|---|---|
| **Graphics ceiling** | Good / stylized | AAA photorealistic |
| **Copilot language** | GDScript (well-supported) | C++ + Blueprints (C++ well-supported) |
| **Codespaces viability** | ✅ Full workflow | ⚠️ Code-only, no visual preview |
| **CI/CD complexity** | Low (Docker image available) | High (self-hosted GPU runner required) |
| **Asset pipeline** | Simple (import from Blender) | Complex (Datasmith, MetaHuman, Quixel) |
| **GitHub Models integration** | Direct HTTP in GDScript | REST call from C++ or Blueprint HTTP node |
| **Git LFS requirement** | Optional | Essential (100+ GB of binary assets) |
| **Learning curve with AI** | Low | Medium–High (engine complexity) |
| **Build time** | Seconds (GDScript, no compile) | Minutes–hours (C++ + shader compile) |
| **Royalties** | None (MIT) | 5% above $1M gross revenue |

**Bottom line:** Switching to UE5 does not change *which* GitHub AI tools you use — Copilot, Copilot Chat, GitHub Models, and GitHub Actions all remain central. What changes is the *focus*: more C++ and HLSL generation, more CI/CD infrastructure work, and GitHub Models playing a bigger role in procedural content layout rather than real-time NPC dialogue (due to UE5's rendering complexity consuming more development time).
