// Fill out your copyright notice in the Description page of Project Settings.


#include "EOS_Gameinstance.h"
//#include "OnlineSubsystemUtils.h"
//#include "OnlineSubsystem.h"
//#include "Interfaces/OnlineIdentityInterface.h"
//#include "Interfaces/OnlineFriendsInterface.h"
//#include "Private/RedpointInterfaces/OnlineLobbyInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/Guid.h"
#include "UObject/NameTypes.h"


UEOS_Gameinstance::UEOS_Gameinstance(const FObjectInitializer& ObjectInitializer)
{}

//LoginPlayer
void UEOS_Gameinstance::LoginPlayer(const FDelegateLoginEOSSubsystem & OnLoginCompleted, int32 LocalUserNum)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (IdentityPtr)
		{
			//link  bp delegate
			this->DelegateLoginEOSSubsystem = OnLoginCompleted;

			//link cpp callback
			this->LoginDelegateHande = IdentityPtr->AddOnLoginCompleteDelegate_Handle(
				LocalUserNum, 
				FOnLoginComplete::FDelegate::CreateUObject(this, &UEOS_Gameinstance::HandleLoginComplete));

			//next step is done in the callback
			if (!IdentityPtr->AutoLogin(LocalUserNum)) 
			{
				UE_LOG(LogTamEOS, Error, TEXT("EOS Call AutoLogin didn't start. Check if Online Subsystem is implemented!"));
			}
			return;
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("login player: No valid OnlineSubsystem"));
	return;
}

void UEOS_Gameinstance::HandleLoginComplete(const int32 LocalUserNum, const bool bWasSuccessful,const FUniqueNetId &UserID,const FString &Error)
{
	//launch BP delegate and print result in log
	this->DelegateLoginEOSSubsystem.ExecuteIfBound(bWasSuccessful, IOnlineSubsystem::Get()->GetSubsystemName().ToString());
	if (bWasSuccessful)
	{
		UE_LOG(LogTamEOS, Warning, TEXT("Loged in OnlineSubsystem sucessfully!"));
	}
	else
	{
		UE_LOG(LogTamEOS, Error, TEXT("Failed login to the OnlineSubsystem, Reason: %s"), *Error);
	}

	//deregister the cpp delegate
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (IdentityPtr)
		{
			IdentityPtr->ClearOnLoginCompleteDelegate_Handle(LocalUserNum, this->LoginDelegateHande);
			this->LoginDelegateHande.Reset();
		}
	}
	return;
}

//LogoutPlayer
void UEOS_Gameinstance::LogoutPlayer(const FDelegateLogoutEOSSubsystem & OnLogoutCompleted, int32 LocalUserNum)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (IdentityPtr)
		{
			//link  bp delegate
			this->DelegateLogoutEOSSubsystem = OnLogoutCompleted;

			//link cpp callback
			this->LogoutDelegateHande = IdentityPtr->AddOnLogoutCompleteDelegate_Handle(
				LocalUserNum,
				FOnLogoutComplete::FDelegate::CreateUObject(this, &UEOS_Gameinstance::HandleLogoutComplete));

			//next step is done in the callback
			if (!IdentityPtr->Logout(LocalUserNum))
			{
				UE_LOG(LogTamEOS, Error, TEXT("EOS Call Logout didn't start. Check if Online Subsystem is implemented!"));
			}
			return;
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("logout player: No valid OnlineSubsystem"));
}

void UEOS_Gameinstance::HandleLogoutComplete(const int32 LocalUserNum, const bool bWasSuccessful)
{
	//launch BP delegate and print result in log
	this->DelegateLogoutEOSSubsystem.ExecuteIfBound(bWasSuccessful);
	if (bWasSuccessful)
	{
		UE_LOG(LogTamEOS, Warning, TEXT("Loged out OnlineSubsystem sucessfully!"));
	}
	else
	{
		UE_LOG(LogTamEOS, Error, TEXT("Failed logout from OnlineSubsystem!"));
	}

	//deregister the cpp delegate
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (IdentityPtr)
		{
			
			IdentityPtr->ClearOnLogoutCompleteDelegate_Handle(LocalUserNum, this->LogoutDelegateHande);
			this->LogoutDelegateHande.Reset();

			
			IOnlineFriendsPtr OnlineFriend = OnlineSubsystem->GetFriendsInterface();
			OnlineFriend->ClearOnFriendsChangeDelegate_Handle(0, this->OnFriendListChangedDelegateHandle);
			this->OnFriendListChangedDelegateHandle.Reset();

			IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
			Party->ClearOnPartyInvitesChangedDelegate_Handle(this->OnPartyInviteChangedDelegateHandle);
			this->DelegateOnPartyInvitesChanged.Clear();

			Party->ClearOnPartyInviteReceivedExDelegate_Handle(this->OnPartyInviteReceivedExDelegateHandle);
			this->OnPartyInviteReceivedExDelegateHandle.Reset();
		}
	}  
	return;
}

//is player Logged in
bool UEOS_Gameinstance::IsPlayerLoggedIn(int32 LocalUserNum)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (IdentityPtr)
		{
			return IdentityPtr->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn;
			
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("Is player logged in: No valid OnlineSubsystem"));
	return false;
}

//get player name
FString UEOS_Gameinstance::GetPlayerNickname(int32 LocalUserNum)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (IdentityPtr)
		{
			if (IdentityPtr->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn)
			{
				return IdentityPtr->GetPlayerNickname(LocalUserNum);
			}
			UE_LOG(LogTamEOS, Warning, TEXT("get player nickname: Player not logged in"));
			return FString();
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("get player nickname: No valid OnlineSubsystem"));
	return FString();
}

void UEOS_Gameinstance::GetPlayerUniqueNetId(int32 LocalUserNum, FUniqueNetIdRepl& PlayerUniqueNetId)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (IdentityPtr)
		{
			if (IdentityPtr->GetLoginStatus(LocalUserNum) == ELoginStatus::LoggedIn)
			{
				PlayerUniqueNetId = FUniqueNetIdRepl(IdentityPtr->GetUniquePlayerId(LocalUserNum));
				return;
			}
			UE_LOG(LogTamEOS, Error, TEXT("get player uniqueNetID: Player not logged in"));
			PlayerUniqueNetId = FUniqueNetIdRepl();
			return;
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("get player uniqueNetID: No valid OnlineSubsystem"));
	PlayerUniqueNetId = FUniqueNetIdRepl();
	return;
}

void UEOS_Gameinstance::GetPlayerAccount(const FUniqueNetIdRepl& UniqueNetID, FTamBPUserOnlineAccount& UserAccount)
{

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (IdentityPtr)
		{
			UserAccount.UserAccountInfo = IdentityPtr->GetUserAccount(*UniqueNetID.GetUniqueNetId());
			return;
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("get player account: No valid OnlineSubsystem"));
	UserAccount.UserAccountInfo = NULL;
	return;
}

FString UEOS_Gameinstance::GetAuthAttribute(const FTamBPUserOnlineAccount & TargetAccount,const FString Key, bool& Found)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineIdentityPtr IdentityPtr = OnlineSubsystem->GetIdentityInterface();
		if (IdentityPtr)
		{
			FString KeyResult;
			Found = TargetAccount.UserAccountInfo->GetAuthAttribute(Key, KeyResult);
			return(KeyResult);
		}
	}
	return FString();
}

void UEOS_Gameinstance::GetPlayerUniqueIdPlayerController(const APlayerController* PlayerController, FUniqueNetIdRepl& UniqueNetID)
{
	//is controller valid
	if (!IsValid(PlayerController))
	{
		UniqueNetID.SetUniqueNetId(NULL);
		return;
	}
	//is it a local player
	if (PlayerController->IsLocalController())
	{
		ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
		if (IsValid(LocalPlayer))
		{
			UniqueNetID.SetUniqueNetId(LocalPlayer->GetPreferredUniqueNetId().GetUniqueNetId());
			return;
		}
	}
	//is it a remote player
	UNetConnection* RemoteNetConnection = Cast<UNetConnection>(PlayerController->Player);
	if (IsValid(RemoteNetConnection))
	{
		UniqueNetID.SetUniqueNetId(RemoteNetConnection->PlayerId.GetUniqueNetId());
		return;
	}
	UE_LOG(LogTamEOS, Error, TEXT("Player controller does not have a valid remote network connection"));
	return;
}

void UEOS_Gameinstance::GetPlayerUniqueIdPlayerState(const APlayerState* PlayerState, FUniqueNetIdRepl& UniqueNetID)
{
	if (IsValid(PlayerState))
	{
		UniqueNetID.SetUniqueNetId(PlayerState->GetUniqueId().GetUniqueNetId());
		return;
	}
	UniqueNetID.SetUniqueNetId(NULL);
	return;
}

