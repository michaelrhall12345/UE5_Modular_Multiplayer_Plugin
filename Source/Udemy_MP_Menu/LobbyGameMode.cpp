// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if (!GameState)
	{
		return;
	}
	
	int32 numberOfPlayers = GameState.Get()->PlayerArray.Num();
	
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			1,
			60.f,
			FColor::Yellow,
			FString::Printf(TEXT("Players in game: %d"), numberOfPlayers));
			
		APlayerState* playerState = NewPlayer->GetPlayerState<APlayerState>();
		if (playerState)
		{
			FString playerName = playerState->GetPlayerName();
			GEngine->AddOnScreenDebugMessage(
				-1,
				60.f,
				FColor::Cyan,
				FString::Printf(TEXT("%s has joined the game!"), *playerName));
		}
	}
	
}

void ALobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	if (!GameState)
	{
		return; 
	}

	int32 numberOfPlayers = GameState.Get()->PlayerArray.Num();
	APlayerState* playerState = Exiting->GetPlayerState<APlayerState>();
	if (playerState && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			1,
			60.f,
			FColor::Yellow,
			FString::Printf(TEXT("Players in game: %d"), numberOfPlayers - 1));	// hack- to do fix :p

		FString playerName = playerState->GetPlayerName();
		GEngine->AddOnScreenDebugMessage(
			-1,
			60.f,
			FColor::Cyan,
			FString::Printf(TEXT("%s has exited the game!"), *playerName));
	}
}
