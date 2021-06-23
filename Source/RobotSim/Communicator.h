// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dom/JsonObject.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
#include "HAL/RunnableThread.h"
#include "Communicator.generated.h"


USTRUCT(BlueprintType)
struct FJointState
{
	GENERATED_USTRUCT_BODY()

public:
	FJointState()
	{}

	FJointState(const TArray<FString>& _Names, const TArray<FString>& _Positions)
	{
		Names = _Names;
		Positions = _Positions;
	}

	~FJointState()
	{}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Names;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Positions;

	//TArray<double> Velocities;
	//TArray<double> Efforts;
};

// Delegates
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdatePosDelegate, const FJointState&, RobotJointState);

UCLASS()
class ROBOTSIM_API ACommunicator : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACommunicator();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

public:
	// Delegate
	UPROPERTY(BlueprintAssignable, Category = "ROSCommunicator[Tcp]")
	FUpdatePosDelegate OnUpdatePosDelegate;

	void ProcessMsg(const FString& Msg);

	UFUNCTION(BlueprintCallable, Category = "Socket[TCP]")
	bool SocketCreate(FString IPStr, int32 port);

	bool SocketSend(const FString& message);

	FJointState OnMessage(const FString& Msg);

public:
	/************************************************************************/
	/* TCP                                                                  */
	/************************************************************************/
	FSocket *SockFd;
	FRunnableThread *m_RecvThread;
};


