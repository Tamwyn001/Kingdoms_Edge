// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "EOS_Gameinstance.h"

#include "TamwynOnlineFunctionLibrary.generated.h"


/**
 * 
 */
UCLASS()
class KINGDOMS_EDGE_API UTamwynOnlineFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
		/**
		String-based comparasion of the UniqueNetId. See @UniqueNetIdRepl
		*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Equal (FUniqueNetId)", CompactNodeTitle = "=="), Category = "Online")
		static bool EqualEqual_UniqueNetIdRepl(const FUniqueNetIdRepl& A, const FUniqueNetIdRepl& B);

	UFUNCTION(BlueprintPure, meta = (DisplayName = "To String (FUniqueNetId)", CompactNodeTitle = "->", ScriptMethod = "ToString", Category = "Utilities|String"))
		static FString Conv_UniqueNetIdReplToString(const FUniqueNetIdRepl& InUniqueNetId);
	UFUNCTION(BlueprintPure, meta = (DisplayName = "To String (LobbyId)", CompactNodeTitle = "->", ScriptMethod = "ToString", Category = "Utilities|String"))
		static FString Conv_FTamBPOnlinePartyIdlToString(const FTamBPOnlinePartyId& PartyId);

};
