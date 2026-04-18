# PROJECT-VILNATURE — Technical Architecture

## Repository Layout

```
PROJECT-VILNATURE/
├── .devcontainer/          # GitHub Codespaces config
├── .github/workflows/      # CI/CD pipelines
├── Config/                 # UE5 .ini configuration files
├── Content/                # Binary game assets (Git LFS)
│   └── Maps/               # Level layout JSON + generated maps
├── Docs/                   # Technical documentation
├── Source/
│   └── VilNature/
│       ├── VilNature.Build.cs
│       ├── VilNatureGameMode.h/.cpp
│       ├── Character/
│       │   ├── VilNatureCharacter.h/.cpp   ← base superhero pawn
│       │   └── FlightComponent.h/.cpp      ← flight physics
│       ├── Combat/
│       │   └── CombatComponent.h/.cpp      ← melee + eye laser + slam
│       ├── Destruction/
│       │   └── DestructionComponent.h/.cpp ← Chaos fracture integration
│       ├── AI/
│       │   └── NPCDialogueComponent.h/.cpp ← GitHub Models LLM dialogue
│       └── VFX/
│           └── VFXManager.h/.cpp           ← post-process & Niagara mgr
├── Tools/
│   ├── requirements.txt
│   ├── texture_validator.py        ← CI asset quality gate
│   ├── asset_import_pipeline.py    ← AI asset rename & copy pipeline
│   ├── generate_environment.py     ← procedural city layout
│   └── ue5_layout_importer.py      ← UE5 Python editor script
├── VilNature.uproject
├── .gitattributes                  ← Git LFS rules
└── .gitignore
```

## Core Systems

### Flight (FlightComponent)
- `StartFlight()` switches `CharacterMovementComponent` to MOVE_Flying
- Per-frame `AddFlightInput()` → `UpdateVelocity()` with separate accel/decel rates
- `TryFireSonicBoom()` triggers at 85% max speed; fires Niagara burst + broadcasts delegate
- `SetBoostActive()` multiplies max speed by `BoostMultiplier` (default 4×)
- Camera shake & FOV push driven by `GetSpeedFraction()`
- `VFXManager::SetSpeedAberration()` called with speed fraction for chromatic aberration

### Combat (CombatComponent)
- 4-hit light combo chain — cycles through `LightAttackMontages[]`
- `ApplyHitStop()` drops `GlobalTimeDilation` to 0.05 for `HitStopDuration` seconds
- Heavy attack: ragdoll via `SkeletalMeshComponent::AddImpulse()`
- `PerformGroundSlam()` → `SpawnGroundSlamField()` uses `SphereTraceMulti` on ECC_Destructible and pushes `AddImpulseAtLocation`
- `TriggerKillCam()` drops dilation to 0.2× for 2 perceived seconds on finishing blow
- All attacks spawn Niagara impact/blood FX at `Hit.ImpactPoint`

### Destruction (DestructionComponent)
- Requires a `UGeometryCollectionComponent` sibling on the same actor
- `ApplyDestructionForce()` calls `GeoCollection->ApplyExternalImpulse()` to break Chaos joints
- `OnSonicBoomNearby()` delegates from `FlightComponent::OnSonicBoom` — buildings nearby subscribe
- Crater decal spawned via `UGameplayStatics::SpawnDecalAtLocation()`
- Niagara dust + debris layered via `SpawnDestructionVFX()`

### NPC Dialogue (NPCDialogueComponent)
- HTTP POST to `https://models.inference.ai.azure.com/chat/completions`
- Auth: `GITHUB_TOKEN` env var (never hardcoded)
- Villain persona escalates based on combo count (arrogant → frustrated → desperate)
- `OnDialogueLineReady` delegate fires when a line is parsed from `choices[0].message.content`

### VFX Manager (AVFXManager)
- Place ONE instance per level; located via `GetActorOfClass()`
- Drives a Material Parameter Collection (`MPC_GlobalVFX`) with scalars:
  - `ChromaticAberrationAmount` — speed trail
  - `BloodSpatterAlpha` — screen blood (fades over `BloodSpatterFadeTime`)
  - `HeatDistortionStrength` — eye laser overlay
- Niagara spawn wrappers for impact flash, shockwave ring, sonic boom, debris cloud

## CI/CD

| Workflow | Trigger | Action |
|---|---|---|
| `build.yml` | push to main/develop, PR, release | Validate assets → compile UE5 → (on release) cook + package + upload |
| `asset-validation.yml` | push/PR touching Content/ or Tools/ | Run texture_validator.py, asset_import_pipeline.py --validate-only |

## Getting Started (Local)

```bash
# 1. Clone with LFS
git clone --recurse-submodules https://github.com/dox121-pixel/PROJECT-VILNATURE
git lfs pull

# 2. Install Python tools
pip install -r Tools/requirements.txt

# 3. Generate a test city layout
python Tools/generate_environment.py --blocks 3 --seed 1337

# 4. Validate textures (no textures yet = zero errors)
python Tools/texture_validator.py --path Content/

# 5. Open VilNature.uproject in UE5 5.7+
```

## Environment Variables

| Variable | Required | Purpose |
|---|---|---|
| `GITHUB_TOKEN` | Yes (for dialogue) | Auth for GitHub Models API LLM calls |
| `UE5_ROOT` | CI only | Path to Unreal Engine installation on self-hosted runner |
