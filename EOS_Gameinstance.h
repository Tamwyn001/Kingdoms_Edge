// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "Kingdoms_Edge.h"
#include "UObject/Class.h"
#include "Engine/GameInstance.h"
#include "OnlineSessionSettings.h"
#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"
#include "OnlineSubsystemTypes.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
#include "E:/UE_5.1/Engine/Plugins/Marketplace/EOSOnlineSubsystem/Source/RedpointEOSInterfaces/Private/Interfaces/OnlineLobbyInterface.h"
#include "Templates/SharedPointer.h"
#include "Delegates/Delegate.h"
#include "Interfaces/OnlinePartyInterface.h"

#include "EOS_Gameinstance.generated.h"

/**
 * 
 */

typedef TSharedPtr<IOnlineLobby, ESPMode::ThreadSafe> IOnlineLobbyPtr;


//copyright 

USTRUCT(BlueprintType)
struct FTamBPSessionSettings
{
	GENERATED_USTRUCT_BODY()

public:
	
	/** The number of publicly available connections advertised */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 NumPublicConnections;
	/** The number of connections that are private (invite/password) only */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int32 NumPrivateConnections;
	/** Whether this match is publicly advertised on the online service */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShouldAdvertise;
	/** Whether joining in progress is allowed or not */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAllowJoinInProgress;
	/** This game will be lan only and not be visible to external players */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsLANMatch;
	/** Whether the server is dedicated or player hosted */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsDedicated;
	/** Whether the match should gather stats or not */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bUsesStats;
	/** Whether the match allows invitations for this session or not */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAllowInvites;
	/** Whether to display user presence information or not */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bUsesPresence;
	/** Whether joining via player presence is allowed or not */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAllowJoinViaPresence;
	/** Whether joining via player presence is allowed for friends only or not */
	UPROPERTY(BlueprintReadWrite,EditAnywhere)
	bool bAllowJoinViaPresenceFriendsOnly;
	/** Whether the server employs anti-cheat (punkbuster, vac, etc) */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bAntiCheatProtected;
	/** Whether to prefer lobbies APIs if the platform supports them */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bUseLobbiesIfAvailable;
	/** Whether to create (and auto join) a voice chat room for the lobby, if the platform supports it */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bUseLobbiesVoiceChatIfAvailable;

	/** Manual override for the Session Id instead of having one assigned by the backend. Its size may be restricted depending on the platform */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString SessionIdOverride;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	/** Used to keep different builds from seeing each other during searches */
	int32 BuildUniqueId;
	/** Array of custom session settings */
	FSessionSettings Settings;

	/** Map of custom settings per session member (Not currently used by every OSS) */
	TUniqueNetIdMap<FSessionSettings> MemberSettings;


	FTamBPSessionSettings()
	{

	}

};

/**
*FTamBPUserOnlineAccount is used to represent local signed in user. Poor Blueprint use
*/
USTRUCT(BlueprintType)
struct FTamBPUserOnlineAccount
{
	GENERATED_USTRUCT_BODY()

public:
	TSharedPtr<FUserOnlineAccount> UserAccountInfo;


	FTamBPUserOnlineAccount()
	{

	}

	FTamBPUserOnlineAccount(TSharedPtr<FUserOnlineAccount> UserAccount)
	{
		UserAccountInfo = UserAccount;
	}
};

USTRUCT(BlueprintType)
struct FTamBPOnlinePartyId
{
	GENERATED_USTRUCT_BODY()

private:
	bool bUseDirectPointer;


public:
	TSharedPtr<const FOnlinePartyId> OnlinePartyId;
	const FOnlinePartyId* OnlinePartyIdPtr;
	FTamBPOnlinePartyId()
	{
		bUseDirectPointer = false;
		OnlinePartyIdPtr = nullptr;
	}

	FTamBPOnlinePartyId(const TSharedRef<const FOnlinePartyId>& ID)
	{
		SetOnlinePartyId(ID);
	}
	FTamBPOnlinePartyId(const TSharedPtr<const FOnlinePartyId>& ID)
	{
		SetOnlinePartyId(ID);
	}
	FTamBPOnlinePartyId(const FOnlinePartyId* ID)
	{
		SetOnlinePartyId(ID);
	}

	void SetOnlinePartyId(const TSharedPtr<const FOnlinePartyId>& ID)
	{
		bUseDirectPointer = false;
		OnlinePartyIdPtr = nullptr;
		OnlinePartyId = ID;
	}

	void SetOnlinePartyId(const TSharedRef<const FOnlinePartyId>& ID)
	{
		bUseDirectPointer = false;
		OnlinePartyIdPtr = nullptr;
		OnlinePartyId = ID;
	}

	void SetOnlinePartyId(const FOnlinePartyId* ID)
	{
		bUseDirectPointer = true;
		OnlinePartyIdPtr = ID;
	}

	bool IsValid() const
	{
		if (bUseDirectPointer && OnlinePartyIdPtr != nullptr && OnlinePartyIdPtr->IsValid())
		{
			return true;
		}
		else if (OnlinePartyId.IsValid())
		{
			return true;
		}
		else
			return false;

	}

	const FOnlinePartyId* GetOnlinePartyId() const
	{
		if (bUseDirectPointer && OnlinePartyIdPtr != nullptr)
		{
			// No longer converting to non const as all functions now pass const OnlinePartyIds
			return /*const_cast<FOnlinePartyId*>*/(OnlinePartyIdPtr);
		}
		else if (OnlinePartyId.IsValid())
		{
			return OnlinePartyId.Get();
		}
		else
			return nullptr;
	}

	// Adding in a compare operator so that std functions will work with this struct
	FORCEINLINE bool operator==(const FTamBPOnlinePartyId& Other) const
	{
		return (IsValid() && Other.IsValid() && (*GetOnlinePartyId() == *Other.GetOnlinePartyId()));
	}

	FORCEINLINE bool operator!=(const FTamBPOnlinePartyId& Other) const
	{
		return !(IsValid() && Other.IsValid() && (*GetOnlinePartyId() == *Other.GetOnlinePartyId()));
	}


};

USTRUCT(BlueprintType)
struct FTamBPOnlinePartyMember
{
	GENERATED_USTRUCT_BODY()

private:
	bool bUseDirectPointer;


public:
	TSharedPtr<const FOnlinePartyMember> PartyMember;
	const FOnlinePartyMember* PartyMemberPtr;

	void SetPartyMember(const TSharedPtr<const FOnlinePartyMember>& ID)
	{
		bUseDirectPointer = false;
		PartyMemberPtr = nullptr;
		PartyMember = ID;
	}

	void SetPartyMember(const FOnlinePartyMember* ID)
	{
		bUseDirectPointer = true;
		PartyMemberPtr = ID;
	}

	bool IsValid() const
	{
		if (bUseDirectPointer && PartyMemberPtr != nullptr && PartyMemberPtr->GetUserId()->IsValid())
		{
			return true;
		}
		else if (PartyMember.IsValid())
		{
			return true;
		}
		else
			return false;

	}

	const FOnlinePartyMember* GetPartyMember() const
	{
		if (bUseDirectPointer && PartyMemberPtr != nullptr)
		{
			// No longer converting to non const as all functions now pass const PartyMembers
			return /*const_cast<FOnlinePartyMember*>*/(PartyMemberPtr);
		}
		else if (PartyMember.IsValid())
		{
			return PartyMember.Get();
		}
		else
			return nullptr;
	}

	// Adding in a compare operator so that std functions will work with this struct
	/*FORCEINLINE bool operator==(const FTamOnlinePartyMember& Other) const
	{
		return (IsValid() && Other.IsValid() && (*GetPartyMember() == *Other.GetPartyMember()));
	}

	FORCEINLINE bool operator!=(const FTamOnlinePartyMember& Other) const
	{
		return !(IsValid() && Other.IsValid() && (*GetPartyMember() == *Other.GetPartyMember()));
	}*/

	FTamBPOnlinePartyMember()
	{
		bUseDirectPointer = false;
		PartyMemberPtr = nullptr;
	}
};

USTRUCT(BlueprintType)
struct FTamBPOnlinePartyReference
{
	GENERATED_USTRUCT_BODY()

private:
	TWeakPtr<const FOnlineParty> WeakPartyPtr;

public:
	FTamBPOnlinePartyReference() 
	{
	
	}
	TSharedPtr<const FOnlineParty> GetParty() const
	{
		return WeakPartyPtr.Pin();
	}
	void SetParty(const TSharedRef<const FOnlineParty>& InParty)
	{
		WeakPartyPtr = InParty;
	}
	bool IsValid() const
	{
		if (WeakPartyPtr.IsValid())
		{
			return true;
		}
		else
			return false;
	}
	
	// Adding in a compare operator so that std functions will work with this struct
	/*FORCEINLINE bool operator==(const FTamOnlinePartyReference& Other) const		
	{
		return (IsValid() && Other.IsValid() && (*GetParty() == *Other.GetParty()));
	}

	FORCEINLINE bool operator!=(const FTamOnlinePartyReference& Other) const
	{
		return !(IsValid() && Other.IsValid() && (*GetParty() == *Other.GetParty()));
	}*/
};

USTRUCT(BlueprintType)
struct FTamBPOnlinePartyJoinInfo
{
	GENERATED_USTRUCT_BODY()

private:
	
public:
	
	 TWeakPtr<const IOnlinePartyJoinInfo> PartyJoinInfo;
	 UPROPERTY(BlueprintReadWrite, EditAnywhere)
		 FUniqueNetIdRepl SenderId = FUniqueNetIdRepl();

	FTamBPOnlinePartyJoinInfo()
	{

	}

	//auto generated join info from get friend party join ingo 
	FTamBPOnlinePartyJoinInfo(IOnlinePartyJoinInfoConstPtr InvitationToParty)
	{
		PartyJoinInfo = InvitationToParty->AsWeak();
	}

