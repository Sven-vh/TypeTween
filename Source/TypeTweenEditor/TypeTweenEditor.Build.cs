// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
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