void UEOS_Gameinstance::GetMemberUniqueId(const FTamBPOnlinePartyMember& PartyMember, FUniqueNetIdRepl& UniqueNetID)
{
	UniqueNetID = FUniqueNetIdRepl(*PartyMember.GetPartyMember()->GetUserId());
	return;
}

void UEOS_Gameinstance::QueryUsersInfos(const FDelegateQueryUsersInfo & OnInfoQueried, const TArray< FUniqueNetIdRepl>& UniqueNetIdsToQuery)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineUserPtr User = Subsystem->GetUserInterface();

	this->QueryUserInfoDelegateHandle = User->AddOnQueryUserInfoCompleteDelegate_Handle(0, FOnQueryUserInfoComplete::FDelegate::CreateUObject(
		this, &UEOS_Gameinstance::HandleQueryUserInfoComplete));
	this->DelegateQueryUsersInfo = OnInfoQueried;

	//init data to send
	TArray<TSharedRef<const FUniqueNetId>> UserIds;
	for (auto InUser : UniqueNetIdsToQuery)
	{
		UserIds.Add(InUser.GetUniqueNetId().ToSharedRef());
	}

	if(!User->QueryUserInfo(0, UserIds))
	{
		UE_LOG(LogTamEOS, Error, TEXT("QueryUserInfos, call didn't start!"));
	}
	return;
}

void UEOS_Gameinstance::HandleQueryUserInfoComplete(int32 LocalUserNum,
	bool WasSuccessful,
	const TArray<TSharedRef<const FUniqueNetId>>& UserIds,
	const FString& ErrorStr)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineUserPtr User = Subsystem->GetUserInterface();

	TArray<FUniqueNetIdRepl> UniqueNetIds;
	if (WasSuccessful)
	{
		for (auto UserId : UserIds)
		{
			UniqueNetIds.Add(FUniqueNetIdRepl(UserId.Get()));
		}
	}
	this->DelegateQueryUsersInfo.ExecuteIfBound(WasSuccessful, UniqueNetIds, ErrorStr);

	User->ClearOnQueryUserInfoCompleteDelegate_Handle(0, this->QueryUserInfoDelegateHandle);
	this->QueryUserInfoDelegateHandle.Reset();
	return;
}

bool UEOS_Gameinstance::GetCachedPlateformAvatar(const FUniqueNetIdRepl& TargetUser, UTexture2D& OutTexture)
{
	//directly call steam
	return false;
}

void UEOS_Gameinstance::GetUserInfo(const int32 LocalUserNum,const FUniqueNetIdRepl& UniqueNetID, FTamBPUserInfos & UserInfo)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineUserPtr UserInterface = Subsystem->GetUserInterface();
	if (UniqueNetID.IsValid())
	{
		UserInfo = FTamBPUserInfos(UserInterface->GetUserInfo(LocalUserNum, *UniqueNetID.GetUniqueNetId().Get()));
	}
	return;
}



void UEOS_Gameinstance::CreateParty(const FDelegateCreatePartyCompleted& OnPartyCreated, int32 MaxMembers)
{
	this->DelegateCreatePartyCompleted = OnPartyCreated;
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		FOnlinePartyTypeId PartyTypeId = Party->GetPrimaryPartyTypeId();
		if (Party && Identity)
		{
			PartyConfig->bIsAcceptingMembers = true;
			PartyConfig->MaxMembers = MaxMembers;
			PartyConfig->bShouldRemoveOnDisconnection = true;
			PartyConfig->JoinRequestAction = EJoinRequestAction::AutoApprove;

			//the lambda in the if takes care of calling the function, the delegate, and the bp delgate
			if (!Party->CreateParty(*Identity->GetUniquePlayerId(0).Get(),
				(FOnlinePartyTypeId)PartyTypeId, *PartyConfig,
				FOnCreatePartyComplete::CreateLambda([this](
					const FUniqueNetId& LocalUserId,
					const TSharedPtr<const FOnlinePartyId>& PartyId,
					const ECreatePartyCompletionResult Result)
					{
						
						FTamBPOnlinePartyId OnlinePartyId;
						OnlinePartyId.SetOnlinePartyId(PartyId);
						this->DelegateCreatePartyCompleted.ExecuteIfBound(FUniqueNetIdRepl(LocalUserId), OnlinePartyId,
							static_cast<ETamBPCreatePartyCompletionResult>(Result));
					}
				)))
			{
				UE_LOG(LogTamEOS, Error, TEXT("Create party, call didn't start!"));
			}
		}
	}
	return;
}

void UEOS_Gameinstance::KickMember(const FDelegateKickMemberCompleted & OnMemberKicked, const FUniqueNetIdRepl& UserIdLeader, const FTamBPOnlinePartyId& PartyId, const FUniqueNetIdRepl& MemberToKick)
{
	this->DelegateKickMemberCompleted = OnMemberKicked;
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		FOnlinePartyTypeId PartyTypeId = Party->GetPrimaryPartyTypeId();
		if (Party && Identity)
		{
			//the lambda in the if takes care of calling the function, the delegate, and the bp delgate
			if (!Party->KickMember(
				*UserIdLeader.GetUniqueNetId(),
				*PartyId.GetOnlinePartyId(),
				*MemberToKick.GetUniqueNetId(),
				FOnKickPartyMemberComplete::CreateLambda([this](
					const FUniqueNetId& LocalUserId,
					const FOnlinePartyId& PartyId,
					const FUniqueNetId& KickedMember,
					const EKickMemberCompletionResult Result)
					{
						FTamBPOnlinePartyId OnlinePartyId;
						OnlinePartyId.SetOnlinePartyId(&PartyId);
						this->DelegateKickMemberCompleted.ExecuteIfBound(FUniqueNetIdRepl(LocalUserId),
							OnlinePartyId, FUniqueNetIdRepl(KickedMember), static_cast<ETamBPKickMemberCompletionResult>(Result));
					}
				)))
			{
				UE_LOG(LogTamEOS, Error, TEXT("Kick member, call didn't start!"));
			}
		}
	}
	return;
}

void UEOS_Gameinstance::PromoteMember(const FDelegatePromoteMemberCompleted & OnMemberPromoted, const FUniqueNetIdRepl& UserIdLeader, const FTamBPOnlinePartyId& PartyId, const FUniqueNetIdRepl& UserIdToPromote)
{
	this->DelegatePromoteMemberCompleted = OnMemberPromoted;
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		if (Party && Identity)
		{
			if (!Party->PromoteMember(
				*UserIdLeader.GetUniqueNetId(),
				*PartyId.GetOnlinePartyId(),
				*UserIdToPromote.GetUniqueNetId(),
				FOnPromotePartyMemberComplete::CreateLambda([this](
					const FUniqueNetId& LocalUserId,
					const FOnlinePartyId& PartyId,
					const FUniqueNetId& PromotedMember,
					const EPromoteMemberCompletionResult Result)
					{

						FTamBPOnlinePartyId OnlinePartyId;
						OnlinePartyId.SetOnlinePartyId(&PartyId);
						this->DelegatePromoteMemberCompleted.ExecuteIfBound(FUniqueNetIdRepl(LocalUserId), OnlinePartyId,
							FUniqueNetIdRepl(PromotedMember), static_cast<ETamBPPromoteMemberCompletionResult>(Result));
					})))
			{
				UE_LOG(LogTamEOS, Error, TEXT("Promote member, call didn't start!"));
			}
		}
	}
	return;
}

void UEOS_Gameinstance::LeaveParty(const FDelegateLeavePartyCompleted & OnPartyLeaved, int32 LocalUserNum, const FTamBPOnlinePartyId& PartyId)
{
	this->DelegateLeavePartyCompleted = OnPartyLeaved;
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		if (Party && Identity)
		{
			if (!Party->LeaveParty(
				*Identity->GetUniquePlayerId(LocalUserNum),
				*PartyId.GetOnlinePartyId(),
				FOnLeavePartyComplete::CreateLambda([this](
					const FUniqueNetId& LocalUserId,
					const FOnlinePartyId& PartyId,
					const ELeavePartyCompletionResult Result) 
					{
						FTamBPOnlinePartyId OnlinePartyId;
						OnlinePartyId.SetOnlinePartyId(&PartyId);
						this->DelegateLeavePartyCompleted.ExecuteIfBound(FUniqueNetIdRepl(LocalUserId), OnlinePartyId,
							static_cast<ETamBPLeavePartyCompletionResult>(Result));
					})))
				
			{
				UE_LOG(LogTamEOS, Error, TEXT("Leave Party, call didn't start!"));
			}
		}
	}
	return;
}