	FTamBPOnlinePartyJoinInfo(IOnlinePartyJoinInfoConstRef InvitationToParty)
	{
		PartyJoinInfo = InvitationToParty->AsWeak();
		SenderId = FUniqueNetIdRepl(InvitationToParty->GetSourceUserId());
		UE_LOG(LogTamEOS, Warning, TEXT("Incomming Invite from: %s"), *InvitationToParty->GetSourceUserId()->ToString());
	}

	FTamBPOnlinePartyJoinInfo(const IOnlinePartyJoinInfo& InvitationToParty)
	{
		PartyJoinInfo = InvitationToParty.AsWeak();
		SenderId = FUniqueNetIdRepl(InvitationToParty.GetSourceUserId());
		UE_LOG(LogTamEOS, Warning, TEXT("Incomming Invite from: %s"), *InvitationToParty.GetSourceUserId()->ToString());
	}
	bool IsValid() const
	{
		return PartyJoinInfo.Pin().IsValid();
	}
	const IOnlinePartyJoinInfo& GetParty() const
	{
		return *PartyJoinInfo.Pin().Get();
	}
};
USTRUCT(BlueprintType)
struct FTamBPOnlinePartyPendingJoinRequestInfoConstRef
{
	GENERATED_USTRUCT_BODY()

private:
	TWeakPtr<const IOnlinePartyPendingJoinRequestInfo> WeakPartyPendingJoinRequestInfoPtr;

public:
	FTamBPOnlinePartyPendingJoinRequestInfoConstRef()
	{

	}
	TSharedPtr<const IOnlinePartyPendingJoinRequestInfo> GetPendingJoinRequest() const
	{
		return WeakPartyPendingJoinRequestInfoPtr.Pin();
	}
	void SetPendingJoinRequest(const TSharedPtr<const IOnlinePartyPendingJoinRequestInfo>& InPartyPendingJoinRequest)
	{
		WeakPartyPendingJoinRequestInfoPtr = InPartyPendingJoinRequest;
	}
	bool IsValid() const
	{
		if (WeakPartyPendingJoinRequestInfoPtr.IsValid())
		{
			return true;
		}
		else
			return false;
	}
};

UENUM(BlueprintType)
enum class ETamBPVariantDataType : uint8
{
	Boolean = 0 	UMETA(DisplayName = "Boolean"),
	Int32 	UMETA(DisplayName = "Integer"),
	Float 	UMETA(DisplayName = "Float"),
	FString 	UMETA(DisplayName = "String")
};

USTRUCT(BlueprintType)
struct FTamMetadataQuery
{
	GENERATED_USTRUCT_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		ETamBPVariantDataType Type;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Key;
	FTamMetadataQuery(){}
	FTamMetadataQuery(const ETamBPVariantDataType& InType, const FString InKey)
	{
		Type = InType;
		Key = InKey;
	}
};

USTRUCT(BlueprintType)
struct 	FTamBPVariantData {
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		ETamBPVariantDataType Type;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool AsBool;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		int AsInt;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		float AsFloat;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString AsString;
	FTamBPVariantData(){}
	FTamBPVariantData(const bool InBool) 
	{
		SetTypeBool(InBool);
	}
	FTamBPVariantData(const int InInt)
	{
		SetTypeInt(InInt);
	}
	FTamBPVariantData(const float InFloat)
	{
		SetTypeFloat(InFloat);
	}
	FTamBPVariantData(const FString InString)
	{
		SetTypeFString(InString);
	}

	void SetTypeBool(const bool InBool)
	{
		AsBool = InBool;
		Type = ETamBPVariantDataType::Boolean;
	}
	void SetTypeInt(const int32 InInt)
	{
		AsInt = InInt;
		Type = ETamBPVariantDataType::Int32;
	}
	void SetTypeFloat(const float InFloat)
	{
		AsFloat = InFloat;
		Type = ETamBPVariantDataType::Float;
	}
	void SetTypeFString(const FString InString)
	{
		AsString = InString;
		Type = ETamBPVariantDataType::FString;
	}
};

USTRUCT(BlueprintType)
struct FTamBPPartyMetadata {
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString AttributeName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FTamBPVariantData AttributeValue;

	FTamBPPartyMetadata() {};
	FTamBPPartyMetadata(FString InAttributeName, const FTamBPVariantData& InAttributeValue)
	{
		AttributeName = InAttributeName;
		AttributeValue = InAttributeValue;
	}
	FVariantData GetAttributeValue()
	{
		switch (AttributeValue.Type)
		{
		default:
			break;
		case ETamBPVariantDataType::Boolean:
			return FVariantData(AttributeValue.AsBool);
				break;
		case ETamBPVariantDataType::Int32:
			return FVariantData(AttributeValue.AsInt);
				break;
		case ETamBPVariantDataType::Float:
			return FVariantData(AttributeValue.AsFloat);
				break;
		case ETamBPVariantDataType::FString:
			return FVariantData(AttributeValue.AsString);
				break;
		}
		return FVariantData();
	}
};

USTRUCT(BlueprintType)
struct 	FTamBPSessionSearchResultInfos{
	GENERATED_USTRUCT_BODY()

public:
	TSharedPtr<FOnlineSessionSearchResult> RawResult;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString ConnectInfos;
	FTamBPSessionSearchResultInfos()
	{
		RawResult = nullptr;
	}
	FTamBPSessionSearchResultInfos(FOnlineSessionSearchResult NewRawResult)
	{
		RawResult = MakeShared<FOnlineSessionSearchResult>(NewRawResult);
	}
	FTamBPSessionSearchResultInfos(FOnlineSessionSearchResult NewRawResult,FString NewConnectInfos)
	{
		RawResult = MakeShared<FOnlineSessionSearchResult>(NewRawResult);
		ConnectInfos = NewConnectInfos;
	}
	void SetSessionSearchInfos(const FOnlineSessionSearchResult& NewRawResult, FString NewConnectInfos) 
	{
		RawResult = MakeShared<FOnlineSessionSearchResult>(NewRawResult);
		ConnectInfos = NewConnectInfos;
	}
	bool IsValid() const
	{
		if (RawResult.IsValid())
		{
			return true;
		}
		else
			return false;
	}
};
//expose some enum to bp

USTRUCT(BlueprintType)
struct 	FTamBPOnlineFriend {
	GENERATED_USTRUCT_BODY()

public: 
	TWeakPtr<FOnlineFriend> FriendInfo;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FString Nickname;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FUniqueNetIdRepl UniqueNetId;
	FTamBPOnlineFriend()
	{
		Nickname = TEXT("");
		UniqueNetId = FUniqueNetIdRepl();
	}

	FTamBPOnlineFriend(TSharedRef<FOnlineFriend> FriendSharedRef)
	{
		FriendInfo = FriendSharedRef;
		Nickname = FriendInfo.Pin()->GetDisplayName();
		UniqueNetId = FriendInfo.Pin()->GetUserId();
	}
	FTamBPOnlineFriend(TSharedPtr<FOnlineFriend> FriendSharedRef)
	{
		FriendInfo = FriendSharedRef.ToSharedRef();
		Nickname = FriendInfo.Pin()->GetDisplayName();
		UniqueNetId = FriendInfo.Pin()->GetUserId();
	}
	bool IsValid() const
	{
		return FriendInfo.IsValid();
	}
};

UENUM(BlueprintType)
enum class ETamBPOnlinePresenceState : uint8
{
	Online = static_cast<uint8>(EOnlinePresenceState::Online),
	Offline = static_cast<uint8>(EOnlinePresenceState::Offline),
	Away = static_cast<uint8>(EOnlinePresenceState::Away),
	ExtendedAway = static_cast<uint8>(EOnlinePresenceState::ExtendedAway),
	DoNotDisturb = static_cast<uint8>(EOnlinePresenceState::DoNotDisturb),
	Chat = static_cast<uint8>(EOnlinePresenceState::Chat)
};

USTRUCT(BlueprintType)
struct FTamBPUserInfos {
	GENERATED_USTRUCT_BODY()
public:
	TSharedPtr<FOnlineUser> OnlineUser;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString PlayerNickname;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FUniqueNetIdRepl UniqueNetId;

	FTamBPUserInfos()
	{

	}
	FTamBPUserInfos(TSharedPtr<FOnlineUser> UserPtr)
	{
		if(UserPtr.IsValid())
		{
			PlayerNickname = UserPtr.Get()->GetDisplayName();
			UniqueNetId = FUniqueNetIdRepl(UserPtr.Get()->GetUserId().Get());
		}
	}

};

USTRUCT(BlueprintType)
struct 	FTamBPOnlineUserPresenceStatus {
	GENERATED_USTRUCT_BODY()

public:
	FOnlineUserPresenceStatus PresenceStatus;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		ETamBPOnlinePresenceState UserPresenceState;
	FTamBPOnlineUserPresenceStatus()
	{
		PresenceStatus.State = EOnlinePresenceState::Offline;
		UserPresenceState = ETamBPOnlinePresenceState::Offline;
	}
	FTamBPOnlineUserPresenceStatus(FOnlineUserPresenceStatus UserPresenceStatus)
	{
		PresenceStatus = UserPresenceStatus;
		UserPresenceState = static_cast<ETamBPOnlinePresenceState>(PresenceStatus.State);
		
	}

};

USTRUCT(BlueprintType)
struct 	FTamBPOnlineSessionId {
	GENERATED_USTRUCT_BODY()

public:
	FUniqueNetIdPtr SessionId;
	FTamBPOnlineSessionId()
	{
		SessionId = nullptr;
	}
	FTamBPOnlineSessionId(FUniqueNetIdPtr OnlineSessionId)
	{
		SessionId = OnlineSessionId;
	}
};

