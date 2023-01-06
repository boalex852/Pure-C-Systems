// Fill out your copyright notice in the Description page of Project Settings.

#include "InventorySlotWidget.h"
#include "Components/TextBlock.h"
#include "Components/SizeBox.h"

void UInventorySlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventorySlotWidget::SetData(const FString& ItemName, const int32 StackSize)
{
	ItemNameText->SetText(FText::FromString(ItemName));
	if (StackSize < 1)
	{
		SlotStackText->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		SlotStackText->SetText(FText::FromString(FString::FromInt(StackSize)));
	}
}