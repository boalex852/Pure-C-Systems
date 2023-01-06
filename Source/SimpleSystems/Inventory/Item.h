// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SimpleSystems/Interfaces/Interaction.h"
#include "Item.generated.h"

USTRUCT(BlueprintType)
struct FItemData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString ItemName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString ItemDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int32 MaxStackSize;

	UPROPERTY(BlueprintReadOnly, meta = (EditCondition = "bCanEditClass", EditConditionHides))
	TSubclassOf<AItem> ItemRef;

	bool bCanEditClass = false;
};


UCLASS()
class SIMPLESYSTEMS_API AItem : public AActor, public IInteraction
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItem();

	UPROPERTY(EditDefaultsOnly)
	FItemData ItemData;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Mesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void Interact() override;

	virtual bool ActorDestroyedOnInteract() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
