// © Copyright 2022 Graham Chabas, All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_AddPinInterface.h"
#include "UObject/Object.h"
#include "ResultSetLoop.generated.h"

/* Allows blueprints to easily iterate over Query Result Set data. */

UCLASS(BlueprintType, Blueprintable)
class SQLITEGAMEDBEDITOR_API UK2Node_ResultSetByIndexLoop : public UK2Node, public IK2Node_AddPinInterface
{
public:
	virtual void AllocateDefaultPins() override;
	virtual void AddInputPin() override;
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual FText GetPinNameOverride(const UEdGraphPin& Pin) const override;
	virtual FText GetPinDisplayName(const UEdGraphPin* Pin) const override;
	virtual FText GetTooltipText() const override;
	virtual FText GetKeywords() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
	virtual FText GetMenuCategory() const override;
	virtual bool CanAddPin() const override;
	virtual void RemoveInputPin(UEdGraphPin* Pin) override;
	virtual bool CanRemovePin(const UEdGraphPin* Pin) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

protected:

private:
	
	const static int32 NumBaseInputs = 1;

	UPROPERTY()
	int32 NumAdditionalInputs;

	void AddPinsInner(int32 AdditionalPinIndex);
	static FText GetNameForPin(int32 PinIndex);
private:
	GENERATED_BODY()
};
