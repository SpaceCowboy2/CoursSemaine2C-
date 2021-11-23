// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeBouncer.generated.h"

UCLASS()
class EXERCICECPLUSPLUS_API ACubeBouncer : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeBouncer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UPROPERTY(VisibleAnywhere)
		UStaticMeshComponent* StaticMesh;

	UPROPERTY(EditAnywhere)
		float launchForce;
};
