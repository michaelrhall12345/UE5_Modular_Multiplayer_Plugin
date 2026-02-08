
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LobbyGameMode.generated.h"

/**
 * ALobbyGameMode @brief: This custom gamemode class is used to simply track player entry and exit upon multiplayer
 * connection. If you use this gamemode, make sure that your override character class is replicated in order to function with the plugin. 
 */
UCLASS()
class UDEMY_MP_MENU_API ALobbyGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	/*
	 * @brief: This function requires a valid game state, which we get from the AGameModeBase class. It tracks how many players
	 * are on the server, and prints to the screen when a new player joins. 
	 * 
	 * @param NewPlayer: Player controller of an incoming player. 
	 */
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/*
	 * @brief: This function also requires a valid game state, which we get from the AGameModeBase class. It tracks and prints to the screen
	 * when a player leaves the game
	 *
	 * @param Exiting: Controller of an exiting player.
	 */
	virtual void Logout(AController* Exiting) override;
};