bool UEOS_Gameinstance::GetJoinedParties(const FUniqueNetIdRepl & LocalUserId, TArray<FTamBPOnlinePartyId>& OutPartyIdArray)
{
	OutPartyIdArray.Empty();
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		if (Party && Identity)
		{
			TArray<TSharedRef<const FOnlinePartyId>> PartyIdArray;
			if (Party->GetJoinedParties(*LocalUserId.GetUniqueNetId(), PartyIdArray))
			{
				
				for (const TSharedRef<const FOnlinePartyId>& PartyIdElem : PartyIdArray)
				{
					FTamBPOnlinePartyId TempPartyId;
					TempPartyId.SetOnlinePartyId(PartyIdElem);
					OutPartyIdArray.Add(TempPartyId);
				}
				return true;
			}
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("Get joined Parties, call didn't start!"));
	return false;
}

bool UEOS_Gameinstance::GetPartyMembers(int32 LocalUserNum, const FTamBPOnlinePartyId& PartyId, TArray<FTamBPOnlinePartyMember>& OutMembers)
{
	OutMembers.Empty();
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		if (Party && Identity)
		{
			TArray<FOnlinePartyMemberConstRef> MemberArray;
			if (Party->GetPartyMembers(
				*Identity->GetUniquePlayerId(LocalUserNum).Get(),
				*PartyId.GetOnlinePartyId(),
				MemberArray)){
				for (const FOnlinePartyMemberConstRef& MemberElem : MemberArray)
				{
					FTamBPOnlinePartyMember TempMember;
					TempMember.SetPartyMember(MemberElem);
					OutMembers.Add(TempMember);
				}
			}
		}
	}

	return false;
}

void UEOS_Gameinstance::RequestFriendPartyJoinInfo(const FDelegateEOSGetFriendPartyJoinInfo & OnPartyFound, const FUniqueNetIdRepl& PlayerRequesting, const FUniqueNetIdRepl& Recipient, const TArray<FTamBPPartyMetadata>& SearchFilters, const int ResultsLimit)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	IOnlinePartyPtr PartyInterface = OnlineSubsystem->GetPartyInterface();
	IOnlineLobbyPtr LobbyInterface = Online::GetLobbyInterface(OnlineSubsystem);

	//parties are an extention of lobbies, so we use the lobby interface to search a party
	TSharedRef<FOnlineLobbySearchQuery> Search = MakeShared<FOnlineLobbySearchQuery>();
	
	// To search by settings, add a LobbySetting and SettingValue to search for:
	for(FTamBPPartyMetadata It_Filter : SearchFilters)
	{
		Search->Filters.Add(
			FOnlineLobbySearchQueryFilter(
				It_Filter.AttributeName,
				It_Filter.GetAttributeValue(),
				EOnlineLobbySearchQueryFilterComparator::Equal));
	}
	Search->Limit = ResultsLimit;

	this->DelegateEOSGetFriendPartyJoinInfo = OnPartyFound;

	if (!LobbyInterface->Search(
		*PlayerRequesting.GetUniqueNetId(),
		*Search,
		FOnLobbySearchComplete::CreateLambda([this](const FOnlineError& Error,
			const FUniqueNetId& UserId,
			const TArray<TSharedRef<const FOnlineLobbyId>>& Lobbies)
			{

				if (Error.WasSuccessful())
				{
					UE_LOG(LogTamEOS, Warning, TEXT("Request Friend join Lobby info suceeded, found %ld results!"), Lobbies.Num());
					if (Lobbies.Num() == 0)
					{
						this->DelegateEOSGetFriendPartyJoinInfo.ExecuteIfBound(false, FTamBPOnlinePartyJoinInfo(), FString(TEXT("No results found with passed filters")));
					}
					for (auto It_Lobby : Lobbies)
					{
						IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
						IOnlinePartyPtr PartyInterface = OnlineSubsystem->GetPartyInterface();

						IOnlineLobbyPtr LobbyInterface = Online::GetLobbyInterface(OnlineSubsystem);


						FVariantData JoinTokenResult;
						LobbyInterface->GetLobbyMetadataValue(UserId, It_Lobby.Get(), FString(TEXT("JoinInfoJson")), JoinTokenResult);

						FTamBPOnlinePartyJoinInfo JoinInfo(PartyInterface->MakeJoinInfoFromJson(JoinTokenResult.ToString()));
						this->DelegateEOSGetFriendPartyJoinInfo.ExecuteIfBound(true, JoinInfo, Error.GetErrorMessage().ToString());

						return;
					}
				}
				else
				{
					this->DelegateEOSGetFriendPartyJoinInfo.ExecuteIfBound(false, FTamBPOnlinePartyJoinInfo(), Error.GetErrorMessage().ToString());
					UE_LOG(LogTamEOS, Warning, TEXT("Request Friend join Lobby info failed, error message: %s : %s!"), *Error.GetErrorCode(), *Error.GetErrorMessage().ToString());
					return;
				}
			}
	)))
	{
		UE_LOG(LogTamEOS, Warning, TEXT("Request Friend Party join info, call search() didn't start"));
		return;
	}
}

FString UEOS_Gameinstance::GetPartyJoinJson(const FUniqueNetIdRepl& LocalUserId, const FTamBPOnlinePartyId& PartyId)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	IOnlinePartyPtr PartyInterface = OnlineSubsystem->GetPartyInterface();

	return PartyInterface->MakeJoinInfoJson(*LocalUserId.GetUniqueNetId(), *PartyId.GetOnlinePartyId());
	
}
bool UEOS_Gameinstance::UpdatePartyMetadata(const FUniqueNetIdRepl& LocalUserId, const FTamBPOnlinePartyId& PartyId, const TArray<FTamBPPartyMetadata>& MetadataToUpload)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	IOnlinePartyPtr PartyInterface = OnlineSubsystem->GetPartyInterface();

	auto ReadOnlyData = PartyInterface->GetPartyData(*LocalUserId.GetUniqueNetId(), *PartyId.GetOnlinePartyId(), NAME_Default);
	if (!ReadOnlyData.IsValid())
	{
		// Party Data isn't valid, return error.
		UE_LOG(LogTamEOS, Error, TEXT("UpdatePartyMetadata failed: unable to make a mutable copy!"));
		return false;
	}

	auto PartyData = MakeShared<FOnlinePartyData>(*ReadOnlyData);

	for(FTamBPPartyMetadata It_ToUploadMetadata : MetadataToUpload)
	{
		PartyData->SetAttribute(It_ToUploadMetadata.AttributeName, It_ToUploadMetadata.GetAttributeValue());
	}

	if (!PartyInterface->UpdatePartyData(*LocalUserId.GetUniqueNetId(), *PartyId.GetOnlinePartyId(), NAME_Default, *PartyData))
	{
		UE_LOG(LogTamEOS, Error, TEXT("UpdatePartyMetadata failed: unable to set data!"));
		return false;
	}
	return true;
}

void UEOS_Gameinstance::UpdatePartyAdvancedMetadata(const FDelegateUpdateAdvancedPartyMetadata & OnMetadataUpdated, const FUniqueNetIdRepl& LocalUserId, const FTamBPOnlinePartyId& PartyId, const bool IsPublic, const bool IsLocked, const int32 NewCapacity)
{

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineLobbyPtr LobbyInterfarce = Online::GetLobbyInterface(Subsystem);

	TSharedPtr<FOnlineLobbyTransaction> Txn = LobbyInterfarce->MakeUpdateLobbyTransaction(*LocalUserId.GetUniqueNetId(), *PartyId.GetOnlinePartyId());

	// To change the visibility of the lobby, set the Public value. If you
	// don't set a value for this, it leaves the setting unchanged.
	Txn->Public = IsPublic;
	// To change whether players can join the lobby, set the Locked value.
	Txn->Locked = IsLocked;
	// To change the capacity of the lobby, set the Capacity value.
	Txn->Capacity = NewCapacity;

	this->DelegateUpdateAdvancedPartyMetadata = OnMetadataUpdated;
	if (!LobbyInterfarce->UpdateLobby(
		*LocalUserId.GetUniqueNetId(),
		*PartyId.GetOnlinePartyId(),
		*Txn,
		FOnLobbyOperationComplete::CreateLambda([this](
			const FOnlineError& Error,
			const FUniqueNetId& UserId,
			const TSharedPtr<class FOnlineLobby>& Lobby)
			{
				this->DelegateUpdateAdvancedPartyMetadata.ExecuteIfBound(Error.WasSuccessful(), *Error.GetErrorMessage().ToString());
				if (Error.WasSuccessful())
				{
					UE_LOG(LogTamEOS, Warning, TEXT("Updated advanced Party Metadata sucessfull!"));
				}
				else
				{
					UE_LOG(LogTamEOS, Warning, TEXT("Updated advanced Party Metadata, call failed, %s"), *Error.GetErrorMessage().ToString());
				}
			})))
	{
		UE_LOG(LogTamEOS, Warning, TEXT("Updated advanced Party Metadata, call didn't start"));
	}
}

