// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupAndRotateActor.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
APickupAndRotateActor::APickupAndRotateActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MyMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyMesh"));
	MyMesh->SetSimulatePhysics(true);

	RootComponent = MyMesh;

	bHolding = false;
	bGravity = true;
}
// Called when the game starts or when spawned
void APickupAndRotateActor::BeginPlay()
{
	Super::BeginPlay();

	MyCharacter = UGameplayStatics::GetPlayerCharacter(this,0);
	TArray<USceneComponent*> Components;

	MyCharacter->GetComponents(Components);

	if(Components.Num() > 0)
	{
		for(auto& Comp : Components)
		{
			if(Comp->GetName() == "HoldingComponent")
			{
				HoldingComp = Cast<USceneComponent>(Comp);
			}
		}
	}
	
}

// Called every frame
void APickupAndRotateActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(bHolding && HoldingComp)
	{
		SetActorLocationAndRotation(HoldingComp->GetComponentLocation(),HoldingComp->GetComponentRotation());
	}
}

void APickupAndRotateActor::RotateActor()
{
	ControlRotation = GetWorld()->GetFirstPlayerController()->GetControlRotation();
	SetActorRotation(FQuat(ControlRotation));
}

void APickupAndRotateActor::Pickup()
{
	bHolding = !bHolding;
	bGravity = !bGravity;
	MyMesh->SetEnableGravity(bGravity);
	MyMesh->SetSimulatePhysics((bHolding ? false : true));
	MyMesh->SetCollisionEnabled((bHolding ? ECollisionEnabled::NoCollision : ECollisionEnabled::QueryAndPhysics));

	if(!bHolding)
	{
		ForwardVector = MyCharacter->GetCapsuleComponent()->GetForwardVector();
		MyMesh->AddForce(ForwardVector*100000*MyMesh->GetMass());
	}
}


