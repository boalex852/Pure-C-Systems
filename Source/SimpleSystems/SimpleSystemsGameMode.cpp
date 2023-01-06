// Copyright Epic Games, Inc. All Rights Reserved.

#include "SimpleSystemsGameMode.h"
#include "SimpleSystemsCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASimpleSystemsGameMode::ASimpleSystemsGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