USTRUCT(BlueprintType)
struct 	FTamBPOnlinePresenceData {
	GENERATED_USTRUCT_BODY()

public:
	FOnlineUserPresence UserPresence; //for cpp

	UPROPERTY(BlueprintReadWrite, EditAnywhere)//for bp
		FTamBPOnlineSessionId SessionId;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bIsOnline;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bIsPlaying;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bIsPlayingThisGame;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bIsJoinable;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		bool bHasVoiceSupport;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FTamBPOnlineUserPresenceStatus PresenceStatus;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FDateTime LastOnline;

	FTamBPOnlinePresenceData()
	{
		UserPresence = FOnlineUserPresence();

		SessionId = FTamBPOnlineSessionId();
		bIsOnline = false;
		bIsPlaying = false;
		bIsPlayingThisGame = false;
		bIsJoinable = false;
		bHasVoiceSupport = false;
		PresenceStatus = FTamBPOnlineUserPresenceStatus();
		LastOnline = FDateTime();
	}

	FTamBPOnlinePresenceData(FOnlineUserPresence OnlineUserPresence)
	{
		UserPresence = OnlineUserPresence;

		SessionId = FTamBPOnlineSessionId(OnlineUserPresence.SessionId);
		bIsOnline = OnlineUserPresence.bIsOnline;
		bIsPlaying = OnlineUserPresence.bIsOnline;
		bIsPlayingThisGame = OnlineUserPresence.bIsPlayingThisGame;
		bIsJoinable = OnlineUserPresence.bIsJoinable;
		bHasVoiceSupport = OnlineUserPresence.bHasVoiceSupport;
		PresenceStatus = FTamBPOnlineUserPresenceStatus(OnlineUserPresence.Status);
		LastOnline = OnlineUserPresence.LastOnline;
	}
	

};

UENUM(BlueprintType)
enum class ETamBPCreatePartyCompletionResult : uint8
{
	UnknownClientFailure = static_cast<uint8>(ECreatePartyCompletionResult::UnknownClientFailure) UMETA(DisplayName = "Unknown client failure"),
	AlreadyInPartyOfSpecifiedType = static_cast<uint8>(ECreatePartyCompletionResult::AlreadyInPartyOfSpecifiedType) UMETA(DisplayName = "Already in party of specified type"),
	AlreadyCreatingParty = static_cast<uint8>(ECreatePartyCompletionResult::AlreadyCreatingParty) UMETA(DisplayName = "Already creating party"),
	AlreadyInParty = static_cast<uint8>(ECreatePartyCompletionResult::AlreadyInParty) UMETA(DisplayName = "Already in party"),
	FailedToCreateMucRoom = static_cast<uint8>(ECreatePartyCompletionResult::FailedToCreateMucRoom) UMETA(DisplayName = "Failed to create MUC room"),
	NoResponse = static_cast<uint8>(ECreatePartyCompletionResult::NoResponse) UMETA(DisplayName = "No response"),
	LoggedOut = static_cast<uint8>(ECreatePartyCompletionResult::LoggedOut) UMETA(DisplayName = "Logged out"),
	NotPrimaryUser = static_cast<uint8>(ECreatePartyCompletionResult::NotPrimaryUser) UMETA(DisplayName = "Not primary user"),
	UnknownInternalFailure = static_cast<uint8>(ECreatePartyCompletionResult::UnknownInternalFailure) UMETA(DisplayName = "Unknown internal failure"),
	Succeeded = static_cast<uint8>(ECreatePartyCompletionResult::Succeeded) UMETA(DisplayName = "Succeeded")
};

UENUM(BlueprintType)
enum class ETamBPKickMemberCompletionResult : uint8
{
	UnknownClientFailure = static_cast<uint8>(EKickMemberCompletionResult::UnknownClientFailure) UMETA(DisplayName = "Unknown Client Failure"),
	UnknownParty = static_cast<uint8>(EKickMemberCompletionResult::UnknownParty )UMETA(DisplayName = "Unknown Party"),
	LocalMemberNotMember = static_cast<uint8>(EKickMemberCompletionResult::LocalMemberNotMember )UMETA(DisplayName = "Local Member Not Member"),
	LocalMemberNotLeader = static_cast<uint8>(EKickMemberCompletionResult::LocalMemberNotLeader )UMETA(DisplayName = "Local Member Not Leader"),
	RemoteMemberNotMember = static_cast<uint8>(EKickMemberCompletionResult::RemoteMemberNotMember )UMETA(DisplayName = "Remote Member Not Member"),
	MessagingFailure = static_cast<uint8>(EKickMemberCompletionResult::MessagingFailure )UMETA(DisplayName = "Messaging Failure"),
	NoResponse = static_cast<uint8>(EKickMemberCompletionResult::NoResponse) UMETA(DisplayName = "No Response"),
	LoggedOut = static_cast<uint8>(EKickMemberCompletionResult::LoggedOut )UMETA(DisplayName = "Logged Out"),
	UnknownInternalFailure = static_cast<uint8>(EKickMemberCompletionResult::UnknownInternalFailure )UMETA(DisplayName = "Unknown Internal Failure"),
	Succeeded = static_cast<uint8>(EKickMemberCompletionResult::Succeeded) UMETA(DisplayName = "Succeeded")
};

UENUM(BlueprintType)
enum class ETamBPPromoteMemberCompletionResult : uint8
{
	UnknownClientFailure= static_cast<uint8>(EPromoteMemberCompletionResult::UnknownClientFailure) UMETA(DisplayName = "Unknown Client Failure"),
	UnknownServiceFailure  = static_cast<uint8>(EPromoteMemberCompletionResult::UnknownServiceFailure)UMETA(DisplayName = "Unknown Service Failure"),
	UnknownParty  = static_cast<uint8>(EPromoteMemberCompletionResult::UnknownParty) UMETA(DisplayName = "Unknown Party"),
	LocalMemberNotMember  = static_cast<uint8>(EPromoteMemberCompletionResult::LocalMemberNotMember)UMETA(DisplayName = "Local Member Not Member"),
	LocalMemberNotLeader = static_cast<uint8>(EPromoteMemberCompletionResult::LocalMemberNotLeader)UMETA(DisplayName = "Local Member Not Leader"),
	PromotionAlreadyPending  = static_cast<uint8>(EPromoteMemberCompletionResult::PromotionAlreadyPending) UMETA(DisplayName = "Promotion Already Pending"),
	TargetIsSelf  = static_cast<uint8>(EPromoteMemberCompletionResult::TargetIsSelf)UMETA(DisplayName = "Target Is Self"),
	TargetNotMember  = static_cast<uint8>(EPromoteMemberCompletionResult::TargetNotMember)UMETA(DisplayName = "Target Not Member"),
	MessagingFailure  = static_cast<uint8>(EPromoteMemberCompletionResult::MessagingFailure)UMETA(DisplayName = "Messaging Failure"),
	NoResponse  = static_cast<uint8>(EPromoteMemberCompletionResult::NoResponse) UMETA(DisplayName = "No Response"),
	LoggedOut = static_cast<uint8>(EPromoteMemberCompletionResult::LoggedOut)  UMETA(DisplayName = "Logged Out"),
	UnknownInternalFailure  = static_cast<uint8>(EPromoteMemberCompletionResult::UnknownInternalFailure)UMETA(DisplayName = "Unknown Internal Failure"),
	Succeeded = static_cast<uint8>(EPromoteMemberCompletionResult::Succeeded)  UMETA(DisplayName = "Succeeded"),
};

UENUM(BlueprintType)
enum class ETamBPLeavePartyCompletionResult : uint8
{
	UnknownClientFailure = static_cast<uint8>(ELeavePartyCompletionResult::UnknownClientFailure),
	NoResponse = static_cast<uint8>(ELeavePartyCompletionResult::NoResponse),
	LoggedOut = static_cast<uint8>(ELeavePartyCompletionResult::LoggedOut),
	UnknownParty = static_cast<uint8>(ELeavePartyCompletionResult::UnknownParty),
	LeavePending = static_cast<uint8>(ELeavePartyCompletionResult::LeavePending),
	UnknownLocalUser = static_cast<uint8>(ELeavePartyCompletionResult::UnknownLocalUser),
	NotMember = static_cast<uint8>(ELeavePartyCompletionResult::NotMember),
	MessagingFailure = static_cast<uint8>(ELeavePartyCompletionResult::MessagingFailure),
	UnknownTransportFailure = static_cast<uint8>(ELeavePartyCompletionResult::UnknownTransportFailure),
	UnknownInternalFailure = static_cast<uint8>(ELeavePartyCompletionResult::UnknownInternalFailure),
	Succeeded = static_cast<uint8>(ELeavePartyCompletionResult::Succeeded)
};

UENUM(BlueprintType)
enum class ETamBPSendPartyInvitationCompletionResult :uint8
{
	NotLoggedIn = static_cast<uint8>(ESendPartyInvitationCompletionResult::NotLoggedIn),
	InvitePending = static_cast<uint8>(ESendPartyInvitationCompletionResult::InvitePending),
	AlreadyInParty = static_cast<uint8>(ESendPartyInvitationCompletionResult::AlreadyInParty),
	PartyFull = static_cast<uint8>(ESendPartyInvitationCompletionResult::PartyFull),
	NoPermission = static_cast<uint8>(ESendPartyInvitationCompletionResult::NoPermission),
	RateLimited = static_cast<uint8>(ESendPartyInvitationCompletionResult::RateLimited),
	UnknownInternalFailure = 0,
	Succeeded = 1
};

