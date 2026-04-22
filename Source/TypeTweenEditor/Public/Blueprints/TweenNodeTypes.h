#pragma once
#include "CoreMinimal.h"
#include "TweenNodeBase.h"

/* Custom Types */
#include "Blueprints/Specializations/TweenAsyncColor.h"

/* Generated Types*/
#include "Blueprints/Generated/TweenAsyncFloat.h"
#include "Blueprints/Generated/TweenAsyncDouble.h"

#include "TweenNodeTypes.generated.h"

/* Custom Types */
UCLASS()
class TYPETWEENEDITOR_API UK2Node_TweenColor : public UK2Node_Tween {
	GENERATED_BODY()

public:

	virtual FText GetNodeTitle(ENodeTitleType::Type /*TitleType*/) const override {
		return NSLOCTEXT("TypeTween", "TweenColor_Title", "Tween Color");
	}

	virtual FText GetMenuCategory() const override {
		return NSLOCTEXT("TypeTween", "TweenColor_Category", "TypeTween");
	}

	UK2Node_TweenColor() {
		ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UTweenAsyncColorFactory, TweenColor);
		ProxyFactoryClass = UTweenAsyncColorFactory::StaticClass();
		ProxyClass = UTweenAsyncColor::StaticClass();
	}
};

/* Generated Types*/
UCLASS()
class TYPETWEENEDITOR_API UK2Node_TweenFloat : public UK2Node_Tween {
	GENERATED_BODY()

public:

	virtual FText GetNodeTitle(ENodeTitleType::Type /*TitleType*/) const override {
		return NSLOCTEXT("TypeTween", "TweenFloat_Title", "Tween Float");
	}

	virtual FText GetMenuCategory() const override {
		return NSLOCTEXT("TypeTween", "TweenFloat_Category", "TypeTween");
	}

	UK2Node_TweenFloat() {
		ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UTweenAsyncFloatFactory, TweenFloat);
		ProxyFactoryClass = UTweenAsyncFloatFactory::StaticClass();
		ProxyClass = UTweenAsyncFloat::StaticClass();
	}
};

UCLASS()
class TYPETWEENEDITOR_API UK2Node_TweenDouble : public UK2Node_Tween {
	GENERATED_BODY()

public:

	virtual FText GetNodeTitle(ENodeTitleType::Type /*TitleType*/) const override {
		return NSLOCTEXT("TypeTween", "TweenDouble_Title", "Tween Double");
	}

	virtual FText GetMenuCategory() const override {
		return NSLOCTEXT("TypeTween", "TweenDouble_Category", "TypeTween");
	}

	UK2Node_TweenDouble() {
		ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UTweenAsyncDoubleFactory, TweenDouble);
		ProxyFactoryClass = UTweenAsyncDoubleFactory::StaticClass();
		ProxyClass = UTweenAsyncDouble::StaticClass();
	}
};