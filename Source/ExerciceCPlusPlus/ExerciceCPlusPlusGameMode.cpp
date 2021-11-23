#include "ExerciceCPlusPlusGameMode.h"
#include "ExerciceCPlusPlusCharacter.h"
#include "UObject/ConstructorHelpers.h"

AExerciceCPlusPlusGameMode::AExerciceCPlusPlusGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}

void AExerciceCPlusPlusGameMode::BeginPlay()
{
	Super::BeginPlay();

	//Bind our Player died delegate to the Gamemode's PlayerDied function.
	if (!OnPlayerDied.IsBound())
	{
		OnPlayerDied.AddDynamic(this, &AExerciceCPlusPlusGameMode::PlayerDied);
	}

}

void AExerciceCPlusPlusGameMode::RestartPlayer(AController* NewPlayer)
{
	Super::RestartPlayer(NewPlayer);
}

void AExerciceCPlusPlusGameMode::PlayerDied(ACharacter* Character)
{
	//Get a reference to our Character's Player Controller
	AController* CharacterController = Character->GetController();
	RestartPlayer(CharacterController);
}