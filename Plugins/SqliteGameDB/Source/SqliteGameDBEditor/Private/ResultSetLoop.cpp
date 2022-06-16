// © Copyright 2022 Graham Chabas, All Rights Reserved


#include "ResultSetLoop.h"

#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "DBSupport.h"
#include "ScopedTransaction.h"
#include "Kismet2/BlueprintEditorUtils.h"

#define LOCTEXT_NAMESPACE "SqliteGameDB"

void UK2Node_ResultSetByIndexLoop::AllocateDefaultPins()
{
	//Super::AllocateDefaultPins();
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Execute);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Completed);
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, UEdGraphSchema_K2::PN_Loop);

	FEdGraphPinType PinType;
	PinType.PinCategory = UEdGraphSchema_K2::PC_Struct;
	PinType.PinSubCategoryObject = FQueryResult::StaticStruct();
	CreatePin(EGPD_Input, PinType, TEXT("Query Result"));
	
}

void UK2Node_ResultSetByIndexLoop::AddInputPin()
{
	if (CanAddPin())
	{
		FScopedTransaction Transaction(LOCTEXT("AddPinTx", "AddPin"));
		Modify();

		AddPinsInner(NumAdditionalInputs + NumBaseInputs);
		++NumAdditionalInputs;

		FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(GetBlueprint());
	}
}

FText UK2Node_ResultSetByIndexLoop::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("ResultSetLoop", "Resultset Index Loop");
}

FText UK2Node_ResultSetByIndexLoop::GetPinNameOverride(const UEdGraphPin& Pin) const
{
	return Super::GetPinNameOverride(Pin);
}

FText UK2Node_ResultSetByIndexLoop::GetPinDisplayName(const UEdGraphPin* Pin) const
{
	return Super::GetPinDisplayName(Pin);
}

FText UK2Node_ResultSetByIndexLoop::GetTooltipText() const
{
	return Super::GetTooltipText();
}

FText UK2Node_ResultSetByIndexLoop::GetKeywords() const
{
	//return Super::GetKeywords();
	FText KeywordsText = FText::FromString(TEXT("SQL Result Set Query Loop"));
	return KeywordsText;
}

FLinearColor UK2Node_ResultSetByIndexLoop::GetNodeTitleColor() const
{
	return Super::GetNodeTitleColor();
}

FText UK2Node_ResultSetByIndexLoop::GetMenuCategory() const
{
	return Super::GetMenuCategory();
}

bool UK2Node_ResultSetByIndexLoop::CanAddPin() const
{
	return (NumAdditionalInputs < GetMaxInputPinsNum());
}

void UK2Node_ResultSetByIndexLoop::RemoveInputPin(UEdGraphPin* Pin)
{
	IK2Node_AddPinInterface::RemoveInputPin(Pin);
}

bool UK2Node_ResultSetByIndexLoop::CanRemovePin(const UEdGraphPin* Pin) const
{
	return IK2Node_AddPinInterface::CanRemovePin(Pin);
}

void UK2Node_ResultSetByIndexLoop::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	//Super::GetMenuActions(ActionRegistrar);
	UClass* actionKey = GetClass();

	if (ActionRegistrar.IsOpenForRegistration(actionKey))
	{
		UBlueprintNodeSpawner* nodeSpawner = UBlueprintNodeSpawner::Create(GetClass());
		check(nodeSpawner != nullptr);

		ActionRegistrar.AddBlueprintAction(actionKey, nodeSpawner);
	}
}

void UK2Node_ResultSetByIndexLoop::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	//Super::ExpandNode(CompilerContext, SourceGraph);

	
}

void UK2Node_ResultSetByIndexLoop::AddPinsInner(int32 AdditionalPinIndex)
{
	//const FEdGraphPinType OutputType();
	//FText OutputPinName = GetNameForPin(AdditionalPinIndex);

	//UEdGraphPin* OutputPin = CreatePin(EGPD_Output, FQueryResultField, *OutputPinName.BuildSourceString());

	//OutputPin->PinFriendlyName = OutputPinName;
}

FText UK2Node_ResultSetByIndexLoop::GetNameForPin(int32 PinIndex)
{
	check(PinIndex < GetMaxInputPinsNum());
	FString Name = TEXT("ColumnIndex_");
	Name.AppendChar(TCHAR('A') + PinIndex);

	FFormatNamedArguments Args;
	Args.Add(TEXT("Identifier"), FText::FromString(Name));
	Args.Add(TEXT("Direction"), LOCTEXT("DoOnceMultiOut", "Out"));

	return FText::Format(LOCTEXT("ResultSetByIndexLoop", "{Identifier} {Direction}"), Args);
}

#undef LOCTEXT_NAMESPACE