UENUM(BlueprintType)
enum class ETamBPJoinPartyCompletionResult :uint8
{	
	/** Unspecified error.  No message sent to party leader. */
	UnknownClientFailure = static_cast<uint8>(EJoinPartyCompletionResult::UnknownClientFailure),
	/** Your build id does not match the build id of the party */
	BadBuild=static_cast<uint8>(EJoinPartyCompletionResult::BadBuild),
	/** Your provided access key does not match the party's access key */
	InvalidAccessKey=static_cast<uint8>(EJoinPartyCompletionResult::InvalidAccessKey),
	/** The party leader already has you in the joining players list */
	AlreadyInLeadersJoiningList = static_cast<uint8>(EJoinPartyCompletionResult::AlreadyInLeadersJoiningList),
	/** The party leader already has you in the party members list */
	AlreadyInLeadersPartyRoster = static_cast<uint8>(EJoinPartyCompletionResult::AlreadyInLeadersPartyRoster),
	/** The party leader rejected your join request because the party is full*/
	NoSpace = static_cast<uint8>(EJoinPartyCompletionResult::NoSpace),
	/** The party leader rejected your join request for a game specific reason, indicated by the NotApprovedReason parameter */
	NotApproved = static_cast<uint8>(EJoinPartyCompletionResult::NotApproved),
	/** The player you send the join request to is not a member of the specified party */
	RequesteeNotMember = static_cast<uint8>(EJoinPartyCompletionResult::RequesteeNotMember),
	/** The player you send the join request to is not the leader of the specified party */
	RequesteeNotLeader = static_cast<uint8>(EJoinPartyCompletionResult::RequesteeNotLeader),
	/** A response was not received from the party leader in a timely manner, the join attempt is considered failed */
	NoResponse = static_cast<uint8>(EJoinPartyCompletionResult::NoResponse),
	/** You were logged out while attempting to join the party */
	LoggedOut = static_cast<uint8>(EJoinPartyCompletionResult::LoggedOut),
	/** You were unable to rejoin the party */
	UnableToRejoin = static_cast<uint8>(EJoinPartyCompletionResult::UnableToRejoin),
	/** Your platform is not compatible with the party */
	IncompatiblePlatform = static_cast<uint8>(EJoinPartyCompletionResult::IncompatiblePlatform),

	/** We are currently waiting for a response for a previous join request for the specified party.  No message sent to party leader. */
	AlreadyJoiningParty = static_cast<uint8>(EJoinPartyCompletionResult::AlreadyJoiningParty),
	/** We are already in the party that you are attempting to join.  No message sent to the party leader. */
	AlreadyInParty = static_cast<uint8>(EJoinPartyCompletionResult::AlreadyInParty),
	/** The party join info is invalid.  No message sent to the party leader. */
	JoinInfoInvalid = static_cast<uint8>(EJoinPartyCompletionResult::JoinInfoInvalid),
	/** We are already in a party of the specified type.  No message sent to the party leader. */
	AlreadyInPartyOfSpecifiedType = static_cast<uint8>(EJoinPartyCompletionResult::AlreadyInPartyOfSpecifiedType),
	/** Failed to send a message to the party leader.  No message sent to the party leader. */
	MessagingFailure = static_cast<uint8>(EJoinPartyCompletionResult::MessagingFailure),

	/** Game specific reason, indicated by the NotApprovedReason parameter.  Message might or might not have been sent to party leader. */
	GameSpecificReason = static_cast<uint8>(EJoinPartyCompletionResult::GameSpecificReason),

	/** Your app id does not match the app id of the party */
	MismatchedApp = static_cast<uint8>(EJoinPartyCompletionResult::MismatchedApp),

	/** DEPRECATED */
	UnknownInternalFailure = 0,

	/** Successully joined the party */
	Succeeded = 1
};

UENUM(BlueprintType)
enum class ETamBPMemberExitedReason : uint8
{
	Unknown= static_cast<uint8>(EMemberExitedReason::Unknown),
	Left = static_cast<uint8>(EMemberExitedReason::Left),
	Removed = static_cast<uint8>(EMemberExitedReason::Removed),
	Kicked = static_cast<uint8>(EMemberExitedReason::Kicked)
};

UENUM(BlueprintType)
enum class ETamBPOnJoinSessionCompleteResult : uint8
{
	/** The join worked as expected */
	Success = static_cast<uint8>(EOnJoinSessionCompleteResult::Success),
	/** There are no open slots to join */
	SessionIsFull = static_cast<uint8>(EOnJoinSessionCompleteResult::SessionIsFull),
	/** The session couldn't be found on the service */
	SessionDoesNotExist = static_cast<uint8>(EOnJoinSessionCompleteResult::SessionDoesNotExist),
	/** There was an error getting the session server's address */
	CouldNotRetrieveAddress = static_cast<uint8>(EOnJoinSessionCompleteResult::CouldNotRetrieveAddress),
	/** The user attempting to join is already a member of the session */
	AlreadyInSession = static_cast<uint8>(EOnJoinSessionCompleteResult::AlreadyInSession),
	/** An error not covered above occurred */
	UnknownError = static_cast<uint8>(EOnJoinSessionCompleteResult::UnknownError)

};

UENUM(BlueprintType)
enum class ETamBPOnlineSessionState : uint8
{
	/** An online session has not been created yet */
	NoSession,
	/** An online session is in the process of being created */
	Creating,
	/** Session has been created but the session hasn't started (pre match lobby) */
	Pending,
	/** Session has been asked to start (may take time due to communication with backend) */
	Starting,
	/** The current session has started. Sessions with join in progress disabled are no longer joinable */
	InProgress,
	/** The session is still valid, but the session is no longer being played (post match lobby) */
	Ending,
	/** The session is closed and any stats committed */
	Ended,
	/** The session is being destroyed */
	Destroying
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateLogoutEOSSubsystem, const bool, bWasSuccessfull);

DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateEOSSessionDestroyed, const bool, bWasSuccessfull);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateUpdateEOSSession, const bool, bWasSuccessfull);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateCancelSearchEOSSession, const bool, bWasSuccessfull);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateFriendListChanged, const int32, LocalUserIdD);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateEOSUserFileWriten, const bool, bWasSuccessfull);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateStartEOSSession, const bool, bWasSuccessfull);
DECLARE_DYNAMIC_DELEGATE_OneParam(FDelegateEnumerateEOSUserFile, const bool, bWasSuccessfull);



DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateEOSFileReadAsSaveGame, const bool, WasSucessful, USaveGame*, SaveData);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateFindEOSSessionBySessionId, const bool, bWasSuccessfull, const FTamBPSessionSearchResultInfos &, SearchResult);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateReadEOSFriendList, const bool, bWasSuccessfull, const FString, ErrorStr);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateUpdateAdvancedPartyMetadata, const bool, bWasSuccessfull, const FString, ErrorStr);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateOnPresenceTaskComplete, const bool, bWasSuccessfull, const FUniqueNetIdRepl&, UserId);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateLoginEOSSubsystem,const bool, bWasSuccessfull,const FString, IdentityProvider);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateEOSSessionCreated, const bool, bWasSuccessfull, const int32, UniqueSessionId);
DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateOnPartyInvitesChanged,
	const FUniqueNetIdRepl&, LocalUserId,
	const TArray<FTamBPOnlinePartyJoinInfo>&, AllInvites);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateEOSSessionFound, const bool, bWasSuccessfull, 
	const TArray<FTamBPSessionSearchResultInfos>&, SearchInfos);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FDelegateOnPartyInvitesReceived,
	const FUniqueNetIdRepl&, LocalUserId,
	const FTamBPOnlinePartyJoinInfo, PartyJoinInfos);

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateCreatePartyCompleted,
	const FUniqueNetIdRepl&, LeaderUserId,
	const FTamBPOnlinePartyId&, PartyId,
	const ETamBPCreatePartyCompletionResult, Result);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateEOSSessionJoined,
	const FName, SessionName,
	const ETamBPOnJoinSessionCompleteResult, JoinResult,
	const int32, SessionUniqueId);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateQueryUsersInfo,
	bool, WasSucessfull,
	const TArray<FUniqueNetIdRepl>&, QueriedUsers,
	const FString, ErrorString);

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateEOSGetFriendPartyJoinInfo, 
	const bool, WasSucessfull, const FTamBPOnlinePartyJoinInfo&, JoinInfo, const FString, ErrorString);

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateEOSQueryPartyMetadata,
	const bool, WasSucessfull, const TArray<FTamBPPartyMetadata>& , Metadata, const FString, ErrorString);


DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateLeavePartyCompleted,
	const FUniqueNetIdRepl &, LocalUserId, const FTamBPOnlinePartyId &, PartyId,
	const ETamBPLeavePartyCompletionResult, Result);
DECLARE_DYNAMIC_DELEGATE_FourParams(FDelegateKickMemberCompleted,
	const FUniqueNetIdRepl &, UserIdLeader, const FTamBPOnlinePartyId &, PartyId,
	const FUniqueNetIdRepl &, KickedMember,
	const ETamBPKickMemberCompletionResult, Result);
DECLARE_DYNAMIC_DELEGATE_FourParams(FDelegatePromoteMemberCompleted,
	const FUniqueNetIdRepl &, UserIdLeader, FTamBPOnlinePartyId&, PartyId,
	const FUniqueNetIdRepl &, PromotedMember,
	const ETamBPPromoteMemberCompletionResult, Result);
DECLARE_DYNAMIC_DELEGATE_FourParams(FDelegateInviteMemberCompleted,
	const FUniqueNetIdRepl&, Sender,const FTamBPOnlinePartyId&, PartyId,
	const FUniqueNetIdRepl&, Recipient,
	const ETamBPSendPartyInvitationCompletionResult, Result);
DECLARE_DYNAMIC_DELEGATE_FourParams(FDelegateCancelInviteMemberCompleted,
	const FUniqueNetIdRepl&, Sender, const FTamBPOnlinePartyId&, PartyId,
	const FUniqueNetIdRepl&, Recipient,
	const FString, ErrorString);
