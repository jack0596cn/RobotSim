// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HAL/RunnableThread.h"
#include "HAL/Runnable.h"
#include <string>
#include "Sockets.h"
#include "SocketTypes.h"
#include "Windows/WindowsPlatformProcess.h"
#include "SocketSubsystem.h"
#include "Communicator.h"
#include "Async/Async.h"

static const int32 CommandSize = 1024;

class ROBOTSIM_API FReceiveThread : public FRunnable
{
public:
	FReceiveThread(FSocket* client, ACommunicator* _CActor = NULL);
	virtual ~FReceiveThread();

	virtual bool Init() override
	{
		bStopped = false;
		return true;
	}

	virtual uint32 Run() override
	{
		if (!m_Client)
		{
			return 0;
		}

		TArray<uint8> RecvBuffer;
		RecvBuffer.Empty();
		RecvBuffer.AddDefaulted(CommandSize);

		while (!bStopped)
		{
			uint32 PendingDataSize = 0;
			if (m_Client->HasPendingData(PendingDataSize) && PendingDataSize > 0)
			{
				int32 BytesRead = 0;
				FMemory::Memset(RecvBuffer.GetData(), 0, CommandSize);
				if (m_Client->Recv(RecvBuffer.GetData(), CommandSize, BytesRead))
				{
					AsyncTask(ENamedThreads::GameThread, [=]()
					{
						const std::string cstr(reinterpret_cast<const char*>(RecvBuffer.GetData()), RecvBuffer.Num());
						FString ReceivedStr(UTF8_TO_TCHAR(cstr.c_str()));
						CActor->ProcessMsg(ReceivedStr);
					});
				}
			}
			FPlatformProcess::Sleep(0.001f);
		}

		return 1;
	}

	virtual void Stop() override
	{
		bStopped = true;
	}

	virtual void Exit() override
	{
		if (m_Client)
		{
			m_Client->Close();
			ISocketSubsystem* SocketSubsystem = ISocketSubsystem::Get();
			if (SocketSubsystem)
			{
				SocketSubsystem->DestroySocket(m_Client);
			}
		}
		m_Client = nullptr;
		bStopped = true;
	}
private:
	/*loop control*/
	bool bStopped;

	/*client socket*/
	UPROPERTY()
	FSocket* m_Client;

 	UPROPERTY()
 	ACommunicator* CActor;
};