void UEOS_Gameinstance::SendPartyInvite(FDelegateInviteMemberCompleted OnInviteCompleted, const FUniqueNetIdRepl& Sender, const FTamBPOnlinePartyId& PartyId, const FUniqueNetIdRepl& Recipient)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		if (Party && Identity)
		{
			this->DelegateInviteMemberCompleted = OnInviteCompleted;
			if (!Party->SendInvitation(
				*Sender,
				*PartyId.GetOnlinePartyId(),
				*Recipient,
				FOnSendPartyInvitationComplete::CreateLambda([this](
					const FUniqueNetId& LocalUserId,
					const FOnlinePartyId& PartyId,
					const FUniqueNetId& RecipientId,
					const ESendPartyInvitationCompletionResult Result)
					{
						FTamBPOnlinePartyId OnlinePartyId;
						OnlinePartyId.SetOnlinePartyId(&PartyId);
						this->DelegateInviteMemberCompleted.ExecuteIfBound(FUniqueNetIdRepl(LocalUserId),
							OnlinePartyId, FUniqueNetIdRepl(RecipientId),
							static_cast<ETamBPSendPartyInvitationCompletionResult>(Result));
					})))
			{
				UE_LOG(LogTamEOS, Error, TEXT("Invite Player to Party, call didn't start!"));
			}
		}
	}
	return;
}

void UEOS_Gameinstance::CancelPartyInvite(const FDelegateCancelInviteMemberCompleted OnCancelCompleted, const FUniqueNetIdRepl& Sender, const FTamBPOnlinePartyId& PartyId, const FUniqueNetIdRepl& Recipient)
{
	IOnlineSubsystem* Subsytem = Online::GetSubsystem(this->GetWorld());
	IOnlinePartyPtr Party = Subsytem->GetPartyInterface();

	this->DelegateCancelInviteMemberCompleted = OnCancelCompleted;

	Party->CancelInvitation(*Sender.GetUniqueNetId(), *Recipient.GetUniqueNetId(), *PartyId.GetOnlinePartyId(),
		FOnCancelPartyInvitationComplete::CreateLambda([this](
			const FUniqueNetId& Sender,
			const FOnlinePartyId& PartyId,
			const FUniqueNetId& Recepient,
			const FOnlineError& Error)
			{
				FTamBPOnlinePartyId OnlinePartyId;
				OnlinePartyId.SetOnlinePartyId(&PartyId);
				this->DelegateCancelInviteMemberCompleted.ExecuteIfBound(FUniqueNetIdRepl(Sender),
					OnlinePartyId,
					FUniqueNetIdRepl(Recepient),
					Error.GetErrorCode());
			}
		));
}

bool UEOS_Gameinstance::GetPlayerPendingInvites(const FUniqueNetIdRepl& UserId, TArray<FTamBPOnlinePartyJoinInfo>& OutInviteList)
{
	OutInviteList.Empty();
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		if (Party && Identity)
		{
			
			TArray<IOnlinePartyJoinInfoConstRef> InvitesArray;
			bool Found = Party->GetPendingInvites(*UserId.GetUniqueNetId(), InvitesArray);
			if (Found)
			{
				for (const IOnlinePartyJoinInfoConstRef InviteElem : InvitesArray)
				{
					FTamBPOnlinePartyJoinInfo TempInvite = FTamBPOnlinePartyJoinInfo(InviteElem);
					OutInviteList.Add(TempInvite);
				}
			}
			return Found;
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("Get pending invites, call didn't start!"))
	return false;
}

void UEOS_Gameinstance::ListenForChangePartiesInvite(const FDelegateOnPartyInvitesChanged & OnChanged)
{
	this->DelegateOnPartyInvitesChanged = OnChanged;
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		if (Party && Identity)
		{
			this->OnPartyInviteChangedDelegateHandle = Party->AddOnPartyInvitesChangedDelegate_Handle(
				FOnPartyInvitesChanged::FDelegate::CreateUObject(this, &UEOS_Gameinstance::HandleOnPartyInviteChanged));
			return;
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("Listen for invite changes, call didn't start!"));
	return;
}

void UEOS_Gameinstance::HandleOnPartyInviteChanged(const FUniqueNetId& LocalUserId)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();

	TArray<IOnlinePartyJoinInfoConstRef> PendingInvites;
	Party->GetPendingInvites(LocalUserId, PendingInvites);

	TArray<FTamBPOnlinePartyJoinInfo> OutInviteList;
	for (auto &It_Invite : PendingInvites)
	{
		OutInviteList.Add(FTamBPOnlinePartyJoinInfo(It_Invite));
	}

	this->DelegateOnPartyInvitesChanged.ExecuteIfBound(LocalUserId, OutInviteList);
	return;
}

void UEOS_Gameinstance::ListenForReceivePartiesInvite(const FDelegateOnPartyInvitesReceived & OnReceived)
{
	this->DelegateOnPartyInvitesReceived = OnReceived;
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		if (Party && Identity)
		{
			//AddOnPartyInviteReceivedDelegate_Handle depreciated,add the "ex"
			this->OnPartyInviteReceivedExDelegateHandle = Party->AddOnPartyInviteReceivedExDelegate_Handle(
				FOnPartyInviteReceivedEx::FDelegate::CreateUObject(
				this,
				&UEOS_Gameinstance::HandleOnPartyInviteReceivedEx));
			return;

		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("Listen for invite receive, call didn't start!"));
	return;
}

void UEOS_Gameinstance::HandleOnPartyInviteReceivedEx(const FUniqueNetId& LocalUserId,const IOnlinePartyJoinInfo& InviationToParty)
{
	
	FTamBPOnlinePartyJoinInfo OnlinePartyJoinInfo(InviationToParty);
	this->DelegateOnPartyInvitesReceived.ExecuteIfBound(FUniqueNetIdRepl(LocalUserId), OnlinePartyJoinInfo);
	return;
}

void UEOS_Gameinstance::JoinParty(const FDelegateJoinPartyCompleted & OnJoined, const FUniqueNetIdRepl& UserId, const FTamBPOnlinePartyJoinInfo& PartyToJoin)
{
	if (PartyToJoin.IsValid())
	{
		IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
		if (OnlineSubsystem)
		{
			IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
			IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
			if (Party && Identity)
			{
				this->DelegateJoinPartyCompleted = OnJoined;
				if (!Party->JoinParty(
					*UserId,
					PartyToJoin.GetParty(),
					FOnJoinPartyComplete::CreateLambda([this](
						const FUniqueNetId& LocalUserId,
						const FOnlinePartyId& PartyId,
						const EJoinPartyCompletionResult Result,
						const int32 NotApprovedReason) {
							FTamBPOnlinePartyId OnlinePartyID;
							OnlinePartyID.SetOnlinePartyId(&PartyId);
							this->DelegateJoinPartyCompleted.ExecuteIfBound(FUniqueNetIdRepl(LocalUserId),
								OnlinePartyID,static_cast<ETamBPJoinPartyCompletionResult>(Result),
								NotApprovedReason);
						})))
				{
					UE_LOG(LogTamEOS, Error, TEXT("Join Partie, call didn't start!"));
				}
			}
		}
	}
	return;
}

void UEOS_Gameinstance::SetupLeaderListens(const FDelegateOnPartyJoinRequestReceived & JoinRequestReceived)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		if (Party && Identity)
		{
			this->DelegateOnPartyJoinRequestReceived = JoinRequestReceived;
			Party->AddOnPartyJoinRequestReceivedDelegate_Handle(FOnPartyJoinRequestReceived::FDelegate::CreateUObject(
				this, &UEOS_Gameinstance::HandleOnPartyJoinRequestReceived));
			return;
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("Setup Leader Listens, call didn't start!"));
	return;
}

