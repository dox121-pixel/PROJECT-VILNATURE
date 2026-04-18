# PROJECT-VILNATURE — Complete Beginner's Implementation Guide

> **Who is this guide for?**  
> This guide assumes you have *never* used Unreal Engine 5 (UE5) or GitHub AI tools before. Every step is explained in plain English. When a term might be confusing, it is explained in plain brackets **[like this]**. Take it one section at a time — you don't need to do everything in one sitting.

---

## Table of Contents

1. [What Is This Project?](#1-what-is-this-project)
2. [Software You Need to Install First](#2-software-you-need-to-install-first)
3. [Getting the Project onto Your Computer](#3-getting-the-project-onto-your-computer)
4. [Opening the Project in Unreal Engine 5](#4-opening-the-project-in-unreal-engine-5)
5. [Understanding What's Already Built](#5-understanding-whats-already-built)
6. [Implementing Each System in UE5 (Step by Step)](#6-implementing-each-system-in-ue5-step-by-step)
   - [6A. The Superhero Character](#6a-the-superhero-character)
   - [6B. Flight System](#6b-flight-system)
   - [6C. Combat System](#6c-combat-system)
   - [6D. Destruction System](#6d-destruction-system)
   - [6E. AI NPC Dialogue (GitHub AI)](#6e-ai-npc-dialogue-github-ai)
   - [6F. VFX Manager](#6f-vfx-manager)
7. [Using the Python Tools](#7-using-the-python-tools)
   - [7A. Generate a City Layout](#7a-generate-a-city-layout)
   - [7B. Import AI-Generated Assets](#7b-import-ai-generated-assets)
   - [7C. Validate Textures](#7c-validate-textures)
8. [Auto-Importing the City Layout into UE5](#8-auto-importing-the-city-layout-into-ue5)
9. [How GitHub CI/CD Works (Auto-Checks on Every Save)](#9-how-github-cicd-works-auto-checks-on-every-save)
10. [Future: Making Everything Auto-Implement into UE5](#10-future-making-everything-auto-implement-into-ue5)
11. [Troubleshooting Common Problems](#11-troubleshooting-common-problems)
12. [Quick Reference Cheat Sheet](#12-quick-reference-cheat-sheet)

---

## 1. What Is This Project?

PROJECT-VILNATURE is a superhero action game built in Unreal Engine 5. Think of it like an *Invincible*-style game where you punch villains, fly through a destructible city, and the villains talk back to you using a real AI language model (the same technology as ChatGPT).

The project is stored on GitHub, which is a website that saves your code like Google Drive saves documents — except it keeps a full history of every change ever made.

The project has these main parts:

| Part | What it does |
|------|-------------|
| **C++ Source Code** | The game's actual logic (flying, punching, destruction) |
| **Python Tools** | Scripts that help set up the city and import art assets |
| **GitHub CI/CD** | Automated checks that run every time you save/push code |
| **GitHub Models API** | The AI brain that makes villains talk back |

---

## 2. Software You Need to Install First

Install each of these in order. Each one is free.

### Step 2A — Install Git
Git is the tool that lets you download and save code to GitHub.

1. Go to https://git-scm.com/downloads
2. Download the installer for your operating system (Windows/Mac/Linux)
3. Install it with all default settings
4. Open a **Terminal** (Mac/Linux) or **Command Prompt** (Windows) and type:
   ```
   git --version
   ```
   If you see something like `git version 2.43.0`, it worked ✅

### Step 2B — Install Git LFS
**[Git LFS = Git Large File Storage]** — Some game files (like 3D models and textures) are huge. Git LFS stores them separately so your download stays manageable.

1. Go to https://git-lfs.com
2. Download and install for your OS
3. In your Terminal/Command Prompt, type:
   ```
   git lfs install
   ```
   You should see `Git LFS initialized.` ✅

### Step 2C — Install Python 3.11
Python is used by the helper tools in the `Tools/` folder.

1. Go to https://www.python.org/downloads/release/python-3110/
2. Download **Python 3.11** (not a newer version — 3.11 is specifically required)
3. **Important on Windows:** When the installer opens, tick the checkbox that says **"Add Python to PATH"**
4. Install with default settings
5. Verify in Terminal:
   ```
   python --version
   ```
   Should show `Python 3.11.x` ✅

### Step 2D — Install Unreal Engine 5.3
**[UE5 = Unreal Engine 5]** — This is the actual game engine. It's the program that runs the game.

1. Go to https://www.unrealengine.com/en-US/download
2. Create a free Epic Games account if you don't have one
3. Download and install the **Epic Games Launcher**
4. Inside the launcher, go to **Unreal Engine → Library**
5. Click the **"+"** button to add a new engine version
6. Select **5.3** from the dropdown and click **Install**
   - ⚠️ This download is ~30 GB. Leave it overnight if needed.
7. After installation, the version should appear in your Library ✅

---

## 3. Getting the Project onto Your Computer

**[Cloning = downloading the project along with its full history]**

1. Open Terminal/Command Prompt
2. Navigate to where you want the project (e.g., your Documents folder):
   ```
   cd Documents
   ```
3. Clone the repository **with all large files**:
   ```
   git clone --recurse-submodules https://github.com/dox121-pixel/PROJECT-VILNATURE
   git lfs pull
   ```
4. This will create a folder called `PROJECT-VILNATURE`. Go into it:
   ```
   cd PROJECT-VILNATURE
   ```
5. Install the Python helper tools:
   ```
   pip install -r Tools/requirements.txt
   ```
   This installs small libraries like `Pillow` (for reading image files) and `rich` (for pretty terminal output). ✅

---

## 4. Opening the Project in Unreal Engine 5

1. Open the **Epic Games Launcher**
2. Click **Unreal Engine → Library**
3. Next to **5.3**, click **Launch**
4. UE5 will open with a project browser. Click **Browse** (or "Open")
5. Navigate to your `PROJECT-VILNATURE` folder
6. Double-click **`VilNature.uproject`**
7. UE5 will ask to **compile the C++ code** — click **Yes**
   - This takes 5–15 minutes the first time. A progress bar will appear. ✅

> **What just happened?** UE5 compiled all the C++ game code (the flight, combat, destruction systems, etc.) into a form the engine can run. You only need to do this once, and again only when you change C++ files.

---

## 5. Understanding What's Already Built

Here is everything that exists in the project right now:

```
📁 Source/VilNature/
│
├── 🦸 Character/
│   ├── VilNatureCharacter    ← The hero. Has health, input bindings, camera
│   └── FlightComponent       ← All flight logic (speed, boost, sonic boom)
│
├── 👊 Combat/
│   └── CombatComponent       ← 4-hit combos, eye laser, ground slam, kill cam
│
├── 💥 Destruction/
│   └── DestructionComponent  ← Makes buildings crumble using Chaos physics
│
├── 🤖 AI/
│   └── NPCDialogueComponent  ← Villain talks back using GitHub Models / GPT-4o
│
└── ✨ VFX/
    └── VFXManager            ← Manages screen effects and particle explosions

📁 Tools/
├── generate_environment.py   ← Creates a JSON map of a whole city
├── ue5_layout_importer.py    ← Reads that JSON and places buildings in UE5
├── asset_import_pipeline.py  ← Renames and organises 3D art files for UE5
└── texture_validator.py      ← Checks that textures follow the naming rules
```

---

## 6. Implementing Each System in UE5 (Step by Step)

### 6A. The Superhero Character

The `AVilNatureCharacter` class is the base for your hero. Here's how to connect it to something you can actually play in UE5.

**Create a Blueprint from the C++ class:**

1. In UE5, open the **Content Browser** (bottom panel)
2. Right-click in the `Content` folder → **New Folder** → name it `Blueprints`
3. Right-click inside `Blueprints` → **Blueprint Class**
4. In the search box type `VilNatureCharacter`, select it, click **Select**
5. Name the Blueprint `BP_Hero`
6. Double-click `BP_Hero` to open it

**Assign Input Actions (required for movement to work):**

Inside `BP_Hero`, on the right side **Details** panel, find the **VilNature | Input** section:

| Field | What to assign |
|-------|---------------|
| `Default Mapping Context` | Create a new **Input Mapping Context** asset called `IMC_Default` |
| `IA Move` | Create a new **Input Action** called `IA_Move` (type: Axis2D) |
| `IA Look` | Create `IA_Look` (type: Axis2D) |
| `IA Jump` | Create `IA_Jump` (type: Digital) |
| `IA Fly Up` | Create `IA_FlyUp` (type: Axis1D) |
| `IA Boost` | Create `IA_Boost` (type: Digital) |
| `IA Light Attack` | Create `IA_LightAttack` (type: Digital) |
| `IA Heavy Attack` | Create `IA_HeavyAttack` (type: Digital) |
| `IA Eye Laser` | Create `IA_EyeLaser` (type: Digital) |
| `IA Ground Slam` | Create `IA_GroundSlam` (type: Digital) |

**Map keys to those actions (inside `IMC_Default`):**
- Open `IMC_Default`, click **"+"** next to `IA_Move`, add **WASD** keys
- Map `IA_Jump` to **Spacebar**
- Map `IA_FlyUp` to **Spacebar** (hold triggers flight)
- Map `IA_Boost` to **Shift**
- Map `IA_LightAttack` to **Left Mouse Button**
- Map `IA_HeavyAttack` to **Right Mouse Button**
- Map `IA_EyeLaser` to **E**
- Map `IA_GroundSlam` to **Q**

**Set BP_Hero as the default pawn:**
1. Open **Edit → Project Settings → Maps & Modes**
2. Set **Default Pawn Class** to `BP_Hero` ✅

---

### 6B. Flight System

The `FlightComponent` is already attached to the character automatically. You just need to assign VFX assets.

**How flight works:**
- Hold **Spacebar** for 0.3 seconds → flight activates
- **WASD** steers in the air
- Hold **Shift** → boost (4× speed)
- At 85% max speed → a **Sonic Boom** fires automatically

**Assign Niagara effects (optional but recommended):**

1. Open `BP_Hero`
2. Find the **FlightComponent** in the Components panel (left side)
3. In the Details panel, look for `VilNature | VFX` properties
4. Assign Niagara particle systems for:
   - `SonicBoomFX` — a shockwave ring effect
   - `FlightTrailFX` — a speed trail behind the hero

> **Don't have Niagara assets yet?** That's fine. The component will still work; the FX slots will just be empty.

---

### 6C. Combat System

The `CombatComponent` is also automatically attached to the character.

**How combat works:**
- **Left click** → light attack (cycles through up to 4-hit combo)
- **Right click** → heavy attack (sends enemies flying with ragdoll physics)
- **E (hold)** → eye laser beam
- **Q** → ground slam (shockwave in a sphere around you)
- Defeating an enemy triggers a **kill cam** (slow motion cinematic)

**Assign attack animations:**

1. Open `BP_Hero`
2. Click **CombatComponent** in the Components panel
3. In the Details panel, find `VilNature | Animations`:
   - `Light Attack Montages` — add 4 Animation Montages for the combo chain
   - `Heavy Attack Montage` — 1 Animation Montage for the heavy hit
   - `Eye Laser Montage` — 1 Animation Montage for the laser charge pose
   - `Ground Slam Montage` — 1 Animation Montage for the slam landing

> **Don't have animations yet?** The combat system still works — it will just have no visual animation. You can add animations later at any time.

---

### 6D. Destruction System

The destruction uses UE5's built-in **Chaos physics** system. This is what makes buildings crumble when you punch them.

**How to make a building destructible:**

1. In the Content Browser, find (or import) a building Static Mesh
2. Right-click it → **Fracture Mesh** (opens the Fracture Editor)
3. In the Fracture Editor, click **Fracture → Uniform Voronoi** (this splits the mesh into chunks)
4. Click **Fracture** button, then **Close**
5. UE5 will create a **Geometry Collection** asset next to the mesh
6. In your level, drag the Geometry Collection into the scene
7. In the Details panel, click **Add Component** → search for `DestructionComponent`
8. The `DestructionComponent` is now linked ✅

When the player performs a **sonic boom** near this building, the `DestructionComponent` will receive the signal and call Chaos physics to break the building apart.

**Destructible buildings auto-placed by the city generator** (see Section 7) are pre-tagged — they expect the `DestructionComponent` to be assigned exactly this way.

---

### 6E. AI NPC Dialogue (GitHub AI)

This is the coolest part of the project. Villains will say unique things based on what's happening in the fight — powered by the same AI as ChatGPT.

**What you need:**

A **GitHub Personal Access Token** with access to GitHub Models. Here's how to get one:

1. Log into https://github.com
2. Click your profile picture (top right) → **Settings**
3. Scroll down to **Developer Settings** (bottom of the left sidebar)
4. Click **Personal access tokens → Fine-grained tokens**
5. Click **Generate new token**
6. Give it a name like `vilnature-llm-token`
7. Under **Permissions**, you don't need to grant any extra permissions — just create the token
8. Copy the token (it looks like `github_pat_...`)
   - ⚠️ **You only see this once. Copy it now.**

**Set up the token on your computer:**

On **Windows** (in Command Prompt):
```
setx GITHUB_TOKEN "your_token_here"
```
Then **restart** Command Prompt.

On **Mac/Linux** (in Terminal):
```
echo 'export GITHUB_TOKEN="your_token_here"' >> ~/.bashrc
source ~/.bashrc
```

On **Windows** (GUI method):
1. Search "environment variables" in Start Menu
2. Click "Edit the system environment variables"
3. Click "Environment Variables…"
4. Under "User variables", click New
5. Variable name: `GITHUB_TOKEN`, Variable value: your token

**Add the Dialogue Component to a villain Blueprint:**

1. In the Content Browser, create a Blueprint based on `VilNatureCharacter` — call it `BP_Villain`
2. Open `BP_Villain`
3. Click **Add Component** → search for `NPCDialogueComponent`
4. Click the component in the list — in Details you'll see:
   - `Model Endpoint` — leave as default (`https://models.inference.ai.azure.com/chat/completions`)
   - `Model Id` — default is `gpt-4o` (you can change to `phi-3-medium` for faster/cheaper responses)
   - `Api Key` — **leave this empty** (the component reads your `GITHUB_TOKEN` env var automatically)
   - `Villain Persona` — edit this text to describe your villain's personality

**Call dialogue from Blueprint:**

1. Open `BP_Villain`'s Event Graph
2. Right-click → search for `Request Taunt`
3. Connect it to an event (e.g., when the villain takes damage):
   ```
   Event On Damage Taken
       → Request Taunt (Combo Count = ..., Player Health = ..., Context = "player just landed a combo")
   ```
4. To receive the dialogue line: right-click → **Bind Event to On Dialogue Line Ready**
5. In that event, connect the `Dialogue Line` (text string) to a **Print String** or your subtitle UI ✅

---

### 6F. VFX Manager

The `AVFXManager` drives all the screen-space visual effects: chromatic aberration when flying fast, blood spatter when hitting enemies, and heat distortion from eye lasers.

**Place one VFXManager in your level:**

1. In the Content Browser → Blueprints folder → right-click → **Blueprint Class**
2. Search for `VFXManager`, select it, name it `BP_VFXManager`
3. Drag `BP_VFXManager` into your level (just once — one per level is enough)
4. It will automatically be found by the flight, combat, and laser systems ✅

**Create the Material Parameter Collection:**

The VFXManager needs a special asset called `MPC_GlobalVFX`:

1. Content Browser → right-click → **Material Parameter Collection**
2. Name it exactly `MPC_GlobalVFX`
3. Double-click to open it, click **"+"** next to **Scalar Parameters** three times:
   - Name: `ChromaticAberrationAmount`, Default: `0`
   - Name: `BloodSpatterAlpha`, Default: `0`
   - Name: `HeatDistortionStrength`, Default: `0`
4. Save ✅

5. Open `BP_VFXManager`, find the `VFX | Settings` section in Details
6. Assign `MPC_GlobalVFX` to the `Global VFX Collection` property ✅

---

## 7. Using the Python Tools

The `Tools/` folder contains Python scripts that automate repetitive tasks. Make sure you ran `pip install -r Tools/requirements.txt` first (Step 3).

All commands below are run from inside the `PROJECT-VILNATURE` folder in your Terminal.

---

### 7A. Generate a City Layout

This creates a JSON file describing where all the buildings and street lights go.

```bash
python Tools/generate_environment.py --blocks 5 --seed 42
```

**What the options mean:**
- `--blocks 5` — create a 5×5 city grid (25 city blocks). Start with 3 for testing.
- `--seed 42` — the random seed. Same seed = same city every time. Change this to get a different layout.

The output file will be saved to `Content/Maps/layout.json`.

**To also export a CSV** (for Datasmith import):
```bash
python Tools/generate_environment.py --blocks 5 --seed 42 --csv
```

---

### 7B. Import AI-Generated Assets

If you have 3D models or textures from an AI tool (like Meshy, Luma AI, or Midjourney), drop them into a folder and this script will rename and organise them automatically.

**Default: put your files in `/tmp/ai_assets/`** (or wherever you like):

```bash
python Tools/asset_import_pipeline.py --source /tmp/ai_assets
```

**To preview what it would do without actually copying files:**
```bash
python Tools/asset_import_pipeline.py --source /tmp/ai_assets --dry-run
```

**What it does:**
- Detects if a file is a mesh (`.fbx`, `.glb`, etc.) or a texture (`.png`, `.tga`, etc.)
- Renames it following UE5 conventions: `SM_BuildingA.fbx`, `T_BuildingA_D.png`
- Copies it to the correct `Content/` subfolder
- Writes an `import_manifest.json` file (used by the UE5 importer in Section 8)

---

### 7C. Validate Textures

Before committing art assets, run this to catch problems early.

```bash
python Tools/texture_validator.py --path Content/
```

It checks that every texture:
- Has a name starting with `T_` (e.g., `T_BuildingA_D.png`)
- Has power-of-two dimensions (e.g., 512×512, 1024×2048 — NOT 500×600)
- Is within the size limit for its type (diffuse: max 4096px, normal: max 2048px)
- Uses a supported format (PNG, TGA, EXR, HDR)

The results are printed to the terminal and saved to `validation_report.json`. ✅

---

## 8. Auto-Importing the City Layout into UE5

After generating a `layout.json` with the city generator, you can automatically place all the buildings into UE5 without dragging anything by hand.

> **Requirement:** Make sure UE5 is open with the level you want to populate.

**Method 1: Via the UE5 Python Console (Quickest)**

1. In UE5, go to **Window → Output Log**
2. At the bottom of the Output Log, there is a Python console input box
3. Type (replacing the path with your actual project folder):
   ```python
   exec(open('C:/Documents/PROJECT-VILNATURE/Tools/ue5_layout_importer.py').read())
   ```
4. Press **Enter**
5. A progress bar will appear in UE5 and buildings will start appearing in your level ✅

**Method 2: Via Terminal (Commandlet)**

If you have UE5 installed at the standard location:

On **Linux/Mac:**
```bash
/home/ue5/UnrealEngine/Engine/Binaries/Linux/UnrealEditor-Cmd \
    VilNature.uproject \
    -run=pythonscript \
    -script=Tools/ue5_layout_importer.py
```

On **Windows:**
```
"C:\Program Files\Epic Games\UE_5.3\Engine\Binaries\Win64\UnrealEditor-Cmd.exe" ^
    VilNature.uproject ^
    -run=pythonscript ^
    -script=Tools/ue5_layout_importer.py
```

This runs UE5 in **headless mode** (no window) and places all actors, then exits. ✅

---

## 9. How GitHub CI/CD Works (Auto-Checks on Every Save)

**[CI/CD = Continuous Integration / Continuous Deployment]** — Think of it as a robot teammate that checks your work every time you push code to GitHub.

Every time you push changes, GitHub automatically runs two workflows:

### Workflow 1: Asset Validation (`.github/workflows/asset-validation.yml`)

**Triggers when:** You push changes to the `Content/` or `Tools/` folders.

**What it does:**
1. Downloads your code
2. Runs the texture validator
3. Runs the asset import pipeline in dry-run mode
4. Uploads a `validation_report.json` so you can download and review it

**How to see the results:**
1. Go to your repository on https://github.com/dox121-pixel/PROJECT-VILNATURE
2. Click the **Actions** tab
3. Click the latest workflow run
4. Green checkmark ✅ = all good. Red ❌ = something failed.
5. Click a failed step to see the error message.

### Workflow 2: Full Build (`.github/workflows/build.yml`)

**Triggers when:** You push to the `main` or `develop` branches, or create a GitHub Release.

**What it does:**
1. Validates assets (same as above)
2. Compiles all C++ code using Unreal Build Tool
3. **On release only:** Cooks and packages the game for Windows and Linux, uploads it as a downloadable artifact

> **Important:** The build workflow requires a **self-hosted runner** — that's a computer you set up to run UE5. GitHub's own servers don't have UE5 installed. See the Troubleshooting section if the build shows "Waiting for runner."

---

## 10. Future: Making Everything Auto-Implement into UE5

This section is about the roadmap — features that don't exist yet but are planned, and exactly how to build them.

### The Goal
Push code to GitHub → GitHub AI generates new game content → it appears in UE5 automatically, without you clicking anything.

### Step 10A: Set Up a Self-Hosted GitHub Actions Runner (Required)

A **self-hosted runner** is your own computer (or a cloud VM) that GitHub sends jobs to. It needs UE5 installed.

1. On GitHub, go to your repository → **Settings → Actions → Runners**
2. Click **New self-hosted runner**
3. Follow the on-screen instructions for your OS — they give you exact copy-paste commands
4. Install UE5 on this machine (same as Section 2D)
5. Set the environment variable `UE5_ROOT` to your UE5 installation path:
   ```
   UE5_ROOT=/path/to/UnrealEngine
   ```
6. Start the runner service. It will show **Idle** in GitHub Settings when ready ✅

### Step 10B: Add a GitHub Actions Workflow that Runs the Layout Importer

Create a new file `.github/workflows/auto-layout-import.yml`:

```yaml
name: Auto-Import City Layout into UE5

on:
  push:
    paths:
      - 'Content/Maps/layout.json'  # only triggers when the layout file changes

jobs:
  import-layout:
    runs-on: self-hosted  # must be your machine with UE5 installed
    steps:
      - uses: actions/checkout@v4
        with:
          lfs: true

      - name: Run UE5 Layout Importer
        run: |
          $UE5_ROOT/Engine/Binaries/Linux/UnrealEditor-Cmd \
            VilNature.uproject \
            -run=pythonscript \
            -script=Tools/ue5_layout_importer.py
        shell: bash
```

Now, every time `layout.json` changes (because someone ran `generate_environment.py` and pushed it), UE5 will automatically place all the actors. ✅

### Step 10C: Add AI Asset Generation to the Pipeline

To go fully automatic — from GitHub Copilot generating a 3D asset description to it appearing in UE5:

1. **Use GitHub Copilot or the Models API** to generate asset descriptions or configuration files
2. **Add a workflow step** that calls `asset_import_pipeline.py` on newly generated files
3. **Add a final step** that calls the UE5 Python importer

Here is the full future pipeline:

```
Developer describes a new building
        ↓
GitHub Copilot/AI generates mesh settings (JSON)
        ↓
CI workflow runs asset_import_pipeline.py
        ↓
Assets renamed and copied to Content/
        ↓
CI workflow runs ue5_layout_importer.py on self-hosted runner
        ↓
Buildings appear in UE5 level automatically
        ↓
CI workflow packages the updated game build
```

### Step 10D: Automating NPC Dialogue Personas

To have GitHub AI automatically generate new villain personas:

1. Create a new Python script `Tools/generate_villain_persona.py` that calls the GitHub Models API
2. Have it write the persona text to a JSON file like `Content/Villains/personas.json`
3. Add a GitHub Actions workflow step that runs this script and pushes the result
4. In UE5, read that JSON file in `NPCDialogueComponent::BeginPlay()` to load personas at startup

This means you could add a new villain by just pushing a one-line description to GitHub.

### Step 10E: Using GitHub Copilot for Code Changes

**GitHub Copilot** is an AI that writes code for you inside your code editor. To set it up:

1. Install **Visual Studio Code** (https://code.visualstudio.com)
2. Install the **GitHub Copilot extension** from the VS Code marketplace
3. Sign in with your GitHub account
4. Open any `.cpp` or `.h` file from the project
5. Start typing a comment like `// add a new ability that...` and Copilot will suggest code

For UE5 C++ changes to take effect, you still need to:
- Save the file
- Push to GitHub (or recompile locally in UE5)
- UE5 will detect the change and ask to recompile ✅

---

## 11. Troubleshooting Common Problems

### "The project failed to compile when I opened it"
- Make sure you installed UE5 **version 5.3 exactly** (not 5.4 or 5.5)
- In UE5, go to **Tools → Refresh Visual Studio Project** and try again
- Check the **Output Log** (Window → Output Log) for the exact error message

### "I can't fly in-game"
- Make sure `IA_FlyUp` is bound in your Input Mapping Context
- Hold Spacebar for **0.3 seconds** — it won't activate on a tap
- Open `BP_Hero`, click FlightComponent, check that `Max Flight Speed` is greater than 0

### "The villain isn't talking"
- Open the **Output Log** in UE5. Look for errors like `HTTP 401 Unauthorized`
- This means your `GITHUB_TOKEN` environment variable isn't set, or the token is expired
- Generate a new token (Section 6E) and set it again
- After setting the env var, **restart UE5 completely** so it picks up the new value

### "The build workflow says 'Waiting for runner'"
- You need a self-hosted runner with UE5 installed (Section 10A)
- If you don't have one, the build workflow won't run — this is expected
- The **asset validation** workflow (`asset-validation.yml`) runs on GitHub's own servers and works without a self-hosted runner

### "Python says 'Module not found'"
- Run `pip install -r Tools/requirements.txt` again
- Make sure you're using Python 3.11, not an older version: `python --version`

### "Git LFS says it can't download files"
- Run `git lfs authenticate` in your terminal
- Make sure you're logged into GitHub: `git config --global user.email "you@example.com"`

### "The layout importer places buildings but no meshes appear"
- The mesh assets referenced in `layout.json` (e.g., `SM_Building_SkyscraperA`) must exist in your `Content/Environment/Buildings/` folder
- Either import those meshes first, or run `asset_import_pipeline.py` to place AI-generated meshes there first

---

## 12. Quick Reference Cheat Sheet

### Terminal Commands (run from inside the `PROJECT-VILNATURE` folder)

```bash
# Download project for the first time
git clone --recurse-submodules https://github.com/dox121-pixel/PROJECT-VILNATURE
git lfs pull
pip install -r Tools/requirements.txt

# Get latest changes from GitHub
git pull
git lfs pull

# Generate a 5x5 city
python Tools/generate_environment.py --blocks 5 --seed 42

# Import AI assets from /tmp/ai_assets/
python Tools/asset_import_pipeline.py --source /tmp/ai_assets

# Validate textures
python Tools/texture_validator.py --path Content/

# Save and push your changes to GitHub
git add .
git commit -m "Describe what you changed"
git push
```

### In UE5

| Task | How |
|------|-----|
| Place city from layout.json | Window → Output Log → Python console → `exec(open('Tools/ue5_layout_importer.py').read())` |
| Recompile C++ after changes | Tools → Refresh Visual Studio Project, then reopen project |
| See CI check results | Go to GitHub repo → Actions tab |
| Add a new component to a Blueprint | Open Blueprint → Add Component button → search component name |

### Key Folder Locations

| Folder | Contents |
|--------|----------|
| `Source/VilNature/` | All C++ game code |
| `Content/` | Game assets (meshes, textures, maps) — tracked by Git LFS |
| `Tools/` | Python helper scripts |
| `.github/workflows/` | CI/CD automation scripts |
| `Docs/` | Documentation (you are here) |
| `Config/` | UE5 settings files (`.ini`) |

---

*Last updated: 2026 — This guide covers everything in the project as of the initial implementation. As new systems are added, this guide will be updated to match.*
