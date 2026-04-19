// VilNature.Build.cs — Unreal Build Tool module rules for PROJECT-VILNATURE

using UnrealBuildTool;

public class VilNature : ModuleRules
{
    public VilNature(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

        // Add the module root so subdirectory-qualified includes (e.g. "Combat/CombatComponent.h") resolve correctly
        PublicIncludePaths.Add(ModuleDirectory);

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "InputCore",
            "EnhancedInput",
            "PhysicsCore",
            "Chaos",
            "GeometryCollectionEngine",
            "Niagara",
            "AIModule",
            "NavigationSystem",
            "GameplayTasks",
            "HTTP",
            "Json",
            "JsonUtilities",
            "UMG",
            "SlateCore",
            "Slate",
            "AudioMixer",
            "SignalProcessing",
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "RenderCore",
            "RHI",
            "ChaosVehicles",
            "HairStrandsCore",
            "AnimGraphRuntime",
        });

        // Enable RTTI for dynamic_cast use in AI/dialogue systems
        bUseRTTI = true;
    }
}
