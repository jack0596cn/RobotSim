// Fill out your copyright notice in the Description page of Project Settings.

#include "ReceiveThread.h"


FReceiveThread::FReceiveThread(FSocket* client, ACommunicator* _CActor)
	: m_Client(client)
	, CActor(_CActor)
{
}

FReceiveThread::~FReceiveThread()
{
	bStopped = true;
}
