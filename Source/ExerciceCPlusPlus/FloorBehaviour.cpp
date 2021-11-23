// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorBehaviour.h"

#include "ExerciceCPlusPlusCharacter.h"
#include "GameFramework/Character.h"

// Sets default values
AFloorBehaviour::AFloorBehaviour()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> CubeMesh(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube.1M_Cube'"));

	// Create the static mesh 
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("NomMesh"));

	if (CubeMesh.Succeeded())
	{
		StaticMesh->SetStaticMesh(CubeMesh.Object);
	}

	StaticMesh->OnComponentHit.AddDynamic(this, &AFloorBehaviour::OnHit);

	StaticMesh->SetSimulatePhysics(false);

	// The component that defines the transform (location, rotation, scale) of this Actor in the world, all other components must be attached to this one somehow
	RootComponent = StaticMesh;


}

// Called when the game starts or when spawned
void AFloorBehaviour::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AFloorBehaviour::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFloorBehaviour::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	AExerciceCPlusPlusCharacter* Character = Cast<AExerciceCPlusPlusCharacter>(OtherActor);
	if (Character == nullptr) //< if (!Character)
		return;

	GLog->Log("Dead by lava");
	//Character->LaunchCharacter(FVector(0, 0, launchForce), false, true);
	Character->healthValue = 0;
}