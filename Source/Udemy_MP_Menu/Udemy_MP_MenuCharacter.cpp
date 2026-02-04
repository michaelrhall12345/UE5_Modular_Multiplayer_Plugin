// Copyright Epic Games, Inc. All Rights Reserved.

#include "Udemy_MP_MenuCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Udemy_MP_Menu.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"


AUdemy_MP_MenuCharacter::AUdemy_MP_MenuCharacter()
	
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	
	IOnlineSubsystem* OnlineSubsystem = IOnlineSubsystem::Get();
	if (OnlineSubsystem)
	{
		OnlineSessionInterface = OnlineSubsystem->GetSessionInterface();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Blue, FString::Printf(TEXT("Found Subsystem %s"), *OnlineSubsystem->GetSubsystemName().ToString()));
		}
	}

	// Create delegates by using the CreateUObject function. These functions constructs a new delegate and takes the address of our callback function, binding it.
	CreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnCreateSessionComplete);
	FindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &ThisClass::OnFindSessionComplete);
	JoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &ThisClass::OnJoinSessionComplete);
}

void AUdemy_MP_MenuCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) 
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUdemy_MP_MenuCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AUdemy_MP_MenuCharacter::Look);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUdemy_MP_MenuCharacter::Look);
	}
	else
	{
		UE_LOG(LogUdemy_MP_Menu, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AUdemy_MP_MenuCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();
	DoMove(MovementVector.X, MovementVector.Y);
}

void AUdemy_MP_MenuCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AUdemy_MP_MenuCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AUdemy_MP_MenuCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AUdemy_MP_MenuCharacter::DoJumpStart()
{
	Jump();
}

void AUdemy_MP_MenuCharacter::DoJumpEnd()
{
	StopJumping();
}

// Called when pressing 1 key for testing purposes 
void AUdemy_MP_MenuCharacter::CreateGameSession()
{
	if (!OnlineSessionInterface.IsValid()) // being a smart ptr, we use the IsValid function
	{
		return;
	}

	auto ExistingSession = OnlineSessionInterface->GetNamedSession(NAME_GameSession);	// We always use the same session name, so this is a global var to use.
	if (ExistingSession != nullptr)
	{
		OnlineSessionInterface->DestroySession(NAME_GameSession);
	}

	// Add our delegate to session interface's delegate list
	OnlineSessionInterface->AddOnCreateSessionCompleteDelegate_Handle(CreateSessionCompleteDelegate);

	TSharedPtr<FOnlineSessionSettings> sessionSettings = MakeShareable(new FOnlineSessionSettings());
	sessionSettings->bIsLANMatch = false;
	sessionSettings->NumPublicConnections = 4;
	sessionSettings->bAllowJoinInProgress = true;
	sessionSettings->bAllowJoinViaPresence = true;
	sessionSettings->bShouldAdvertise = true;
	sessionSettings->bUsesPresence = true;
	sessionSettings->bUseLobbiesIfAvailable = true;
	sessionSettings->Set(FName("MatchType"), FString("FreeForAll"), EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	
	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!localPlayer || !localPlayer->GetPreferredUniqueNetId().IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid LocalPlayer/UniqueNetId for CreateSession"));
		return;
	}
	
	OnlineSessionInterface->CreateSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, *sessionSettings);
}

// Called by pressing the 2 key for testing purposes
void AUdemy_MP_MenuCharacter::JoinGameSession()
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}
	
	OnlineSessionInterface->AddOnFindSessionsCompleteDelegate_Handle(FindSessionsCompleteDelegate);

	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	SessionSearch->MaxSearchResults = 10000; // We hard code this value to be high bc/ we are currently using Steams example session ID, 480. (So we will find many)
	SessionSearch->bIsLanQuery = false;
	SessionSearch->QuerySettings.Set(SEARCH_LOBBIES, true, EOnlineComparisonOp::Equals);

	const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
	if (!localPlayer || !localPlayer->GetPreferredUniqueNetId().IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("No valid LocalPlayer/UniqueNetId for JoinGameSession"));
		return;
	}

	OnlineSessionInterface->FindSessions(*localPlayer->GetPreferredUniqueNetId(), SessionSearch.ToSharedRef());
}

void AUdemy_MP_MenuCharacter::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15.f,
				FColor::Blue,
				FString::Printf(TEXT("Created Session: %s"), *SessionName.ToString()));
		}

		UWorld* world = GetWorld();
		if (world)
		{
			world->ServerTravel(FString("/Game/ThirdPerson/Lobby?listen")); // ?listen marks to join this map as a listen server

		}

	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1,
				15.f,
				FColor::Red,
				FString(TEXT("Failed to create session!")));
		}
	}
}

void AUdemy_MP_MenuCharacter::OnFindSessionComplete(bool bWasSuccessful)
{
	for (auto Result : SessionSearch->SearchResults)
	{
		FString id = Result.GetSessionIdStr();
		FString user = Result.Session.OwningUserName;
		FString matchType;

		if (!OnlineSessionInterface.IsValid())
		{
			return;
		}

		Result.Session.SessionSettings.Get(FName("MatchType"), matchType);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,
				15.f,
				FColor::Cyan,
				FString::Printf(TEXT("Id: %s, User: %s"), *id, *user)
			);
			
			if (matchType == FString("FreeForAll"))
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					15.f,
					FColor::Cyan,
					FString::Printf(TEXT("Joining Match Type: %s"), *matchType)
				);

				// Delegate will now be called after join session completion
				OnlineSessionInterface->AddOnJoinSessionCompleteDelegate_Handle(JoinSessionCompleteDelegate);
				
				const ULocalPlayer* localPlayer = GetWorld()->GetFirstLocalPlayerFromController();
				if (!localPlayer || !localPlayer->GetPreferredUniqueNetId().IsValid())
				{
					UE_LOG(LogTemp, Warning, TEXT("No valid LocalPlayer/UniqueNetId for JoinGameSession"));
					return;
				}

				OnlineSessionInterface->JoinSession(*localPlayer->GetPreferredUniqueNetId(), NAME_GameSession, Result);
				return;
			}
		}
		
	}
}

void AUdemy_MP_MenuCharacter::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (!OnlineSessionInterface.IsValid())
	{
		return;
	}

	if (Result != EOnJoinSessionCompleteResult::Success)
	{
		UE_LOG(LogTemp, Warning, TEXT("JoinSession failed: %d"), (int32)Result);
		return;
	}

	FString address;
	if (OnlineSessionInterface->GetResolvedConnectString(NAME_GameSession, address) && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			15.f,
			FColor::Yellow,
			FString::Printf(TEXT("Connect String: %s"), *address)
		);

		APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
		if (PlayerController)
		{
			PlayerController->ClientTravel(address, ETravelType::TRAVEL_Absolute);
		}
	}
}