// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include "OnlineSubsystem.h"

// Called within Level BP
void UMenu::MenuSetup(int32 NumberOfPublicConnections, FString TypeOfMatch, FString LobbyPath)
{
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	NumPublicConnections = NumberOfPublicConnections;
	MatchType = TypeOfMatch;
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PLayerController = World->GetFirstPlayerController();
		if (PLayerController)
		{
			FInputModeUIOnly inputModeData;
			inputModeData.SetWidgetToFocus(TakeWidget());
			inputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PLayerController->SetInputMode(inputModeData);
			PLayerController->SetShowMouseCursor(true);
		}
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem< UMultiplayerSessionsSubsystem>();
	}

	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionsSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
		MultiplayerSessionsSubsystem->MultiplayerOnFindSessionComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionsSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	if (HostButton)
	{
		HostButton->OnClicked.AddDynamic(this, &ThisClass::HostButtonClicked);
	}
	if (JoinButton)
	{
		JoinButton->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
	}
	return true;
}

void UMenu::NativeDestruct()
{
	MenuTearDown();
	Super::NativeDestruct();
}

/******************************************************************************************************************************
 * Delegate bound functions
 ******************************************************************************************************************************/

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Green,
				FString(TEXT("Session Created Successfully!"))
			);
		}
		
		UWorld* world = GetWorld();
		if (world)
		{
			const bool bTravelStarted = world->ServerTravel(PathToLobby);

			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1, 15.f, bTravelStarted ? FColor::Green : FColor::Red,
					FString::Printf(TEXT("ServerTravel started: %s"), bTravelStarted ? TEXT("TRUE") : TEXT("FALSE"))
				);
			}

		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to create session..."))
			);
		}
		HostButton->SetIsEnabled(true);
	}
}

void UMenu::OnDestroySession(bool bWasSuccessful)
{

}

void UMenu::OnStartSession(bool bWasSuccessful)
{

}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{
	if (MultiplayerSessionsSubsystem == nullptr)
	{
		return;
	}
	if (!bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15.f,
				FColor::Red,
				TEXT("UMenu::OnFindSessions: FindSessions FAILED; bWasSuccessful == false."));
		}
		return;
	}

	for (auto Result : SessionResults)
	{
		FString SettingsValue;
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Magenta,
					FString::Printf(TEXT("UMenu::OnFindSessions: Session Found, Session ID: %s. Joining. "), *Result.GetSessionIdStr())
				);
			}
			MultiplayerSessionsSubsystem->JoinSession(Result);
			return;
		}
	}
	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Magenta,
			(TEXT("UMenu::OnFindSessions: No Session Found. Returning. "))
		);
	}
	return;
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	if (GEngine)
	{
		/*********************** Purely for testing *************************************************/
		auto JoinResultToString = [](EOnJoinSessionCompleteResult::Type R)
			{
				switch (R)
				{
				case EOnJoinSessionCompleteResult::Success: return TEXT("Success");
				case EOnJoinSessionCompleteResult::SessionIsFull: return TEXT("SessionIsFull");
				case EOnJoinSessionCompleteResult::SessionDoesNotExist: return TEXT("SessionDoesNotExist");
				case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress: return TEXT("CouldNotRetrieveAddress");
				case EOnJoinSessionCompleteResult::AlreadyInSession: return TEXT("AlreadyInSession");
				case EOnJoinSessionCompleteResult::UnknownError: return TEXT("UnknownError");
				default: return TEXT("Other");
				}
			};

		GEngine->AddOnScreenDebugMessage(
			-1, 15.f, FColor::Magenta,
			FString::Printf(TEXT("JoinSession result: %s"), JoinResultToString(Result))
		);
	} /*********************** Purely for testing *************************************************/
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		return;
	}

	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem != nullptr)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			FString address;
			const bool bGotAddress = SessionInterface->GetResolvedConnectString(NAME_GameSession, address);
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Yellow,
					FString::Printf(TEXT("ResolvedConnectString: %s | %s"),
					bGotAddress ? TEXT("TRUE") : TEXT("FALSE"),
					*address)
				);
			}

			APlayerController* playerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (playerController)
			{
				playerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		JoinButton->SetIsEnabled(true);
	}
}

/******************************************************************************************************************************
 * Menu UI Functionality 
 ******************************************************************************************************************************/

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled(false);
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->CreateSession(NumPublicConnections, MatchType);
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);
	if(MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->FindSessions(10000);
	}
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* world = GetWorld();
	if (world)
	{
		APlayerController* playerController = world->GetFirstPlayerController();
		if (playerController)
		{
			FInputModeGameOnly InputModeData;
			playerController->SetInputMode(InputModeData);
			playerController->SetShowMouseCursor(false);
		}
	}
}