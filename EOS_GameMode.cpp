// Fill out your copyright notice in the Description page of Project Settings.


#include "EOS_GameMode.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "Interfaces/OnlineSessionInterface.h"


AEOS_GameMode::AEOS_GameMode(const FObjectInitializer& ObjectInitializer)
{
	bAllExistingPlayersRegistered = false;
}

void AEOS_GameMode::HandleStartingNewPlayer_Implementation(APlayerController* NewPlayer)
{
	Super::HandleStartingNewPlayer_Implementation(NewPlayer);
	//if (NewPlayer)
	//{
	//	UE_LOG(LogTemp, Warning, TEXT("Player Controller is valid"));
	//	FUniqueNetIdRepl unique_netid_repl;
	//	//gamemode is serverside: local PC is host
	//	if (NewPlayer->IsLocalController())
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("Player is local"));
	//		ULocalPlayer* local_player_ref = NewPlayer->GetLocalPlayer();

	//		if (local_player_ref)
	//		{
	//			//take the unique net ID
	//			unique_netid_repl = local_player_ref->GetPreferredUniqueNetId();
	//			UE_LOG(LogTemp, Warning, TEXT("Local Player ref is valid"));
	//		}
	//		else
	//		{
	//			UNetConnection* remote_netconnection_ref = Cast<UNetConnection>(NewPlayer);
	//			check(IsValid(remote_netconnection_ref));
	//			unique_netid_repl = remote_netconnection_ref->PlayerId;
	//		}
	//	}
	//	//another player joining the match
	//	else
	//	{
	//		UNetConnection* remote_netconnection_ref = Cast<UNetConnection>(NewPlayer);
	//		check(IsValid(remote_netconnection_ref));
	//		unique_netid_repl = remote_netconnection_ref->PlayerId;
	//	}
	//	TSharedPtr<const FUniqueNetId> unique_netid = unique_netid_repl.GetUniqueNetId();
	//	if (unique_netid != nullptr)
	//	{

	//		IOnlineSubsystem* online_subsystem_ref = Online::GetSubsystem(NewPlayer->GetWorld());
	//		IOnlineSessionPtr session_ref = online_subsystem_ref->GetSessionInterface();

	//		//the core of the function
	//		bool bRegistrationSucess = session_ref->RegisterPlayer(FName("Main Session"), *unique_netid, false);
	//		if (bRegistrationSucess)
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("REGISTRATION TO SESSION SUCCESSFULL"));
	//		}
	//		else
	//		{
	//			UE_LOG(LogTemp, Warning, TEXT("FAIL REGISTRATION TO SESSION"))
	//		}
	//	}
	//	else
	//	{
	//		UE_LOG(LogTemp, Warning, TEXT("No valid UniqueNetID: not registered to any subsystem, should happen when game launches, in the menu."));
	//	}
	//}

}

void AEOS_GameMode::RegisterPlayers(const FDelegateRegisterPlayer OnPlayerRegistered,  const FName SessionName, const TArray<APlayerController*> PlayersToRegister, const bool bWasInvited)
{
	DelegateRegisterPlayer = OnPlayerRegistered;
	for (const APlayerController* PlayerConrtollerElem : PlayersToRegister)
	{
		FUniqueNetIdRepl UniqueNetIdRepl;
		if (PlayerConrtollerElem->IsLocalPlayerController())
		{
			ULocalPlayer* LocalPlayer = PlayerConrtollerElem->GetLocalPlayer();
			if (IsValid(LocalPlayer))
			{
				UniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
			}
			else
			{
				UNetConnection* RemoteNetConnection = Cast<UNetConnection>(PlayerConrtollerElem->Player);
				check(IsValid(RemoteNetConnection));
				UniqueNetIdRepl = RemoteNetConnection->PlayerId;
			}
		}
		else
		{
			UNetConnection* RemoteNetConnection = Cast<UNetConnection>(PlayerConrtollerElem->Player);
			check(IsValid(RemoteNetConnection));
			UniqueNetIdRepl = RemoteNetConnection->PlayerId;
		}

		// Get the unique player ID.
		TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
		if (UniqueNetId != nullptr)
		{
			// Get the online session interface.
			IOnlineSubsystem* Subsystem = Online::GetSubsystem(PlayerConrtollerElem->GetWorld());
			IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

			// Register the player with the "KingdomsEdge" session; this name should match the name you provided in CreateSession.
			//to rework wtih return
			if (!Session->RegisterPlayer(SessionName, *UniqueNetId, bWasInvited))
			{
				DelegateRegisterPlayer.ExecuteIfBound(false, PlayerConrtollerElem);
			}
			else
			{
				DelegateRegisterPlayer.ExecuteIfBound(true, PlayerConrtollerElem);
			}
			
		}
		else
		{
			DelegateRegisterPlayer.ExecuteIfBound(false, PlayerConrtollerElem);
			UE_LOG(LogTemp, Error, TEXT("Register Player: No valid UniqueNetId"));
		}
	}

	this->bAllExistingPlayersRegistered = true;

}

void AEOS_GameMode::UnregisterPlayer(const FDelegateUnRegisterPlayer OnPlayerUnRegistered, const FName SessionName, const APlayerController* PlayerController)
{
	check(IsValid(PlayerController));

	// This code handles logins for both the local player (listen server) and remote players (net connection).
	FUniqueNetIdRepl UniqueNetIdRepl;
	if (PlayerController->IsLocalPlayerController())
	{
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		if (IsValid(LocalPlayer))
		{
			UniqueNetIdRepl = LocalPlayer->GetPreferredUniqueNetId();
		}
		else
		{
			UNetConnection* RemoteNetConnection = Cast<UNetConnection>(PlayerController->Player);
			check(IsValid(RemoteNetConnection));
			UniqueNetIdRepl = RemoteNetConnection->PlayerId;
		}
	}
	else
	{
		UNetConnection* RemoteNetConnection = Cast<UNetConnection>(PlayerController->Player);
		check(IsValid(RemoteNetConnection));
		UniqueNetIdRepl = RemoteNetConnection->PlayerId;
	}

	// Get the unique player ID.
	TSharedPtr<const FUniqueNetId> UniqueNetId = UniqueNetIdRepl.GetUniqueNetId();
	check(UniqueNetId != nullptr);

	// Get the online session interface.
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(PlayerController->GetWorld());
	IOnlineSessionPtr Session = Subsystem->GetSessionInterface();

	// Unregister the player with the name you provided in CreateSession.
	if (!Session->UnregisterPlayer(SessionName, *UniqueNetId))
	{
		DelegateUnRegisterPlayer.ExecuteIfBound(false);
		return;
	}
	DelegateUnRegisterPlayer.ExecuteIfBound(true);
	return;
}
