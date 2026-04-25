#pragma once
#include "CoreMinimal.h"
#include "TweenNodeBase.h"

/* Custom Types */
#include "Blueprints/Specializations/TweenAsyncColor.h"

/* Generated Types*/
#include "Blueprints/Generated/TweenAsyncDouble.h"
#include "Blueprints/Generated/TweenAsyncFloat.h"
#include "Blueprints/Generated/TweenAsyncRotator.h"
#include "Blueprints/Generated/TweenAsyncTransform.h"
#include "Blueprints/Generated/TweenAsyncVector.h"
#include "Blueprints/Generated/TweenAsyncVector2D.h"

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
/* Double */
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

/* Float */
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

/* Rotator */
UCLASS()
class TYPETWEENEDITOR_API UK2Node_TweenRotator : public UK2Node_Tween {
	GENERATED_BODY()

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type /*TitleType*/) const override {
		return NSLOCTEXT("TypeTween", "TweenRotator_Title", "Tween Rotator");
	}
	virtual FText GetMenuCategory() const override {
		return NSLOCTEXT("TypeTween", "TweenRotator_Category", "TypeTween");
	}
	UK2Node_TweenRotator() {
		ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UTweenAsyncRotatorFactory, TweenRotator);
		ProxyFactoryClass = UTweenAsyncRotatorFactory::StaticClass();
		ProxyClass = UTweenAsyncRotator::StaticClass();
	}
};

/* Transform */
UCLASS()
class TYPETWEENEDITOR_API UK2Node_TweenTransform : public UK2Node_Tween {
	GENERATED_BODY()

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type /*TitleType*/) const override {
		return NSLOCTEXT("TypeTween", "TweenTransform_Title", "Tween Transform");
	}
	virtual FText GetMenuCategory() const override {
		return NSLOCTEXT("TypeTween", "TweenTransform_Category", "TypeTween");
	}
	UK2Node_TweenTransform() {
		ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UTweenAsyncTransformFactory, TweenTransform);
		ProxyFactoryClass = UTweenAsyncTransformFactory::StaticClass();
		ProxyClass = UTweenAsyncTransform::StaticClass();
	}
};

/* Vector */
UCLASS()
class TYPETWEENEDITOR_API UK2Node_TweenVector : public UK2Node_Tween {
	GENERATED_BODY()

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type /*TitleType*/) const override {
		return NSLOCTEXT("TypeTween", "TweenVector_Title", "Tween Vector");
	}
	virtual FText GetMenuCategory() const override {
		return NSLOCTEXT("TypeTween", "TweenVector_Category", "TypeTween");
	}
	UK2Node_TweenVector() {
		ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UTweenAsyncVectorFactory, TweenVector);
		ProxyFactoryClass = UTweenAsyncVectorFactory::StaticClass();
		ProxyClass = UTweenAsyncVector::StaticClass();
	}
};

/* Vector2D */
UCLASS()
class TYPETWEENEDITOR_API UK2Node_TweenVector2D : public UK2Node_Tween {
	GENERATED_BODY()

public:
	virtual FText GetNodeTitle(ENodeTitleType::Type /*TitleType*/) const override {
		return NSLOCTEXT("TypeTween", "TweenVector2D_Title", "Tween Vector2D");
	}
	virtual FText GetMenuCategory() const override {
		return NSLOCTEXT("TypeTween", "TweenVector2D_Category", "TypeTween");
	}
	UK2Node_TweenVector2D() {
		ProxyFactoryFunctionName = GET_FUNCTION_NAME_CHECKED(UTweenAsyncVector2DFactory, TweenVector2D);
		ProxyFactoryClass = UTweenAsyncVector2DFactory::StaticClass();
		ProxyClass = UTweenAsyncVector2D::StaticClass();
	}
};