DECLARE_DYNAMIC_DELEGATE_FourParams(FDelegateJoinPartyCompleted,
	const FUniqueNetIdRepl&, LocalUserId, const FTamBPOnlinePartyId&, PartyId,
	const ETamBPJoinPartyCompletionResult, Result,
	const int32, NotApprovedReason);

//no SDK support
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateOnPartyJoinRequestReceived,
	const FUniqueNetIdRepl&, LocalUserId,
	const FTamBPOnlinePartyId&, PartyId,
	const FTamBPOnlinePartyPendingJoinRequestInfoConstRef&, JoinRequestInfos);
DECLARE_DYNAMIC_DELEGATE_FourParams(FDelegateOnPartyMemberExited,
	const FUniqueNetIdRepl&, LocalUserId,
	const FTamBPOnlinePartyId&, PartyId,
	const FUniqueNetIdRepl&, MemberId,
	const ETamBPMemberExitedReason, Reason);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateOnPartyMemberJoined,
	const FUniqueNetIdRepl&, LocalUserId,
	const FTamBPOnlinePartyId&, PartyId,
	const FUniqueNetIdRepl&, NewMemberId);
DECLARE_DYNAMIC_DELEGATE_ThreeParams(FDelegateOnPartyMemberPromoted,
	const FUniqueNetIdRepl&, LocalUserId,
	const FTamBPOnlinePartyId&, PartyId,
	const FUniqueNetIdRepl&, NewLeaderId);



UCLASS()
class KINGDOMS_EDGE_API UEOS_Gameinstance : public UGameInstance
{
	GENERATED_BODY()

private:

public:
	UEOS_Gameinstance(const FObjectInitializer& ObjectInitializer);

	//linked BP delegates
	UPROPERTY()
	FDelegateLoginEOSSubsystem DelegateLoginEOSSubsystem;
	UPROPERTY()
	FDelegateOnPartyInvitesChanged DelegateOnPartyInvitesChanged;
	UPROPERTY()
	FDelegateOnPartyInvitesReceived DelegateOnPartyInvitesReceived;
	UPROPERTY()
	FDelegateLogoutEOSSubsystem DelegateLogoutEOSSubsystem;
	UPROPERTY()
	FDelegateCreatePartyCompleted DelegateCreatePartyCompleted;
	UPROPERTY()
	FDelegateLeavePartyCompleted DelegateLeavePartyCompleted;
	UPROPERTY()
	FDelegateKickMemberCompleted DelegateKickMemberCompleted;
	UPROPERTY()
	FDelegatePromoteMemberCompleted DelegatePromoteMemberCompleted;
	UPROPERTY()
	FDelegateInviteMemberCompleted DelegateInviteMemberCompleted;
	UPROPERTY()
		FDelegateCancelInviteMemberCompleted DelegateCancelInviteMemberCompleted;
	UPROPERTY()
	FDelegateJoinPartyCompleted DelegateJoinPartyCompleted;
	UPROPERTY()
	FDelegateOnPartyJoinRequestReceived DelegateOnPartyJoinRequestReceived;
	UPROPERTY()
	FDelegateOnPartyMemberExited DelegateOnPartyMemberExited;
	UPROPERTY()
		FDelegateOnPartyMemberJoined DelegateOnPartyMemberJoined;
	UPROPERTY()
		FDelegateOnPartyMemberPromoted DelegateOnPartyMemberPromoted;
	UPROPERTY()
		FDelegateEOSSessionCreated DelegateEOSSessionCreated;
	UPROPERTY()
		FDelegateEOSSessionDestroyed DelegateEOSSessionDestroyed;
	UPROPERTY()
		FDelegateEOSSessionFound DelegateEOSSessionFound;
	UPROPERTY()
		FDelegateEOSSessionFound DelegateEOSFriendSessionFound;
	UPROPERTY()
		FDelegateEOSSessionJoined DelegateEOSSessionJoined;
	UPROPERTY()
		FDelegateUpdateEOSSession DelegateUpdateEOSSession;
	UPROPERTY()
		FDelegateStartEOSSession DelegateStartEOSSession;
	UPROPERTY()
		FDelegateCancelSearchEOSSession DelegateCancelSearchEOSSession;
	UPROPERTY()
		FDelegateReadEOSFriendList DelegateReadEOSFriendList;
	UPROPERTY()
		FDelegateFindEOSSessionBySessionId DelegateFindEOSSessionBySessionId;
	UPROPERTY()
		FDelegateOnPresenceTaskComplete DelegateOnPresenceTaskComplete;
	UPROPERTY()
		FDelegateFriendListChanged DelegateFriendListChanged;
	UPROPERTY()
		FDelegateEnumerateEOSUserFile DelegateEnumerateEOSUserFile;
	UPROPERTY()
		FDelegateEOSFileReadAsSaveGame DelegateEOSFileReadAsSaveGame;
	UPROPERTY()
		FDelegateEOSUserFileWriten DelegateEOSUserFileWriten;
	UPROPERTY()
		FDelegateQueryUsersInfo DelegateQueryUsersInfo;
	UPROPERTY()
		FDelegateEOSGetFriendPartyJoinInfo DelegateEOSGetFriendPartyJoinInfo;
	UPROPERTY()
		FDelegateUpdateAdvancedPartyMetadata DelegateUpdateAdvancedPartyMetadata;
	UPROPERTY()
		FDelegateEOSQueryPartyMetadata DelegateEOSQueryPartyMetadata;
	//--- Blueprint Exposed Functions ---

//_IDENTITY_

/**
* Login a given player to EOS according to the setted up AuthGraph. See project settings.
* @param OnLoginCompleted The delegate to fire after the completion.
* @param LocalUserNum The local player number. Same as the getPlayerController().
*/
	UFUNCTION(BlueprintCallable,DisplayName="Login Player", Category = "Tamwyn's EOS|Connection")
	void LoginPlayer(
		const FDelegateLoginEOSSubsystem & OnLoginCompleted,
		int32 LocalUserNum);

/**
* Logout a given player to EOS according to the setted up AuthGraph. See project settings.
* @param OnLoginCompleted The delegate to fire after the completion.
* @param LocalUserNum The local player number. Same as the getPlayerController().
*/
	UFUNCTION(BlueprintCallable, DisplayName = "Logout Player", Category = "Tamwyn's EOS|Connection")
		void LogoutPlayer(
			const FDelegateLogoutEOSSubsystem& OnLogoutCompleted,
			int32 LocalUserNum);
	/**
* Is the player logged to the online subsystem.
* @param LocalUserNum The local player number. Same as the getPlayerController().
*/
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Is Player Logged in", Category = "Tamwyn's EOS|Connection")
		bool IsPlayerLoggedIn(int32 LocalUserNum);
	/**
	* Get the local player display name. You may use get user account instead.
	* @param LocalUserNum The local player number. Same as the getPlayerController().
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Local Player Nickname", Category = "Tamwyn's EOS|Player Infos")
		FString GetPlayerNickname(int32 LocalUserNum);
	/**
	* Get the local playerunique net id. This is very useful!
	* @param LocalUserNum The local player number. Same as the getPlayerController().
	* @param PlayerUniqueNetId The UniqueNetId associated with the player.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Local Player UniqueNetId", Category = "Tamwyn's EOS|Player Infos")
		void GetPlayerUniqueNetId(int32 LocalUserNum, FUniqueNetIdRepl& PlayerUniqueNetId);

	/**
	* Get the account of a player.
	* @param UniqueNetID The UniqueNetId associated with the player.
	* @param UserAccount The account of the player.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Player Account", Category = "Tamwyn's EOS|Player Infos")
		bool GetPlayerAccount(const FUniqueNetIdRepl& UniqueNetID, FTamBPUserOnlineAccount& UserAccount);

	/**
	* Get the autheifucation value associated with a key.
	* @param Key The key to get the value of.
	* @param TargetAccount The account of the player.
	* @param KeyResult The value of the key.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Auth Attribute", Category = "Tamwyn's EOS|Player Infos")
		bool GetAuthAttribute(const FTamBPUserOnlineAccount & TargetAccount, const FString Key, FString& KeyResult);
	/**
	* Get the UniqueNetId of a player from his PlayerController. Caution, Player controllor are relative to each game "instance". The Player controller 0 is always the local, so it would correspond to the number 1 for an other player. Prefer GetUniqueNetIdFromPlayerState as Playerstate are the same object reference between the game "instances".
	* @param PlayerController The PlayerController of the player to get the UniqueNetId of.
	* @param UniqueNetID The UniqueNetID of the player.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Player UniqueNetId from Player Controller", Category = "Tamwyn's EOS|Player Infos")
		void GetPlayerUniqueIdPlayerController(const APlayerController* PlayerController, FUniqueNetIdRepl& UniqueNetID);
	/**
	* Get the UniqueNetId of a player from his PlayerState. Playerstate are the same object reference between the game "instances".
	* @param PlayerState The PlayerState of the player to get the UniqueNetId of.
	* @param UniqueNetID The UniqueNetID of the player.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Player UniqueNetId from Player State", Category = "Tamwyn's EOS|Player Infos")
		void GetPlayerUniqueIdPlayerState(const APlayerState* PlayerState, FUniqueNetIdRepl& UniqueNetID);

	/**
	* Cach the UserInfo of an array of player from his PlayerState.This is used to later call GetUserInfo().
	* @param OnInfoQueried The delegate fired after the completion.
	* @param UniqueNetIdsToQuery The UniqueNetIDs to get the infos of.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Query Users Info", Category = "Tamwyn's EOS|Player Infos")
		void QueryUsersInfos(const FDelegateQueryUsersInfo& OnInfoQueried, const TArray< FUniqueNetIdRepl>& UniqueNetIdsToQuery);

	/**
	* Retrieve the cached UserInfo of Player. QueryUsersInfos() must at least be called once before to cache the datas.
	* @param LocalUserNum The lcoacul user number.
	* @param UniqueNetID The UniqueNetID to get the infos of.
	* @param UserInfo The found user infos.
	*/
	UFUNCTION(BlueprintCallable, BlueprintCallable, DisplayName = "Get User Info", Category = "Tamwyn's EOS|Player Infos")
		void GetUserInfo(const int32 LocalUserNum,const FUniqueNetIdRepl& UniqueNetID, FTamBPUserInfos& UserInfo);
//_Avatars

