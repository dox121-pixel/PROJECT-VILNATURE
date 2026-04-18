// VilNatureEditor.Target.cs — Unreal Build Tool editor target for PROJECT-VILNATURE

using UnrealBuildTool;

public class VilNatureEditorTarget : TargetRules
{
    public VilNatureEditorTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Editor;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.Add("VilNature");
    }
}