void UEOS_Gameinstance::HandleOnPartyJoinRequestReceived(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const IOnlinePartyPendingJoinRequestInfo& JoinRequestInfos)
{
	FTamBPOnlinePartyId OnlinePartyId;
	OnlinePartyId.SetOnlinePartyId(&PartyId);
	FTamBPOnlinePartyPendingJoinRequestInfoConstRef RequestInfo;
	const TSharedPtr<const IOnlinePartyPendingJoinRequestInfo> PendingJoinRequest= MakeShareable(&JoinRequestInfos);
	RequestInfo.SetPendingJoinRequest(PendingJoinRequest);
	this->DelegateOnPartyJoinRequestReceived.ExecuteIfBound(FUniqueNetIdRepl(LocalUserId), OnlinePartyId, RequestInfo);
}

void UEOS_Gameinstance::SetupMembersListens(const FDelegateOnPartyMemberJoined & OnPartyMemberJoined, const FDelegateOnPartyMemberExited & OnPartyMemberExited, const FDelegateOnPartyMemberPromoted & OnPartyMemberPromoted)
{

	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlinePartyPtr Party = OnlineSubsystem->GetPartyInterface();
		IOnlineIdentityPtr Identity = OnlineSubsystem->GetIdentityInterface();
		if (Party && Identity)
		{
			this->DelegateOnPartyMemberExited = OnPartyMemberExited;
			this->DelegateOnPartyMemberJoined = OnPartyMemberJoined;
			this->DelegateOnPartyMemberPromoted = OnPartyMemberPromoted;
			Party->AddOnPartyMemberExitedDelegate_Handle(FOnPartyMemberExited::FDelegate::CreateUObject(this, &UEOS_Gameinstance::HandleOnPartyMemberExited));
			Party->AddOnPartyMemberJoinedDelegate_Handle(FOnPartyMemberJoined::FDelegate::CreateUObject(this, &UEOS_Gameinstance::HandleOnPartyMemberJoined));
			Party->AddOnPartyMemberPromotedDelegate_Handle(FOnPartyMemberPromoted::FDelegate::CreateUObject(this, &UEOS_Gameinstance::HandleOnPartyMemberPromoted));
			return;
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("Setup Members Listens, call didn't start!"));
	return;
}

void UEOS_Gameinstance::HandleOnPartyMemberExited(const FUniqueNetId& LocalUserId,const FOnlinePartyId& PartyId,const FUniqueNetId& MemberId,	const EMemberExitedReason Reason)
{
	FTamBPOnlinePartyId OnlinePartyId;
	OnlinePartyId.SetOnlinePartyId(&PartyId);
	this->DelegateOnPartyMemberExited.ExecuteIfBound(FUniqueNetIdRepl(LocalUserId), OnlinePartyId,
		FUniqueNetIdRepl(MemberId), static_cast<ETamBPMemberExitedReason>(Reason));
}

void UEOS_Gameinstance::HandleOnPartyMemberJoined(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FUniqueNetId& NewMemberId)
{
	FTamBPOnlinePartyId OnlinePartyId;
	OnlinePartyId.SetOnlinePartyId(&PartyId);
	this->DelegateOnPartyMemberJoined.ExecuteIfBound(FUniqueNetIdRepl(LocalUserId), OnlinePartyId,
		FUniqueNetIdRepl(NewMemberId));
}

void UEOS_Gameinstance::HandleOnPartyMemberPromoted(const FUniqueNetId& LocalUserId, const FOnlinePartyId& PartyId, const FUniqueNetId& NewLeaderId)
{
	FTamBPOnlinePartyId OnlinePartyId;
	OnlinePartyId.SetOnlinePartyId(&PartyId);
	this->DelegateOnPartyMemberJoined.ExecuteIfBound(FUniqueNetIdRepl(LocalUserId), OnlinePartyId,
		FUniqueNetIdRepl(NewLeaderId));
}

bool UEOS_Gameinstance::IsMemberPartyLeader(const FUniqueNetIdRepl& LocalUserId, const FTamBPOnlinePartyId& PartyId, const FUniqueNetIdRepl& MemberUniqueId)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	IOnlinePartyPtr PartySystem = OnlineSubsystem->GetPartyInterface();

	return PartySystem->IsMemberLeader(*LocalUserId.GetUniqueNetId(), *PartyId.GetOnlinePartyId(), *MemberUniqueId.GetUniqueNetId());
}

FUniqueNetIdRepl UEOS_Gameinstance::GetPartyLeader(const FUniqueNetIdRepl& LocalUserId, const FTamBPOnlinePartyId& PartyId)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	IOnlinePartyPtr PartySystem = OnlineSubsystem->GetPartyInterface();

	TArray<FOnlinePartyMemberConstRef> Users;
	PartySystem->GetPartyMembers(*LocalUserId.GetUniqueNetId(), *PartyId.GetOnlinePartyId(), Users);

	for (auto It_User : Users)
	{
		if (PartySystem->IsMemberLeader(*LocalUserId.GetUniqueNetId(), *PartyId.GetOnlinePartyId(), *It_User->GetUserId()))
		{
			return FUniqueNetIdRepl(*It_User->GetUserId());
		}
	}
	return FUniqueNetIdRepl();
}



void UEOS_Gameinstance::CreateEOSSession(const FDelegateEOSSessionCreated & OnSessionCreated, const FUniqueNetIdRepl& HostingPlayerId, const FName SessionName, const bool bIsDedicated, const int32 MaxNumOfPublicConnection, const int32 MaxNumOfPrivateConnection)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();
		if (Session)
		{
			this->DelegateEOSSessionCreated = OnSessionCreated;
			this->CreateSessionDelegateHandle = Session->AddOnCreateSessionCompleteDelegate_Handle(FOnCreateSessionComplete::FDelegate::
				CreateUObject(this, &UEOS_Gameinstance::HandleCreateSessionCompleted));
			SessionSettings->bAllowInvites = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bUseLobbiesIfAvailable = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bUsesPresence = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bIsDedicated = bIsDedicated;
			SessionSettings->NumPublicConnections = MaxNumOfPublicConnection;
			SessionSettings->NumPrivateConnections = MaxNumOfPrivateConnection;


			SessionSettings->Settings.Add(
				FName(TEXT("SessionSetting")),
				FOnlineSessionSetting(FString(TEXT("SettingValue")), EOnlineDataAdvertisementType::ViaOnlineService));
			//set a search keyword, show up in the portal
			SessionSettings->Set(SEARCH_KEYWORDS, SessionName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
			// Create a session and give the local name "MyLocalSessionName". This name is entirely local to the current player and isn't stored in EOS.
			if (!Session->CreateSession(0, SessionName, *SessionSettings))
			{
				UE_LOG(LogTamEOS, Error, TEXT("Create EOS Session, call didn't start!"));
				return;
			}
		}
	}
	return;
}

void UEOS_Gameinstance::HandleCreateSessionCompleted(const FName SessionName, const bool bWasSuccessful)
{
	
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();
		if (Session)
		{
			Session->ClearOnCreateSessionCompleteDelegate_Handle(this->CreateSessionDelegateHandle);
			this->CreateSessionDelegateHandle.Reset();
			this->DelegateEOSSessionCreated.ExecuteIfBound(bWasSuccessful, Session->GetSessionSettings(SessionName)->BuildUniqueId);
			return;
		}
	}
	this->DelegateEOSSessionCreated.ExecuteIfBound(bWasSuccessful,-1);
	return;
}

void UEOS_Gameinstance::DestroyEOSSession(const FDelegateEOSSessionDestroyed & OnSessionDestroyed, const FName SessionName)
{
	
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();
		if (Session)
		{
			this->DelegateEOSSessionDestroyed = OnSessionDestroyed;
			this->DestroySessionDelegateHandle=
				Session->AddOnDestroySessionCompleteDelegate_Handle(FOnDestroySessionComplete::FDelegate::CreateUObject(
				this,
				&UEOS_Gameinstance::HandleDestroySessionCompleted));
			if (!Session->DestroySession(SessionName))
			{
				UE_LOG(LogTamEOS, Error, TEXT("Destroy EOS Session, call didn't start!"));
				return;
			}
		}
	}
	return;
}

void UEOS_Gameinstance::HandleDestroySessionCompleted(const FName SessionName, const bool  bWasSuccessful)
{
	this->DelegateEOSSessionDestroyed.ExecuteIfBound(bWasSuccessful);
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	if (OnlineSubsystem)
	{
		IOnlineSessionPtr Session = OnlineSubsystem->GetSessionInterface();
		if (Session)
		{
			Session->ClearOnDestroySessionCompleteDelegate_Handle(this->DestroySessionDelegateHandle);
			this->DestroySessionDelegateHandle.Reset();
		}
	}
	return;
}

