// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
using UnrealBuildTool;

public class TypeTween : ModuleRules {
    public TypeTween(ReadOnlyTargetRules Target) : base(Target) {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;


        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "Engine"
            }
        );


        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "Core", "CoreUObject"
            }
        );
    }
}
