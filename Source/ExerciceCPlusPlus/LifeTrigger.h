// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "LifeTrigger.generated.h"

UCLASS()
class EXERCICECPLUSPLUS_API ALifeTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALifeTrigger();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//UFUNCTION()
	//void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent,  AActor* OtherActor,  UPrimitiveComponent* OtherComp,   int32 OtherBodyIndex, 	  bool bFromSweep, 	  const FHitResult &SweepResult );
	
	UPROPERTY(EditAnywhere,Category= "Variables")
	int lifeValue;
	
	UPROPERTY(VisibleAnywhere, Category = "Light Switch")
	UBoxComponent* BoxComponent;

	UPROPERTY(VisibleAnywhere, Category = "Light Switch")
	class UPointLightComponent* pointLight;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* StaticMesh;
};