void UEOS_Gameinstance::SearchEOSSession(const FDelegateEOSSessionFound & OnSessionsFound, const int32 MaxResult)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{	
			// Remove the default search parameters that FOnlineSessionSearch sets up.
			SessionSearch->QuerySettings.SearchParams.Empty();

			//SetingUp the filters
			SessionSearch->QuerySettings.Set(FName(TEXT("SessionSetting")), FString(TEXT("SettingValue")),
				EOnlineComparisonOp::Equals);
			this->FindSessionsDelegateHandle = Session->AddOnFindSessionsCompleteDelegate_Handle(
				FOnFindSessionsComplete::FDelegate::CreateUObject(
					this,
					&UEOS_Gameinstance::HandleFindSessionCompleted
					, SessionSearch,MaxResult));
			this->DelegateEOSSessionFound = OnSessionsFound;
			if (!Session->FindSessions(0, SessionSearch))
			{
				// Call didn't start, return error.
				UE_LOG(LogTamEOS, Error, TEXT("Search EOS Session, call didn't start!"));
			}
		}

	}
}

void UEOS_Gameinstance::HandleFindSessionCompleted(const bool bWasSuccessful, const TSharedRef<FOnlineSessionSearch> SessionSearchRef, const int32 MaxResult)
{
	int32 SessionFindNum = 0;
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{
			if (bWasSuccessful)
			{
				TArray<FTamBPSessionSearchResultInfos> SessionSearches;
				
				for (auto RawResult : SessionSearchRef->SearchResults)
				{
					if (RawResult.IsValid()) 
					{
						FTamBPSessionSearchResultInfos TempSessionInfos;
						FString ConnectInfos;
						if (Session->GetResolvedConnectString(RawResult, NAME_GamePort, ConnectInfos))
						{
							if (SessionFindNum > MaxResult)
							{
								break;
							}
							TempSessionInfos.SetSessionSearchInfos(RawResult, ConnectInfos);
							SessionSearches.Add(TempSessionInfos);
							SessionFindNum += 1;
						}
					}
				}
				const TArray<FTamBPSessionSearchResultInfos> SessionResults;
				this->DelegateEOSSessionFound.ExecuteIfBound(true, SessionResults);
				//unregister callbacks
				Session->ClearOnFindSessionsCompleteDelegate_Handle(this->FindSessionsDelegateHandle);
				this->FindSessionsDelegateHandle.Reset();
				return;
			}
		}
	}

	//error and send cllback to bp
	UE_LOG(LogTamEOS, Error, TEXT("Search EOS Session, callback intern error: no valid online subsytem"));
	const TArray<FTamBPSessionSearchResultInfos> EmptyResult;
	DelegateEOSSessionFound.ExecuteIfBound(false, EmptyResult);
	return;
}

void UEOS_Gameinstance::SearchFriendEOSSession(const FDelegateEOSSessionFound & OnSessionsFound, const int32 LocalUserNum, const FUniqueNetIdRepl& FriendUniqueNetId)
	{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{
			this->DelegateEOSFriendSessionFound = OnSessionsFound;
			this->FindFriendSessionsDelegateHandle = Session->AddOnFindFriendSessionCompleteDelegate_Handle(
				LocalUserNum,
				FOnFindFriendSessionComplete::FDelegate::CreateUObject(
					this,
					&UEOS_Gameinstance::HandleFindFriendSessionCompleted));
			if (!FriendUniqueNetId.IsValid())
			{
				UE_LOG(LogTamEOS, Error, TEXT("Search Friends EOS Session, call didn't start!"));
				const TArray<FTamBPSessionSearchResultInfos> EmptyResult;
				this->DelegateEOSFriendSessionFound.ExecuteIfBound(false, EmptyResult);
				return;
			}
			if (!Session->FindFriendSession(LocalUserNum, *FriendUniqueNetId.GetUniqueNetId()))
			{
				UE_LOG(LogTamEOS, Error, TEXT("Search Friends EOS Session, call didn't start!"));
				const TArray<FTamBPSessionSearchResultInfos> EmptyResult;
				this->DelegateEOSFriendSessionFound.ExecuteIfBound(false, EmptyResult);
				return;
			}
		}
	}
}

void UEOS_Gameinstance::HandleFindFriendSessionCompleted(const int32 LocalUserId, const bool bWasSuccessful, const TArray<FOnlineSessionSearchResult>& SearchResults)
{
	TArray<FTamBPSessionSearchResultInfos> BpSearchResults;

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{
			Session->ClearOnFindFriendSessionCompleteDelegate_Handle(LocalUserId, this->FindFriendSessionsDelegateHandle);
			this->FindFriendSessionsDelegateHandle.Reset();

			FString ConnectInfos;
			for (FOnlineSessionSearchResult SearchResultElem : SearchResults)
			{
				if (Session->GetResolvedConnectString(SearchResultElem, NAME_GamePort, ConnectInfos))
				{
					BpSearchResults.Add(FTamBPSessionSearchResultInfos(SearchResultElem, ConnectInfos));
				}
				else
				{
					UE_LOG(LogTamEOS, Error, TEXT("Search Friends EOS Session callback, could not GetResolvedConnectString!"));
				}
			}
		}
	}
	this->DelegateEOSFriendSessionFound.ExecuteIfBound(bWasSuccessful, BpSearchResults);
}

void UEOS_Gameinstance::CancelSearchEOSSession(const FDelegateCancelSearchEOSSession & OnSearchCanceled)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{
			this->DelegateCancelSearchEOSSession = OnSearchCanceled;
			this->CancelFindSessionDelegateHandle = Session->AddOnCancelFindSessionsCompleteDelegate_Handle(
				FOnCancelFindSessionsComplete::FDelegate::CreateUObject(
					this,
					&UEOS_Gameinstance::HandleFindSessionCanceled));
			if(!Session->CancelFindSessions())
			{
				UE_LOG(LogTamEOS, Error, TEXT("Cancel Find EOS Session, call didn't start!"));
				return;
			}
		}
	}
}

void UEOS_Gameinstance::HandleFindSessionCanceled(const bool bWasSuccessfull)
{
	this->DelegateCancelSearchEOSSession.ExecuteIfBound(bWasSuccessfull);
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{
			Session->ClearOnCancelFindSessionsCompleteDelegate_Handle(this->CancelFindSessionDelegateHandle);
			this->CancelFindSessionDelegateHandle.Reset();
			return;
		}
	}
}

void UEOS_Gameinstance::JoinEOSSession(const FDelegateEOSSessionJoined & OnSessionsJoined, const FTamBPSessionSearchResultInfos & SessionSearchInfo)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{
			this->JoinSessionDelegateHandle = Session->AddOnJoinSessionCompleteDelegate_Handle(
				FOnJoinSessionComplete::FDelegate::CreateUObject(this, &UEOS_Gameinstance::HandleJoinSessionCompleted,
					SessionSearchInfo));
			// "KingdomsEdge" is the local name of the session for this player. It doesn't have to match the name the server gave their session.
			this->DelegateEOSSessionJoined = OnSessionsJoined;
			if (!Session->JoinSession(0, FName(TEXT("KingdomsEdge")), *SessionSearchInfo.RawResult))
			{
				// Call didn't start, return error.
				UE_LOG(LogTamEOS, Error, TEXT("Join EOS Session, call didn't start!"));

			}
		}
	}
}

void UEOS_Gameinstance::HandleJoinSessionCompleted(const FName SessionName, EOnJoinSessionCompleteResult::Type JoinResult, const FTamBPSessionSearchResultInfos SessionSearchInfo)
{
	//connecting to server
	if (JoinResult == EOnJoinSessionCompleteResult::Success ||
		JoinResult == EOnJoinSessionCompleteResult::AlreadyInSession)
	{
		if (GEngine != nullptr)
		{
			
			FURL NewURL(nullptr, *SessionSearchInfo.ConnectInfos, ETravelType::TRAVEL_Absolute);
			UE_LOG(LogTamEOS, Warning, TEXT("NEW URL TO TRAVEL TO ON CONNECTION: %s"), *NewURL.ToString());
			FString BrowseError;
			if (GEngine->Browse(GEngine->GetWorldContextFromWorldChecked(this->GetWorld()), NewURL, BrowseError) ==
				EBrowseReturnVal::Failure)
			{
				UE_LOG(LogTamEOS, Error, TEXT("Failed to start browse: %s"), *BrowseError);
			}
		}
	}	

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{
			Session->ClearOnJoinSessionCompleteDelegate_Handle(this->JoinSessionDelegateHandle);
			this->JoinSessionDelegateHandle.Reset();
			FOnlineSessionSettings* OutSessionSettings = Session->GetSessionSettings(SessionName);
			this->DelegateEOSSessionJoined.ExecuteIfBound(SessionName,
				static_cast<ETamBPOnJoinSessionCompleteResult>(JoinResult)
				, OutSessionSettings->BuildUniqueId);
			return;
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("Join EOS Session callback didn't start, no valid online subsystem!"));
	this->DelegateEOSSessionJoined.ExecuteIfBound(TEXT(""), ETamBPOnJoinSessionCompleteResult::UnknownError, 0);
	return;
	
}

