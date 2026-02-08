// Fill out your copyright notice in the Description page of Project Settings.

#pragma once


#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSessionSettings.h"
#include "MultiplayerSessionsSubsystem.h"
#include "Components/Button.h"
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Menu.generated.h"

/*
 * UMenu @breif: This class functions as a generic user interface for hosting and/or joining multiplayer sessions created by the subsystem.
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/*
	 * @breif: This function should be called via the level BP. It's purpose is to set the menu widget class, enable button functionality, and set 
	 * important variables for multiplay connection. 
	 * @param NumberOfPublicConnections: Number of permitted connections to join the host (1 host + 4 players: 5 player max lobby)
	 * @param TypeOfMatch: String based identifier for match type filtering. One of the search methods when creating/finding our server lobby.
	 * @param LobbyPath: The cooked file path of our lobby map. ** Ensure the path you include is cooked in project settings **
	 */
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnections = 4, FString TypeOfMatch = TEXT("FreeForAll"), FString LobbyPath = TEXT("/Game/ThirdPerson/Lobby?listen"));

protected:

	/*
	 * @breif: Function for initializing the menu class. We use this period to bind button delegates.
	 */
	virtual bool Initialize() override;
	
	/*
	 * @breif: Function for de-initializing the menu class.
	 */
	virtual void NativeDestruct() override;

// Callbacks for custom delegates on MultiplayerSessionsSubsystem
	
	/*
	 * @breif: Function called as a reaction to creating a session. This function is not responsible for creating the session, but based on the passed
	 * bool will initiate server travel to our lobby. 
	 * @param: bWasSuccessful: bool indicating if our session was created successfully or not.
	 */
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	
	/*
	 * @breif: Currently un-implemented. Implement if you desire custom logic to be called when the session is destroyed. 
	 * @param: bWasSuccessful: bool indicating if our session was destroyed successfully or not.
	 */
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	
	/*
	 * @breif: Currently un-implemented. Implement if you desire custom logic to be called when the multiplayer session is started.
	 * @param: bWasSuccessful: bool indicating if our session was started successfully or not.
	 */
	UFUNCTION()
	void OnStartSession(bool bWasSuccessful);
	
	/*
	 * @breif: From an array of multiplayer server search results, filters the results based on our specifications such as match type and region. 
	 * If we find a matching session, we attempt to join it via our multiplayer subsystem.
	 * @param: SessionResults: A passed array of multiplayer sessions, unfiltered. 
	 * @param: bWasSuccessful: bool representing if we found a session.
	 */
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);

	/*
	 * @breif: Upon successfully connecting via steam, OnJoinSession connects the client to the specifed lobby map via a client travel. 
	 * @param: Result: The result of our join attempt (succuss?). 
	 */
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	

private:

	/*
	 * @breif: Function to remove the menu widget from client POV and disable it's functionality.
	 */
	void MenuTearDown();

	/*
	 * @breif: Called as a delegate in reaction to Host Button on our menu BP being clicked. Attempts to create a multiplayer session. 
	 */
	UFUNCTION()
	void HostButtonClicked();

	/*
	 * @breif: Called as a delegate in reaction to Host Button on our menu BP being clicked. Attempts to find a multiplayer session. 
	 */
	UFUNCTION()
	void JoinButtonClicked();

	// Host and Join button are represented via a BP_Widget as our menu. It's important that the widget has 2 buttons with the EXACT
	// naming conventions that you see below (case sensitive), otherwise the menu will not undertand their connection.
	UPROPERTY(meta = (BindWidget))
	UButton* HostButton;
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;

	// SS designed to handle all online session functionality
	UMultiplayerSessionsSubsystem* MultiplayerSessionsSubsystem;
	int32 NumPublicConnections = 4;
	FString MatchType = TEXT("FreeForAll");
	FString PathToLobby = TEXT("");
};
