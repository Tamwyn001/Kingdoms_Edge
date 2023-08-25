// Fill out your copyright notice in the Description page of Project Settings.


#include "EOS_PlayerController.h"

#include "EOS_GameMode.h"
#include "EOS_GameMode.h"

void AEOS_PlayerController::OnNetCleanup(UNetConnection* Connection)
{
	//whene finished, allow engine to disconnect
	Super::OnNetCleanup(Connection);
}