	//UFUNCTION(BlueprintCallable, DisplayName = "Get Cached Plateform Avatar")
		bool GetCachedPlateformAvatar(const FUniqueNetIdRepl& TargetUser, UTexture2D& OutTexture);

//_PARTY_	
//notes:When a player joins a party via the overlay, the party system will automatically leave the current presence party 
//	(if there is one) and join the new party.

	/**
	* Creates a party on EOS
	* @param OnPartyCreated The Degelgate after completion.
	* @param MaxMembers Total number that can join the party.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Create Party", Category = "Tamwyn's EOS|Party")
		void CreateParty(const FDelegateCreatePartyCompleted& OnPartyCreated, int32 MaxMembers);

	/**
	* Kicks a member of the current EOS party.
	* @param OnMemberKicked The Degelgate after completion.
	* @param UserIdLeader The leader UniqueNetId.
	* @param PartyId The joined party.
	* @param MemberToKick The player to kick.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Kick Member", Category = "Tamwyn's EOS|Party")
		void KickMember(const FDelegateKickMemberCompleted & OnMemberKicked,const FUniqueNetIdRepl& UserIdLeader,
			const FTamBPOnlinePartyId& PartyId, const FUniqueNetIdRepl& UserToKick);
	/**
	* Promote a member of the current EOS party.
	* @param OnMemberPromoted The Degelgate after completion.
	* @param UserIdLeader The leader UniqueNetId.
	* @param PartyId The joined party.
	* @param UserIdToPromote The player to promote.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Promote Member", Category = "Tamwyn's EOS|Party")
		void PromoteMember(const FDelegatePromoteMemberCompleted & OnMemberPromoted,const FUniqueNetIdRepl& UserIdLeader, const FTamBPOnlinePartyId &PartyId, const FUniqueNetIdRepl &UserIdToPromote);
	/**
	* Local player leaves the current EOS party.
	* @param OnPartyLeaved The Degelgate after completion.
	* @param LocalUserNum The local user index.
	* @param PartyId The joined party.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Leave Party", Category = "Tamwyn's EOS|Party")
		void LeaveParty(const FDelegateLeavePartyCompleted & OnPartyLeaved, int32 LocalUserNum, const FTamBPOnlinePartyId& PartyId);

	/**
	* Get the joined parties of a local registered player.
	* @param LocalUserId The local user index.
	* @param OutPartyIdArray The found joined parties.
	* @return Was the search sucessfull?
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Joined Parties", Category = "Tamwyn's EOS|Party")
		bool GetJoinedParties(const FUniqueNetIdRepl &LocalUserId, TArray<FTamBPOnlinePartyId>& OutPartyIdArray);
	/**
	* Get all the members of a joined party.
	* @param LocalUserNum The local user index.
	* @param PartyId The targeted party id.
	* @param OutMembers The found member of the joined party.
	* @return Was the search sucessfull?
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Get Party Members", Category = "Tamwyn's EOS|Party")
		bool GetPartyMembers(int32 LocalUserNum, const FTamBPOnlinePartyId& PartyId, TArray<FTamBPOnlinePartyMember>& OutMembers);
	/**
	* Get the UniqueNetId of a PartyMember.
	* @param PartyMember The PartyMember to get the UniqueNetId of.
	* @param UniqueNetID The UniqueNetID of the player.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Get Member UniqueNetId", Category = "Tamwyn's EOS|Party")
		void GetMemberUniqueId(const FTamBPOnlinePartyMember& PartyMember, FUniqueNetIdRepl& UniqueNetID);

	
	/**
	* Invite a member to the party, the callback is sender side.
	* @param OnInviteCompleted The delegate called whene finished.
	* @param Sender The sender UniqueNetId.
	* @param PartyId The party to invite to. The sender shoulb be present in the party.
	* @param Recipient The UserNetId of target user.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Send Party Invite", Category = "Tamwyn's EOS|Party")
		void SendPartyInvite(const FDelegateInviteMemberCompleted OnInviteCompleted, const FUniqueNetIdRepl& Sender, const FTamBPOnlinePartyId& PartyId, const FUniqueNetIdRepl& Recipient);
	
	/*Cancel a send invitation to a party, NOT SUPPORTED atm!*/
	UFUNCTION(BlueprintCallable, DisplayName = "Cancel Party Invite", Category = "Tamwyn's EOS|Party")
		void CancelPartyInvite(const FDelegateCancelInviteMemberCompleted OnCancelCompleted, const FUniqueNetIdRepl& Sender, const FTamBPOnlinePartyId& PartyId, const FUniqueNetIdRepl& Recipient);

	/**
	* Reject an invitation to a party. Removes the party from pending invites.
	* @param UserRejecting The UniqueNetId of the user rejecting the request.
	* @param UserInviting The UniqueNetId of the user sending the request. You can get it from the invitation infos.
	* @return Did the call succeeded?
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Reject Party Invite", Category = "Tamwyn's EOS|Party")
		bool RejectPartyInvite(const FUniqueNetIdRepl& UserRejecting, const FUniqueNetIdRepl& UserInviting);

	/**
	* Get the list of parties, the player has been invited to.
	* @param UserId The UniqueNetId of the local player querying.
	* @param OutInviteList The invitations search results.
	* @return Did the call succeeded?
	*/
	UFUNCTION(BlueprintCallable,BlueprintPure, DisplayName = "Get Player Pending Invites", Category = "Tamwyn's EOS|Party")
		bool GetPlayerPendingInvites(const FUniqueNetIdRepl& PlayerToQueryFor, TArray<FTamBPOnlinePartyJoinInfo>& OutInviteList);
	/**
	* Get notified each time the invite array changes. Useful to display lists of invites. This is reset whene disconnecting.
	* @param OnChanged The event to fire each time pending invite array is changed.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Listen for Parties Invites Changed", Category = "Tamwyn's EOS|Party")
		void ListenForChangePartiesInvite(const FDelegateOnPartyInvitesChanged & OnChanged);
	/**
	* Bind an event taht executes each time the user receives an invite. Useful to display pop-ups.
	* @param OnReceived The event to fire each time invite is received.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Listen for Parties Invites Received", Category = "Tamwyn's EOS|Party")
		void ListenForReceivePartiesInvite(const FDelegateOnPartyInvitesReceived & OnReceived);

	/**
	* Joins the party get through a join info.
	* @param OnJoined The delegate fired on completion.
	* @param UserId The local user that will to join.
	* @param PartyToJoin The join info to a party. Get with Query Join info or join invite received.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Join Party", Category = "Tamwyn's EOS|Party")
		void JoinParty(const FDelegateJoinPartyCompleted & OnJoined, const FUniqueNetIdRepl& UserId, const FTamBPOnlinePartyJoinInfo& PartyToJoin);
	/**
	* Call on leader to listen to leaders relevant event. E.g join requests
	* @param JoinRequestReceived The delegate fired each time a join request is received. join request are not supported, player need to be invited or need to query the join info on its side.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Setup Leader Listens", Category = "Tamwyn's EOS|Party")
		void SetupLeaderListens(const FDelegateOnPartyJoinRequestReceived & JoinRequestReceived);
	/**
	* Call on members (incl. Leader) to listen to members relevant events. E.g member joins, leaves or is promoted
	* @param OnPartyMemberJoined Fired each time a new member joins a party.
	* @param OnPartyMemberExited Fired each time a memeber exits a joined party.
	* @param OnPartyMemberPromoted Fired each time a member of a joined party is promoted.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Setup Members Listens", Category = "Tamwyn's EOS|Party")
		void SetupMembersListens(const FDelegateOnPartyMemberJoined & OnPartyMemberJoined, const FDelegateOnPartyMemberExited & OnPartyMemberExited, const FDelegateOnPartyMemberPromoted & OnPartyMemberPromoted);
	/**
	* Is the target member the leader of the joined party?
	* @param LocalUserId The user that makes the request.
	* @param PartyId The PartyId of the joined party to ask to.
	* @param MemberUniqueId The member to target look if leader.
	* @return Is this target member the leader?
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, DisplayName = "Is Party Leader", Category = "Tamwyn's EOS|Party")
		bool IsMemberPartyLeader(const FUniqueNetIdRepl& LocalUserId,const FTamBPOnlinePartyId& PartyId, const FUniqueNetIdRepl& MemberUniqueId);
	
	/**
	* Get the party Leader of the current joined party.
	* @param LocalUserId The user that makes the request.
	* @param PartyId The PartyId of the joined party to ask to.
	* @param The found leader UniqueNetId.
	* @return Was the member found?
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Get Leader", Category = "Tamwyn's EOS|Party")
		bool GetPartyLeader(const FUniqueNetIdRepl& LocalUserId, const FTamBPOnlinePartyId& PartyId, FUniqueNetIdRepl& LeaderUniqueNetId);

	/**
	* Find join info to party with specific metadata. Scenario is to join a lobby with the friend we played last time. If more than one result is wished, the delegate can be fired multiple times.
	* @param OnPartyFound The delegate fired on completion.
	* @param PlayerRequesting The local player that makes the request.
	* @param SearchFilters An array of filters for the search accorss EOS existing parties.
	* @param ResultsLimit A limit the the number of results.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Request JoinInfo to Party", Category = "Tamwyn's EOS|Party")
		void RequestJoinInfoToParty(const FDelegateEOSGetFriendPartyJoinInfo& OnPartyFound, const FUniqueNetIdRepl& PlayerRequesting, const TArray<FTamBPPartyMetadata>& SearchFilters, const int ResultsLimit);
	/**
	* Find join info to party with specific metadata. Scenario is to join a lobby with the friend we played last time. If more than one result is wished, the delegate can be fired multiple times.
	* @param LocalUserId The UniqueNetId of the local user making the changes.
	* @param PartyId The party to change the metadata of.
	* @param MetadataToUpload The new metadata, overwriten.
	* @return Did the call start?
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Update Party Metadata", Category = "Tamwyn's EOS|Party")
		bool UpdatePartyMetadata(const FUniqueNetIdRepl& LocalUserId, const FTamBPOnlinePartyId& PartyId, const TArray<FTamBPPartyMetadata>& MetadataToUpload);
	
	/**
	* Updates the visibility of the party and its capacity.
	* @param OnMetadataUpdated Delegate fired on completion
	* @param LocalUserId The local user making this request. Should be the leader.
	* @param PartyId The party to change the metadata of.
	* @param IsPublic Can other peoples find this party?
	* @param IsLocked Can new member join?
	* @param NewCapacity The ne total member of this party.
	*/
	/*@todo: optional datas*/
	UFUNCTION(BlueprintCallable, DisplayName = "Update Party Visibility Metadata", Category = "Tamwyn's EOS|Party")
		void UpdatePartyAdvancedMetadata(const FDelegateUpdateAdvancedPartyMetadata& OnMetadataUpdated, const FUniqueNetIdRepl& LocalUserId, const FTamBPOnlinePartyId& PartyId, const bool IsPublic, const bool IsLocked, const int32 NewCapacity);
	/**
	* Get the join info of a party as a JSON. Scenario is to upload the join info as a metadata, so that other player can authomaticaly join the session after reading the metadatas.
	* @param OnMetadataUpdated Delegate fired on completion
	* @param LocalUserId The local user making this request.
	* @param PartyId The party to get the join info from.
	* @return The joinInfo as a JSON format.
	*/
	UFUNCTION(BlueprintPure, DisplayName = "Get Party Join Json", Category = "Tamwyn's EOS|Party")
		FString GetPartyJoinJson(const FUniqueNetIdRepl& LocalUserId, const FTamBPOnlinePartyId& PartyId);

