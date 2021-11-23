// Fill out your copyright notice in the Description page of Project Settings.


#include "LifeTrigger.h"

#include <filesystem>

#include "ExerciceCPlusPlusCharacter.h"
#include "Components/PointLightComponent.h"
#include "GameFramework/Character.h"

// Sets default values
ALifeTrigger::ALifeTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	pointLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("Point Light"));
	// Create the static mesh 
	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Light Sphere Component"));
	
	BoxComponent->SetCollisionProfileName(TEXT("Trigger"));
	BoxComponent->SetupAttachment(RootComponent);

	BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &ALifeTrigger::OnOverlapBegin);
}

// Called when the game starts or when spawned
void ALifeTrigger::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALifeTrigger::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALifeTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,  AActor* OtherActor,  UPrimitiveComponent* OtherComp,   int32 OtherBodyIndex, 	  bool bFromSweep, 	  const FHitResult &SweepResult )
{
	AExerciceCPlusPlusCharacter* Character = Cast<AExerciceCPlusPlusCharacter>(OtherActor);
	if (Character == nullptr)
	{
		GLog->Log("Marchepaaaas");
		//< if (!Character)
		return;
	}
	GLog->Log("damages");

	Character->healthValue += lifeValue;
	//Character->LaunchCharacter(FVector(0, 0, launchForce), false, true);
}

