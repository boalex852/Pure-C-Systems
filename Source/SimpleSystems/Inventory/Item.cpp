// Fill out your copyright notice in the Description page of Project Settings.

#include "Item.h"
#include "SimpleSystems/SimpleSystemsCharacter.h"

// Sets default values
AItem::AItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AItem::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItem::Interact()
{
	ASimpleSystemsCharacter* Player = Cast<ASimpleSystemsCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());

	//Destroy the item if we have picked it up.
	bool bSuccess = Player->InventoryComponent->AddToInvnetory(this);
	if (bSuccess)
	{
		Destroy();
	}
}

bool AItem::ActorDestroyedOnInteract()
{
	return true;
}

#if WITH_EDITOR
void AItem::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FItemData, MaxStackSize))
	{
		if (ItemData.MaxStackSize < 1)
		{
			ItemData.MaxStackSize = 1;
		}
	}
}
#endif