	/**
	* Returns the PartyID of a party from its JoinInfo.
	* @param PlayerId The local user making this request.
	* @param Joininfo The JoinInfo to get the info from
	* @param OutPartyId The associated Party Id
	*/
	UFUNCTION(BlueprintPure, DisplayName = "Get PartyId from JoinInfo", Category = "Tamwyn's EOS|Party")
		void GetPartyIdFromJoinInfo(const FUniqueNetIdRepl& PlayerId, const FTamBPOnlinePartyJoinInfo& Joininfo, FTamBPOnlinePartyId& OutPartyId);

	/**
	* Queries the wished metadata of an exisiting EOS Party.
	* @param OnMetadataFetched Fired on completion.
	* @param PlayerId The player making this request.
	* @param PartyId The PartyId to get the metadata from.
	* @param InQueryAttributes The attributes that needed to be query.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Get External Party Metadata", Category = "Tamwyn's EOS|Party")
		void GetExternalPartyMetadata(const FDelegateEOSQueryPartyMetadata& OnMetadataFetched, const FUniqueNetIdRepl& PlayerId, const FTamBPOnlinePartyId& PartyId, const TArray<FTamMetadataQuery>& InQueryAttributes);
	
	/**
	* Get the wished metadata of an exisiting EOS Party. These informations are cached, i.e. instanteneaous accessible.
	* @param OnMetadataFetched Fired on completion.
	* @param PlayerId The player making this request.
	* @param PartyId The PartyId to get the metadata from.
	* @param QueryAttributes The attributes that needed to be query.
	* @param OutMetadatas The found values for the given Metadata keys.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Get Joined Party Metadata", Category = "Tamwyn's EOS|Party")
		bool GetJoinedPartyMetadata(const FUniqueNetIdRepl& PlayerId, const FTamBPOnlinePartyId& PartyId, const TArray<FTamMetadataQuery>& QueryAttributes, TArray<FTamBPPartyMetadata>& OutMetadatas);

	
	//_SESSION_	
	/**
	* Create a session on EOS, in wich players from differents machines can play together
	* @param OnSessionCreated Delegate fired on completion.
	* @param HostingPlayerId The player id hosting this session. Should be a local user.
	* @param SessionName The session Name, may be relevant for some sessions events.
	* @param bIsDedicated Should use the dedicated server layer?
	* @param MaxNumOfPublicConnection The total number of public players.
	* @param MaxNumOfPrivateConnection The total number of private players.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Create EOS Session", Category = "Tamwyn's EOS|Session")
		void CreateEOSSession(const FDelegateEOSSessionCreated & OnSessionCreated, const FUniqueNetIdRepl& HostingPlayerId, const FName SessionName, const bool bIsDedicated, const int32 MaxNumOfPublicConnection, const int32 MaxNumOfPrivateConnection);

	/**
	* Destroy a given session. When your server has finished hosting a game, or when the game client disconnects from a server early, you need to destroy the local session.
	* @param OnSessionDestroyed Delegate fired on completion.
	* @param SessionName The session Name is the local name of the session for the client or server. It should be the value you specified when creating or joining the session (depending on which you called).
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Destroy EOS Session", Category = "Tamwyn's EOS|Session")
		void DestroyEOSSession(const FDelegateEOSSessionDestroyed & OnSessionDestroyed, const FName SessionName);

	/**
	* Find an EOS session to join.
	* @param OnSessionsFound Delegate fired on completion.
	* @param MaxResult The max number of search results.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Search EOS Session", Category = "Tamwyn's EOS|Session")
		void SearchEOSSession(const FDelegateEOSSessionFound & OnSessionsFound, int32 MaxResult);

	/**
	* Finds a session of a given friend.
	* @param OnSessionsFound Delegate fired on completion.
	* @param LocalUserNum The max number of search results.
	* @param FriendUniqueNetId The friend unique net id.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Search Friend's EOS Session", Category = "Tamwyn's EOS|Session")
		void SearchFriendEOSSession(const FDelegateEOSSessionFound & OnSessionsFound, const int32 LocalUserNum, const FUniqueNetIdRepl& FriendUniqueNetId);

	/**
	* Cancel an EOS session search.
	* @param OnSearchCanceled Delegate fired on completion.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Cancel Search EOS Session", Category = "Tamwyn's EOS|Session")
		void CancelSearchEOSSession(const FDelegateCancelSearchEOSSession & OnSearchCanceled);

	/**
	* Join a given session
	* @param OnSessionsJoined Delegate fired on completion.
	* @param SessionSearchInfo The sessions search infos: accessible from "Seach EOS Session" .
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Join EOS Session", Category = "Tamwyn's EOS|Session")
		void JoinEOSSession(const FDelegateEOSSessionJoined & OnSessionsJoined, const FTamBPSessionSearchResultInfos & SessionSearchInfo);

	/**
	* Find an advertised session given by a sessionID, for exemple through friend presence. Presence is cached so session infos too.
	* @param OnSessionsFound Delegate fired on completion.
	* @paramSearchingUser The user making this request. Should be the local user.
	* @param SessionId The sessonId of the friend.
	* @param FriendId The friend from wich we want to get the sesson from.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Find Advertised EOS Session from Session ID", Category = "Tamwyn's EOS|Session")
		void FindEOSSessionFromSessionID(const FDelegateFindEOSSessionBySessionId & OnSessionsFound, const FUniqueNetIdRepl& SearchingUser, const FTamBPOnlineSessionId& SessionId, const FTamBPOnlineFriend& FriendId);
	/**
	* Get the current state of the passed ssession.
	* @param SessionName Local name of the session. Should match name given in create seesion.
	* @return The state of the session.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Get EOS Session State", Category = "Tamwyn's EOS|Session")
		ETamBPOnlineSessionState GetEOSSessionState(const FName SessionName);

	/**
	* Refresh an EOS session to apply modifications.
	* @param OnSessionUpdated Delegate fired on completion.
	* @param SessionName Local name of the session. Should match name given in create seesion.
	* @param bSubmitToEOS Should we send the infos to EOS servers?
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Refresh EOS Session", Category = "Tamwyn's EOS|Session")
		void UpdateEOSSession(const FDelegateUpdateEOSSession & OnSessionUpdated, const FName SessionName, const bool bSubmitToEOS);

	/**
	* Marks an EOS Session as InProgress instead of Lobbie or Pending. An EOS session must be created before.
	* @param OnSessionStarted Delegate fired on completion.
	* @param SessionName Local name of the session. Should match name given in create seesion.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Start EOS Session", Category = "Tamwyn's EOS|Session")
		void StartEOSSession(const FDelegateStartEOSSession & OnSessionStarted, const FName SessionName);

	/**
	* Get the settings of the given session.
	* @param SessionName Session Name. Should match name given in create seesion.
	* @param SessionSettings The settings.
	*/
	UFUNCTION(BlueprintPure, DisplayName = "Get EOS Session Settings", Category = "Tamwyn's EOS|Session")
		void GetEOSSessionSettings(const FName SessionName, FTamBPSessionSettings& SessionSettings);



