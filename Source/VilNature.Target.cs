// VilNature.Target.cs — Unreal Build Tool game target for PROJECT-VILNATURE

using UnrealBuildTool;

public class VilNatureTarget : TargetRules
{
    public VilNatureTarget(TargetInfo Target) : base(Target)
    {
        Type = TargetType.Game;
        DefaultBuildSettings = BuildSettingsVersion.V5;
        IncludeOrderVersion = EngineIncludeOrderVersion.Latest;

        ExtraModuleNames.Add("VilNature");
    }
}
