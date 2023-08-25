// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "Interfaces/OnlineSessionInterface.h"


#include "EOS_PlayerController.generated.h"

/**
 * 
 */
UCLASS()
class KINGDOMS_EDGE_API AEOS_PlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void OnNetCleanup(UNetConnection* Connection) override;

	
};
