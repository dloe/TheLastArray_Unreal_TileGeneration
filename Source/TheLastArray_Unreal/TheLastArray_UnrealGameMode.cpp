// Copyright Epic Games, Inc. All Rights Reserved.

#include "TheLastArray_UnrealGameMode.h"
#include "TheLastArray_UnrealCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATheLastArray_UnrealGameMode::ATheLastArray_UnrealGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
