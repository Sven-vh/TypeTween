using UnrealBuildTool;

public class TypeTweenEditor : ModuleRules {
    public TypeTweenEditor(ReadOnlyTargetRules Target) : base(Target) {
        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "TypeTween"
        });

        PrivateDependencyModuleNames.AddRange(new string[] {
            "Core", "CoreUObject", "Engine", "BlueprintGraph", "KismetCompiler", "UnrealEd"
        });
    }
}