void UEOS_Gameinstance::FindEOSSessionFromSessionID(const FDelegateFindEOSSessionBySessionId & OnSessionsFound, const FUniqueNetIdRepl & SearchingUser, const FTamBPOnlineSessionId& SessionId, const FTamBPOnlineFriend & FriendId)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{
			this->DelegateFindEOSSessionBySessionId = OnSessionsFound;
			if (!Session->FindSessionById(
				*SearchingUser.GetUniqueNetId().Get(),
				*SessionId.SessionId.Get(), *FriendId.FriendInfo.Pin()->GetUserId(), FOnSingleSessionResultComplete::FDelegate::CreateUObject(
					this,
					&UEOS_Gameinstance::HandleSingleSessionResultComplete)))
			{
				UE_LOG(LogTamEOS, Error, TEXT("Find EOS Session by ID, call didn't start!"));
				return;
			}
		}
	}
	
}

void UEOS_Gameinstance::HandleSingleSessionResultComplete(int32 LocalUserNum,bool bWasSuccesfull,const FOnlineSessionSearchResult& SearchResult)
{
	DelegateFindEOSSessionBySessionId.ExecuteIfBound(bWasSuccesfull, FTamBPSessionSearchResultInfos(SearchResult));
}

ETamBPOnlineSessionState UEOS_Gameinstance::GetEOSSessionState(const FName SessionName)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session) 
		{
			return static_cast<ETamBPOnlineSessionState>(Session->GetSessionState(SessionName));
		}
	}
	return ETamBPOnlineSessionState::NoSession;
}

void UEOS_Gameinstance::UpdateEOSSession(const FDelegateUpdateEOSSession & OnSessionUpdated, const FName SessionName, const bool bSubmitToEOS)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{
			this->DelegateUpdateEOSSession = OnSessionUpdated;
			this->UpdateSessionDelegateHandle=Session->AddOnUpdateSessionCompleteDelegate_Handle(
				FOnUpdateSessionComplete::FDelegate::CreateUObject(
					this,
					&UEOS_Gameinstance::HandleSessionUpdated));
			if(!Session->UpdateSession(SessionName, *SessionSettings, bSubmitToEOS))
			{
				UE_LOG(LogTamEOS, Error, TEXT("Update EOS Session, call didn't start!"));
				this->DelegateUpdateEOSSession.ExecuteIfBound(false);
				return;
			}
		}
	}
}

void UEOS_Gameinstance::HandleSessionUpdated(const FName SessionName, const bool bWasSuccessfull)
{
	this->DelegateUpdateEOSSession.ExecuteIfBound(bWasSuccessfull);
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{
			Session->ClearOnUpdateSessionCompleteDelegate_Handle(this->UpdateSessionDelegateHandle);
			this->UpdateSessionDelegateHandle.Reset();
		}
	}
}

void UEOS_Gameinstance::StartEOSSession(const FDelegateStartEOSSession & OnSessionStarted, const FName SessionName)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{
			this->DelegateStartEOSSession = OnSessionStarted;
			this->StartSessionDelegateHandle = Session->AddOnStartSessionCompleteDelegate_Handle(
				FOnStartSessionComplete::FDelegate::CreateUObject(
					this,
					&UEOS_Gameinstance::HandleSessionStarted));
			if(!Session->StartSession(SessionName))
			{
				UE_LOG(LogTamEOS, Error, TEXT("Start EOS Session, call didn't start!"));
				this->DelegateStartEOSSession.ExecuteIfBound(false);
				return;
			}
		}
	}
}

void UEOS_Gameinstance::HandleSessionStarted(const FName SessionName, const bool bWasSuccessfull)
{
	this->DelegateStartEOSSession.ExecuteIfBound(bWasSuccessfull);
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{
			Session->ClearOnStartSessionCompleteDelegate_Handle(this->StartSessionDelegateHandle);
			this->StartSessionDelegateHandle.Reset();
		}
	}
}

void UEOS_Gameinstance::GetEOSSessionSettings(const FName SessionName, FTamBPSessionSettings& OutSessionSettings)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineSessionPtr Session = Subsystem->GetSessionInterface();
		if (Session)
		{
			FOnlineSessionSettings* RawSessionSettings = Session->GetSessionSettings(SessionName);

			OutSessionSettings.bAllowInvites = RawSessionSettings->bAllowInvites;
			OutSessionSettings.bAllowJoinInProgress = RawSessionSettings->bAllowJoinInProgress;
			OutSessionSettings.bAllowJoinViaPresence = RawSessionSettings->bAllowJoinViaPresence;
			OutSessionSettings.bAllowJoinViaPresenceFriendsOnly = RawSessionSettings->bAllowJoinViaPresenceFriendsOnly;
			OutSessionSettings.bAntiCheatProtected = RawSessionSettings->bAntiCheatProtected;
			OutSessionSettings.bIsDedicated = RawSessionSettings->bIsDedicated;
			OutSessionSettings.bIsLANMatch = RawSessionSettings->bIsLANMatch;
			OutSessionSettings.bShouldAdvertise = RawSessionSettings->bShouldAdvertise;
			OutSessionSettings.BuildUniqueId = RawSessionSettings->BuildUniqueId;
			OutSessionSettings.bUseLobbiesIfAvailable = RawSessionSettings->bUseLobbiesIfAvailable;
			OutSessionSettings.bUseLobbiesVoiceChatIfAvailable = RawSessionSettings->bUseLobbiesVoiceChatIfAvailable;
			OutSessionSettings.bUsesPresence = RawSessionSettings->bUsesPresence;
			OutSessionSettings.bUsesStats = RawSessionSettings->bUsesStats;
			OutSessionSettings.NumPrivateConnections = RawSessionSettings->NumPrivateConnections;
			OutSessionSettings.NumPublicConnections = RawSessionSettings->NumPublicConnections;
		}
	}
	return;
}



void UEOS_Gameinstance::ReadEOSFriendList(const FDelegateReadEOSFriendList & OnReadCompleted, const int32 LocalUserNum)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();
		if (Friends)
		{
			this->DelegateReadEOSFriendList = OnReadCompleted;
			if (Friends->ReadFriendsList(LocalUserNum,
				TEXT("") /* ListName, unused by EOS */,
				FOnReadFriendsListComplete::CreateUObject(this, &UEOS_Gameinstance::OnReadFriendsComplete))) 
			{
				return;
			};
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("Read EOS Friend List, call didn't start!"));
	return;
}

void UEOS_Gameinstance::OnReadFriendsComplete(int32 LocalUserNum, bool bWasSuccessful, const FString& ListName, const FString& ErrorStr)
{
	this->DelegateReadEOSFriendList.ExecuteIfBound(bWasSuccessful, ErrorStr);
	// Check bWasSuccessful; if it's true, you can now call GetFriendsList.
}

void UEOS_Gameinstance::GetEOSFriendList(const int32 LocalUserNum, bool& bWasSuccessfull, TArray<FTamBPOnlineFriend>& OutFriends)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();
		if (Friends)
		{
			TArray<TSharedRef<FOnlineFriend>> FriendsArr;
			if (Friends->GetFriendsList(LocalUserNum, TEXT(""), FriendsArr)) {
				bWasSuccessfull = true;
				for (const auto & Friend : FriendsArr)
				{
					FTamBPOnlineFriend TempFriend(Friend);
					OutFriends.Add(Friend);
				}
				return;
			}
		}
	}
	bWasSuccessfull = false;
	UE_LOG(LogTamEOS, Error, TEXT("Get EOS Friend List, call didn't start!"));
	return;
}

void UEOS_Gameinstance::GetEOSFriend(const int32 LocalUserNum, const FUniqueNetIdRepl& FriendUserID, bool& bWasSuccessfull, FTamBPOnlineFriend& Friend)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineFriendsPtr OnlineFriend = Subsystem->GetFriendsInterface();
		if (OnlineFriend)
		{
			Friend = FTamBPOnlineFriend(OnlineFriend->GetFriend(LocalUserNum, *FriendUserID.GetUniqueNetId(), TEXT("")));
			bWasSuccessfull = Friend.FriendInfo.IsValid();

		}
	}
}

