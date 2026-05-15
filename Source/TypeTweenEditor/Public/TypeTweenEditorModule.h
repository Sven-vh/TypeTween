// Copyright 2026 Sven van Huessen. All Rights Reserved.
// TypeTween is released under the MIT License.
// See LICENSE file or https://github.com/Sven-vh/TypeTween/blob/main/LICENSE
#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"

class FTypeTweenEditor : public FDefaultModuleImpl
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

