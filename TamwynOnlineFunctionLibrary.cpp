// Fill out your copyright notice in the Description page of Project Settings.


#include "TamwynOnlineFunctionLibrary.h"


bool UTamwynOnlineFunctionLibrary::EqualEqual_UniqueNetIdRepl(const FUniqueNetIdRepl& A, const FUniqueNetIdRepl& B)
{
	return A.ToString() == B.ToString();
}

FString UTamwynOnlineFunctionLibrary::Conv_UniqueNetIdReplToString(const FUniqueNetIdRepl& InUniqueNetId)
{
	return InUniqueNetId.ToString();
}

FString UTamwynOnlineFunctionLibrary::Conv_FTamBPOnlinePartyIdlToString(const FTamBPOnlinePartyId& PartyId)
{
	return PartyId.GetOnlinePartyId()->ToString();
}