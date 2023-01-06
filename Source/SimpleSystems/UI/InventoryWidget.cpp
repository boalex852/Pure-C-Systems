// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryWidget.h"
#include "SimpleSystems/SimpleSystemsCharacter.h"
#include "Components/HorizontalBox.h"
#include "Components/HorizontalBoxSlot.h"
#include "Types/SlateEnums.h"

void UInventoryWidget::NativeConstruct()
{
#if WITH_EDITOR
	if (SlotWidget == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Forgot to set widget class"));
		return;
	}
#endif

	ASimpleSystemsCharacter* Player = Cast<ASimpleSystemsCharacter>(GetOwningPlayerPawn());

	//Add the slots.
	for (const auto& CurrentSlot : Player->InventoryComponent->Inventory)
	{
		UInventorySlotWidget* NewSlot = CreateWidget<UInventorySlotWidget>(this, SlotWidget);
		NewSlot->SetData(CurrentSlot.Item.ItemName, CurrentSlot.StackSize);
		UHorizontalBoxSlot* BoxSlot = Cast<UHorizontalBoxSlot>(InventoryHolder->AddChild(NewSlot));
		BoxSlot->SetHorizontalAlignment(EHorizontalAlignment::HAlign_Center);
		BoxSlot->SetVerticalAlignment(EVerticalAlignment::VAlign_Center);
	}
}