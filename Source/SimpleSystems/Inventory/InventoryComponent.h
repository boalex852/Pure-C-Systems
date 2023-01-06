// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Item.h"
#include "SimpleSystems/UI/InventoryWidget.h"
#include "InventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventorySlot
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FItemData Item;
	
	UPROPERTY(BlueprintReadWrite)
		int32 StackSize = 0;

	FInventorySlot() = default;

	FInventorySlot(FItemData NewItem, int32 NewStackSize)
		: Item(NewItem), StackSize(NewStackSize)
	{
	}
};


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIMPLESYSTEMS_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInventoryComponent();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FInventorySlot> Inventory;

	bool AddToInvnetory(AItem* Item);

	void ToggleInventory();

protected:
	UPROPERTY(EditDefaultsOnly)
	int32 InventorySize = 4;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryWidget> InventoryWidgetClass;

	UUserWidget* InventoryWidget = nullptr;

	// Called when the game starts
	virtual void BeginPlay() override;
};
