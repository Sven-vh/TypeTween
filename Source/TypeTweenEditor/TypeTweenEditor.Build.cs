using UnrealBuildTool;

public class TypeTweenEditor: ModuleRules
{
    public TypeTweenEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(new string[] {"Core", "CoreUObject", "Engine"});
    }
}
