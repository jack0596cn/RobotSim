// Fill out your copyright notice in the Description page of Project Settings.

#include "Communicator.h"
#include "Interfaces/IPv4/IPv4Address.h"
#include "IPAddress.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "ReceiveThread.h"

// Sets default values
ACommunicator::ACommunicator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACommunicator::BeginPlay()
{
	Super::BeginPlay();
}

void ACommunicator::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (m_RecvThread != nullptr)
	{
		m_RecvThread->Kill(true);
		delete m_RecvThread;
		m_RecvThread = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ACommunicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ACommunicator::SocketCreate(FString IPStr, int32 port)
{
	FIPv4Address ip;
	//将传入的IPStr转为IPv4地址
	FIPv4Address::Parse(IPStr, ip);

	TSharedPtr<FInternetAddr> addr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
	addr->SetIp(ip.Value);
	addr->SetPort(port);

	SockFd = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateSocket(NAME_Stream, TEXT("default"), false);

	FString sAddr = addr->ToString(true);
	if (SockFd->Connect(*addr))
	{		
		UE_LOG(LogTemp, Warning, TEXT("Connect ROSBridge: [%s] Succeed!"), *sAddr);
		m_RecvThread = FRunnableThread::Create(new FReceiveThread(SockFd, this), TEXT("RecvThread"));

		FString SubscribeProtocol;
		TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
		JsonObject->SetStringField(TEXT("op"), TEXT("subscribe"));
		JsonObject->SetStringField(TEXT("topic"), TEXT("/joint_states"));
		JsonObject->SetStringField(TEXT("type"), TEXT("sensor_msgs/JointState"));

		TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&SubscribeProtocol);
		FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

		SocketSend(SubscribeProtocol);

		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Connect ROSBridge: [%s] Failed!"), *sAddr);
		return false;
	}

	return true;
}

bool ACommunicator::SocketSend(const FString& message)
{
	TArray<uint8> content;
	content.SetNum(message.Len());
	FMemory::Memcpy(content.GetData(), TCHAR_TO_ANSI(*message), message.Len());

	int32 byteSent = 0;
	if (SockFd && SockFd->Send(content.GetData(), content.Num(), byteSent))
	{
		UE_LOG(LogTemp, Log, TEXT("Message [%s] ___Send Succeed!"), *message);
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Message [%s] ___Send Failed!"), *message);
		return false;
	}

	return true;
}

FJointState ACommunicator::OnMessage(const FString& Msg)
{
	FJointState JointStateObj;

	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Msg);
	if (FJsonSerializer::Deserialize(Reader, JsonObject) || JsonObject.IsValid())
	{
		TArray<FString> name, position;
		const TSharedPtr<FJsonObject>* MsgBody;
		if (JsonObject->TryGetObjectField(TEXT("msg"), MsgBody))
		{
			if ((*MsgBody)->TryGetStringArrayField(TEXT("name"), name))
			{
				JointStateObj.Names = name;
			}
			if ((*MsgBody)->TryGetStringArrayField(TEXT("position"), position))
			{
				JointStateObj.Positions = position;
			}
		}

		return JointStateObj;
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Deserialize Msg Failed!"));
	}

	return JointStateObj;
}

void ACommunicator::ProcessMsg(const FString& Msg)
{
	UE_LOG(LogTemp, Log, TEXT("JsonMsg is: %s"), *Msg);
	FJointState JsonMsg = OnMessage(Msg);
	if (OnUpdatePosDelegate.IsBound())
	{
		OnUpdatePosDelegate.Broadcast(JsonMsg);		
	}
}
