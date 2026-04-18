"""
ue5_layout_importer.py
======================
UE5 Python Editor Script — run inside the UE5 Editor via the Python Script Plugin.

Reads the layout.json produced by generate_environment.py and spawns actors
into the current level, sets their mesh, transform, and tags.

Usage (from UE5 Python console or via Commandlet):
    import unreal
    exec(open('/path/to/ue5_layout_importer.py').read())

Or via GitHub Actions self-hosted runner:
    $UE5_ROOT/Engine/Binaries/Linux/UnrealEditor-Cmd VilNature.uproject \
        -run=pythonscript -script=Tools/ue5_layout_importer.py
"""

import json
import os
import unreal

LAYOUT_FILE = os.path.join(
    unreal.SystemLibrary.get_project_content_directory(),
    "../Content/Maps/layout.json"
)


def load_layout(path: str) -> dict:
    with open(path, "r") as f:
        return json.load(f)


def place_actors(layout: dict) -> None:
    editor_subsystem = unreal.get_editor_subsystem(unreal.EditorActorSubsystem)
    world            = unreal.EditorLevelLibrary.get_editor_world()

    total = len(layout["actors"])
    with unreal.ScopedSlowTask(total, "Placing environment actors…") as task:
        task.make_dialog(True)

        for entry in layout["actors"]:
            if task.should_cancel():
                break
            task.enter_progress_frame(1, f"Placing {entry['label']}")

            t = entry["transform"]
            location = unreal.Vector(t["x"], t["y"], t["z"])
            rotation = unreal.Rotator(0.0, t["yaw"], 0.0)
            scale    = unreal.Vector(t["scale_x"], t["scale_y"], t["scale_z"])

            # Spawn a static mesh actor
            spawn_transform = unreal.Transform(location, rotation, scale)
            actor = editor_subsystem.spawn_actor_from_class(
                unreal.StaticMeshActor,
                location,
                rotation
            )

            if actor is None:
                continue

            actor.set_actor_label(entry["label"])
            actor.set_actor_scale3d(scale)

            # Assign mesh
            mesh_comp = actor.static_mesh_component
            if mesh_comp:
                mesh_asset = unreal.load_asset(entry["mesh_asset"])
                if mesh_asset:
                    mesh_comp.set_static_mesh(mesh_asset)

            # Apply tags
            for tag in entry.get("tags", []):
                actor.tags.append(unreal.Name(tag))

    unreal.log(f"Placed {total} actors from layout.")


if __name__ == "__main__":
    if not os.path.exists(LAYOUT_FILE):
        unreal.log_warning(f"Layout file not found: {LAYOUT_FILE}")
        unreal.log_warning("Run: python Tools/generate_environment.py first.")
    else:
        layout = load_layout(LAYOUT_FILE)
        unreal.log(f"Loaded layout: {layout['meta']['actor_count']} actors")
        place_actors(layout)
