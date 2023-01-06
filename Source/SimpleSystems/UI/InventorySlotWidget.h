// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventorySlotWidget.generated.h"



UCLASS(Abstract)
class SIMPLESYSTEMS_API UInventorySlotWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* ItemNameText;

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
	class UTextBlock* SlotStackText;

	virtual void NativeConstruct() override;

public:
	void SetData(const FString& ItemName, const int32 StackSize);
};
