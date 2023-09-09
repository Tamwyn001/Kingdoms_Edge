// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "Delegates/Delegate.h"

#include "EOS_GameMode.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateUnRegisterPlayer, const bool, bWasSuccessfull);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateRegisterPlayer, const bool, bWasSuccessfull);
/**
 * 
 */
UCLASS()
class KINGDOMS_EDGE_API AEOS_GameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tamwyn's EOS|Session")
	bool bAllExistingPlayersRegistered;

public:
	AEOS_GameMode(const FObjectInitializer& ObjectInitializer);
	UPROPERTY()
	FDelegateUnRegisterPlayer DelegateUnRegisterPlayer;
	UPROPERTY()
	FDelegateRegisterPlayer DelegateRegisterPlayer;

	//Postlogin to confirm connection to eos. HandleNewPlayer For BP side
	virtual void HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer) override;
	/**
	* Registers the player so that EOS knows tqht a slot is taken
	*
	* @param The local session name.
	* @param the players to register.
	* @param Was the player invited? should stay false.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Register Players", Category = "Tamwyn's EOS|Session")
		void RegisterPlayers(const FDelegateRegisterPlayer OnPlayerRegistered, const FName SessionName, const TArray<APlayerController*> PlayersToRegister, const bool bWasInvited);

	/**
	* Unregister the player so that EOS knows that a slot is free. Do not call if player is not intentienally disconnected; he can be reconnected.
	*
	* @param The local session name.
	* @param the players to unregister.
	* @param Was the player invited? should stay false.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Unregister Player", Category = "Tamwyn's EOS|Session")
		void UnregisterPlayer(const FDelegateUnRegisterPlayer OnPlayerUnRegistered, const FName SessionName, const APlayerController* PlayerController);
	
	FDelegateHandle RegisterPlayerDelegateHandle;
	void HandleRegisterPlayer(
		FName SessionName,
		const TArray<FUniqueNetIdRef>& PlayerId,
		bool WasSuccessfull
	);

	FDelegateHandle UnRegisterPlayerDelegateHandle;
	void HandleUnRegisterPlayer(
		FName SessionName,
		const TArray<FUniqueNetIdRef>& PlayerId,
		bool WasSuccessfull
	);
};