	/**
	* Read the downloaded (auto on login) EOS Friend list, need to be called before all friend call!
	* @param OnReadCompleted Delegate fired on completion.
	* @param LocalUserNum The local player number.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Read EOS Friend List", Category = "Tamwyn's EOS|Friends")
		void ReadEOSFriendList(const FDelegateReadEOSFriendList & OnReadCompleted, const int32 LocalUserNum);
	
	/**
	* Read the EOS cached Friend list, need to call "Read EOS Friend List" once before all friend call!
	* @param LocalUserNum The local player number.
	* @param Friends The found cached friends.
	* @return Was this call a success?
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Get Cached EOS Friend List", Category = "Tamwyn's EOS|Friends")
		bool GetEOSFriendList(const int32 LocalUserNum, TArray<FTamBPOnlineFriend> & Friends);

	/**
	* Retrive if the supposed friend user linked to the UniqueNetId is a friend.
	* @param LocalUserNum The local player number.
	* @param FriendUserID The user ID of the supposed firend.
	* @param Friend The found friend.
	* @return Was this call a success?
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Is an EOS Friend", Category = "Tamwyn's EOS|Friends")
		bool GetEOSFriend(const int32 LocalUserNum, const FUniqueNetIdRepl & FriendUserID, FTamBPOnlineFriend& Friend);

	/**
	* Listen to changes in the friend list, you can call get firend list on the delegate.
	* @param OnFriendListChanged Fired each time the friend list changes.
	* @param LocalUserNum The local player number.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Listen To EOS Friends Changes", Category = "Tamwyn's EOS|Friends")
		void ListenToFriendsChanges(const FDelegateFriendListChanged& OnFriendListChanged, const int32 LocalUserNum);

	/**
	* Need to call QueryPresence if EpicGames is online subsystem before call in presence context! The presence will then be cached.
	* @param OnPresenceTaskComplete Delegate fired on completion.
	* @param LocalUserNum The local player number.
	* @param Friend The friend to query the presence of.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Querry EOS Presence", Category = "Tamwyn's EOS|Friends")
		void QuerryEOSPresence(const FDelegateOnPresenceTaskComplete & OnPresenceTaskComplete, const int32 LocalUserNum, const FTamBPOnlineFriend & Friend);

	/**
	* Get the friend Presence, need to call "Read EOS Friend List" once before all friend call, Need to call QueryPresence if EpicGames is online subsystem!
	* @param Friend The Friend to get the presence of.
	* @param PresenceInfo The found presence of this user.
	*/
	UFUNCTION(BlueprintPure, DisplayName = "Get cached EOS Friend Presence", Category = "Tamwyn's EOS|Friends")
		void GetEOSFriendPresence(const FTamBPOnlineFriend & Friend, FTamBPOnlinePresenceData & PresenceInfo);


	//Player Data Storage

	/**
	* Before you can read files from Player Data Storage, you need to query the list of files that the user has by calling "Enumerate User Files".
	* @param OnEnumerateComplete Delegate fired on completion.
	* @param UserID The local userid making this request.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Enumerate User File", Category = "Tamwyn's EOS|User Title Storage")
		void EnumerateEOSUserFile(const FDelegateEnumerateEOSUserFile&  OnEnumerateComplete, const FUniqueNetIdRepl & UserID);
	/**
	* Call EnumerateUserFile before! Return the downloaded list of all UserFile path of the local user.
	* @param LocalUserId The local user Id.
	* @param PlayerFileNames The list of all files path.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Get User File List", Category = "Tamwyn's EOS|User Title Storage")
		void GetEOSUserFileList(const FUniqueNetIdRepl & LocalUserId, TArray<FString>& PlayerFileNames);

	/**
	* Read the given EOS UserFile as an save game object.
	* @param OnFileRead Delegate fired on completion.
	* @param LocalUserId The local user Id.
	* @param PlayerFileName The save file path.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Read User File as SaveGameObject", Category = "Tamwyn's EOS|User Title Storage")
		void ReadEOSUserFileAsSavegameObject(const FDelegateEOSFileReadAsSaveGame & OnFileRead,const FUniqueNetIdRepl & LocalUserId,const FString PlayerFileName);
	/**
	* Write the given EOS UserFile as an save game object and upload it to the User's profile. Data will be serialised and upload as UInt8.
	* @param OnFileWrote Delegate fired on completion.
	* @param LocalUserId The local user Id.
	* @param PlayerFileName The save file path.
	* @param SaveData The save oject to serialise and upload.
	* @param CompressBeforeUpload Should we compress the data before uploaded them? Recomanded.
	*/
	UFUNCTION(BlueprintCallable, DisplayName = "Write SaveGameObject as User File", Category = "Tamwyn's EOS|User Title Storage")
		void WriteSavegameObjectAsEOSUserFile(const FDelegateEOSUserFileWriten & OnFileWrote, const FUniqueNetIdRepl & LocalUserId, const FString PlayerFileName, USaveGame* SaveData,const bool CompressBeforeUpload);
	



	//delegates
	void HandleLoginComplete(
		const int32 LocalUserNum,
		const bool bWasSuccessful,
		const FUniqueNetId& UserID,
		const FString& Error);
	FDelegateHandle LoginDelegateHande;

	void HandleLogoutComplete(
		const int32 LocalUserNum,
		bool bWasSuccessful);
	FDelegateHandle LogoutDelegateHande;

	void HandleOnPartyInviteChanged(const FUniqueNetId& LocalUserId);
	FDelegateHandle OnPartyInviteChangedDelegateHandle;

	void HandleOnPartyInviteReceivedEx(
		const FUniqueNetId& LocalUserId,
		const IOnlinePartyJoinInfo& InviationToParty);
	FDelegateHandle OnPartyInviteReceivedExDelegateHandle;

	void HandleOnPartyJoinRequestReceived(
		const FUniqueNetId& LocalUserId,
		const FOnlinePartyId& PartyId,
		const IOnlinePartyPendingJoinRequestInfo& JoinRequestInfos);

	void HandleOnPartyMemberExited(
		const FUniqueNetId& LocalUserId,
		const FOnlinePartyId& PartyId,
		const FUniqueNetId& MemberId,
		const EMemberExitedReason Reason
	);
	void HandleOnPartyMemberJoined(
		const FUniqueNetId& LocalUserId,
		const FOnlinePartyId& PartyId,
		const FUniqueNetId& NewMemberId
	);
	void HandleOnPartyMemberPromoted(
		const FUniqueNetId& LocalUserId,
		const FOnlinePartyId& PartyId,
		const FUniqueNetId& NewLeaderId
	);

	void HandleCreateSessionCompleted(
		const FName  SessionName, 
		const bool  bWasSuccessful);
	FDelegateHandle CreateSessionDelegateHandle;

	void HandleDestroySessionCompleted(
		const FName  SessionName, 
		const bool  bWasSuccessful);
	FDelegateHandle DestroySessionDelegateHandle;

	void HandleFindSessionCompleted(
		const bool bWasSuccessful, 
		const TSharedRef<FOnlineSessionSearch> SessionSearch, 
		const int32 MaxResult);
	FDelegateHandle FindSessionsDelegateHandle;

	void HandleFindFriendSessionCompleted(
		const int32 LocalUserId,
		const bool bWasSuccessful,
		const TArray<FOnlineSessionSearchResult>& SearchResults);
	FDelegateHandle FindFriendSessionsDelegateHandle;

	void HandleJoinSessionCompleted(
		const FName SessionName,
		const EOnJoinSessionCompleteResult::Type JoinResult,
		const FTamBPSessionSearchResultInfos SessionSearchInfo);
	FDelegateHandle JoinSessionDelegateHandle;

	void HandleSessionUpdated(
		const FName SessionName,
		const bool bWasSuccessfull);
	FDelegateHandle UpdateSessionDelegateHandle;

	void HandleSessionStarted(
		const FName SessionName,
		const bool bWasSuccessfull);
	FDelegateHandle StartSessionDelegateHandle;

	void HandleFindSessionCanceled(const bool bWasSuccessfull);
	FDelegateHandle CancelFindSessionDelegateHandle;

	void OnReadFriendsComplete(int32 LocalUserNum,
		bool bWasSuccessful, 
		const FString& ListName, 
		const FString& ErrorStr);

	void HandleSingleSessionResultComplete(
		int32 LocalUserNum,
		bool bWasSuccesfull, 
		const FOnlineSessionSearchResult & SearchResult);

	void HandleOnPresenceTaskCompleteDelegate(
		const class FUniqueNetId &UserId,
		const bool bWasSuccessfull);
	void HandleOnFriendsListChanged(int32 LocalUserNum);
	FDelegateHandle OnFriendListChangedDelegateHandle;

	void HandleEnumerateUserFileComplete(bool bWasSuccessful,
		const FUniqueNetId& UserId);
	FDelegateHandle EnumerateUserFilesDelegateHandle;

	void HandleReadUserFileComplete(bool bWasSuccessful,
		const FUniqueNetId& UserId,
		const FString& FileName);
	FDelegateHandle ReadUserFileDelegateHandle;

	void HandleWriteUserFileComplete(bool bWasSuccessful,
		const FUniqueNetId& UserId,
		const FString& FileName);
	FDelegateHandle WriteUserDataDelegateHandle;

	void HandleQueryUserInfoComplete(int32 LocalUserNum,
		bool WasSuccessful,
		const TArray<TSharedRef<const FUniqueNetId>> &UserIds,
		const FString &ErrorStr
	);
	FDelegateHandle QueryUserInfoDelegateHandle;
	//sharedRef
	TSharedRef<FPartyConfiguration> PartyConfig = MakeShared<FPartyConfiguration>();
	TSharedRef<FOnlineSessionSettings> SessionSettings = MakeShared<FOnlineSessionSettings>();
	TSharedRef<FOnlineSessionSearch> SessionSearch = MakeShared<FOnlineSessionSearch>();
	TArray<FTamMetadataQuery> QueryAttributes;
};
	