void UEOS_Gameinstance::ListenToFriendsChanges(const FDelegateFriendListChanged & OnFriendListChanged, int32 LocalUserNum)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineFriendsPtr OnlineFriend = Subsystem->GetFriendsInterface();
		if (OnlineFriend)
		{
			this->DelegateFriendListChanged = OnFriendListChanged;
			OnFriendListChangedDelegateHandle = OnlineFriend->AddOnFriendsChangeDelegate_Handle(
				0,
				FOnFriendsChangeDelegate::CreateUObject(
					this,
					&UEOS_Gameinstance::HandleOnFriendsListChanged,
					LocalUserNum));
			return;
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("Add delegate to listen EOS Friend List Changed, call didn't start!"));
	return;
}

void UEOS_Gameinstance::HandleOnFriendsListChanged(int32 LocalUserNum)
{
	this->DelegateFriendListChanged.ExecuteIfBound(LocalUserNum);
	return;
}

void UEOS_Gameinstance::GetEOSFriendUniqueNetId(const FTamBPOnlineFriend & Friend, FUniqueNetIdRepl& UniqueNetID)
{
	UniqueNetID = FUniqueNetIdRepl(Friend.FriendInfo.Pin()->GetUserId());
}

void UEOS_Gameinstance::QuerryEOSPresence(const FDelegateOnPresenceTaskComplete & OnPresenceTaskComplete, const int32 LocalUserNum, const FTamBPOnlineFriend & Friend)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
		if (Identity)
		{
			IOnlinePresencePtr Presence = Subsystem->GetPresenceInterface();
			if (Presence)
			{
				this->DelegateOnPresenceTaskComplete = OnPresenceTaskComplete;
				TArray<TSharedRef<const FUniqueNetId>> UserIDs;
				UserIDs.Init(Friend.FriendInfo.Pin()->GetUserId(), 1);

				Presence->QueryPresence(
					*Identity->GetUniquePlayerId(LocalUserNum),
					UserIDs, IOnlinePresence::FOnPresenceTaskCompleteDelegate::CreateUObject(this,
						&UEOS_Gameinstance::HandleOnPresenceTaskCompleteDelegate));
				return;
			}
		}	
	}
	UE_LOG(LogTamEOS, Error, TEXT("Query EOS Presnece, call didn't start!"));
	return;
}

void UEOS_Gameinstance::HandleOnPresenceTaskCompleteDelegate(const FUniqueNetId& UserId, const bool bWasSuccessfull)
{
	this->DelegateOnPresenceTaskComplete.ExecuteIfBound(bWasSuccessfull, FUniqueNetIdRepl(UserId));
	return;
}

void UEOS_Gameinstance::GetEOSFriendPresence(const FTamBPOnlineFriend & Friend, FTamBPOnlinePresenceData & PresenceInfo)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	if (Subsystem)
	{
		IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();
		if (Friends)
		{
			PresenceInfo = FTamBPOnlinePresenceData(Friend.FriendInfo.Pin()->GetPresence());
			return;
		}
	}
	UE_LOG(LogTamEOS, Error, TEXT("Get EOS Friend Presence, call didn't start!"));
	return;
}

void UEOS_Gameinstance::EnumerateEOSUserFile(const FDelegateEnumerateEOSUserFile & OnEnumerateComplete, const FUniqueNetIdRepl & UserID)
{
	IOnlineSubsystem* OnlineSubsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineUserCloudPtr UserCloud = OnlineSubsystem->GetUserCloudInterface();

	this->EnumerateUserFilesDelegateHandle =
		UserCloud->AddOnEnumerateUserFilesCompleteDelegate_Handle(FOnEnumerateUserFilesComplete::FDelegate::CreateUObject(
			this, &UEOS_Gameinstance::HandleEnumerateUserFileComplete));
	this->DelegateEnumerateEOSUserFile = OnEnumerateComplete;
	UserCloud->EnumerateUserFiles(*UserID.GetUniqueNetId().Get());


}

void UEOS_Gameinstance::HandleEnumerateUserFileComplete(bool bWasSuccessful,
	const FUniqueNetId & UserId)
{
	//clear the delegate
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();
	UserCloud->ClearOnEnumerateUserFilesCompleteDelegate_Handle(this->EnumerateUserFilesDelegateHandle);
	this->EnumerateUserFilesDelegateHandle.Reset();

	if (!bWasSuccessful)
	{
		UE_LOG(LogTamEOS, Error, TEXT("HandleEnumerateUserFileComplete failed!"));
	}
	this->DelegateEnumerateEOSUserFile.ExecuteIfBound(bWasSuccessful);
	return;
}

void UEOS_Gameinstance::GetEOSUserFileList(const FUniqueNetIdRepl& LocalUserId, TArray<FString>& PlayerFileNames)
{
	TArray<FCloudFileHeader> Files;

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();

	UserCloud->GetUserFileList(*LocalUserId.GetUniqueNetId().Get(), Files);
	TArray<FString> FileNames;
	FileNames.Reserve(Files.Num() - 1);
	for (const auto& It : Files)
	{
		FileNames.Add(It.FileName);
	}
	PlayerFileNames = FileNames;
	return;
}

void UEOS_Gameinstance::ReadEOSUserFileAsSavegameObject(const FDelegateEOSFileReadAsSaveGame & OnFileRead,const FUniqueNetIdRepl & LocalUserId,const FString PlayerFileName)
{

	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();

	this->ReadUserFileDelegateHandle =
	UserCloud->AddOnReadUserFileCompleteDelegate_Handle(FOnReadUserFileComplete::FDelegate::CreateUObject(
		this,
		&UEOS_Gameinstance::HandleReadUserFileComplete));

	this->DelegateEOSFileReadAsSaveGame = OnFileRead;

	if (!UserCloud->ReadUserFile(*LocalUserId.GetUniqueNetId().Get(),PlayerFileName))
	{
		UE_LOG(LogTamEOS, Error, TEXT("ReadEOSFileAsSavegame didn't start!"));
	}
	return;
}

void UEOS_Gameinstance::HandleReadUserFileComplete(bool bWasSuccessful,
	const FUniqueNetId& UserId,
	const FString& FileName) 
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();

	UserCloud->ClearOnReadUserFileCompleteDelegate_Handle(this->ReadUserFileDelegateHandle);
	this->ReadUserFileDelegateHandle.Reset();
	if (bWasSuccessful)
	{
		TArray<uint8> FileContents;
		if (UserCloud->GetFileContents(UserId, FileName, FileContents))
		{
			USaveGame* Data = UGameplayStatics::LoadGameFromMemory(FileContents);

			this->DelegateEOSFileReadAsSaveGame.ExecuteIfBound(true, Data);
				return;
		}
	}
	this->DelegateEOSFileReadAsSaveGame.ExecuteIfBound(false, nullptr);
}

void UEOS_Gameinstance::WriteSavegameObjectAsEOSUserFile(const FDelegateEOSUserFileWriten& OnFileWrote, const FUniqueNetIdRepl& LocalUserId, const FString PlayerFileName, USaveGame* SaveData, const bool CompressBeforeUpload)
{
	if (!(PlayerFileName == ""))
	{
		IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
		IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();

		this->WriteUserDataDelegateHandle = UserCloud->AddOnWriteUserFileCompleteDelegate_Handle(
			FOnWriteUserFileComplete::FDelegate::CreateUObject(this, &UEOS_Gameinstance::HandleWriteUserFileComplete));

		this->DelegateEOSUserFileWriten = OnFileWrote;

		//populate the bytes to uplaod with a save file
		TArray<uint8> FileData;
		UGameplayStatics::SaveGameToMemory(SaveData, FileData);

		if (!UserCloud->WriteUserFile(*LocalUserId.GetUniqueNetId().Get(), PlayerFileName, FileData, CompressBeforeUpload))
		{
			UE_LOG(LogTamEOS, Error, TEXT("WriteSaveGameAsEOSUserFile didn't start!"));
		}
		return;
	}
	UE_LOG(LogTamEOS, Error, TEXT("WriteSaveGameAsEOSUserFile invalid file name!"));
}

void UEOS_Gameinstance::HandleWriteUserFileComplete(bool bWasSuccessful,
	const FUniqueNetId &UserId,
	const FString& FileName)
{
	IOnlineSubsystem* Subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineUserCloudPtr UserCloud = Subsystem->GetUserCloudInterface();

	//Unregister the handler
	UserCloud->ClearOnWriteUserFileCompleteDelegate_Handle(this->WriteUserDataDelegateHandle);
	this->WriteUserDataDelegateHandle.Reset();

	this->DelegateEOSUserFileWriten.ExecuteIfBound(bWasSuccessful);
}