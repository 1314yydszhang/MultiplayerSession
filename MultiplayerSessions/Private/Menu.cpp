// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu.h"
#include"Components/Button.h"
#include "MultiplayerSessionsSubsystem.h"
#include"OnlineSessionSettings.h"
#include"OnlineSubsystem.h"
void UMenu::MenuSetup(int32 NumberOfPublicConnection , FString TypeOfMatch,FString LobbyPath)
{
	NumPublicConnections = NumberOfPublicConnection;
	MatchType = TypeOfMatch;
	PathToLobby = FString::Printf(TEXT("%s?listen"), *LobbyPath);
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeUIOnly InputModeDate;
			InputModeDate.SetWidgetToFocus(TakeWidget());
			InputModeDate.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			PlayerController->SetInputMode(InputModeDate);
			PlayerController->SetShowMouseCursor(true);
		}
	}
	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionSubsystem =GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
	}
	//bind to delegate
	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->MultiplayerOnCreateSessionComplete.AddDynamic(this, &ThisClass::OnCreateSession);
		MultiplayerSessionSubsystem->MultiplayerOnFindSessionsComplete.AddUObject(this, &ThisClass::OnFindSessions);
		MultiplayerSessionSubsystem->MultiplayerOnJoinSessionComplete.AddUObject(this, &ThisClass::OnJoinSession);
		MultiplayerSessionSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &ThisClass::OnDestroySession);
		MultiplayerSessionSubsystem->MultiplayerOnStartSessionComplete.AddDynamic(this, &ThisClass::OnStartSession);
	
	}
}

bool UMenu::Initialize()
{
	if (!Super::Initialize())//父类返回假就退出
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

void UMenu::OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)
{
	MenuTearDown();
	Super::OnLevelRemovedFromWorld(InLevel,InWorld);
}

void UMenu::OnCreateSession(bool bWasSuccessful)
{
	if (bWasSuccessful)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,//-1不覆盖之前的信息
				15.f,//持续15秒
				FColor::Red,//红色
				FString(TEXT("Create success"))
			);
		}
		//成功创建会话再跳转
		UWorld* World = GetWorld();
		if (World)
		{
			World->ServerTravel(PathToLobby);
		}
	}
	else {
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(
				-1,//-1不覆盖之前的信息
				15.f,//持续15秒
				FColor::Red,//红色
				FString(TEXT("Failed success"))
			);
		}
		HostButton->SetIsEnabled(true);

	}
	
}

void UMenu::OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful)
{

	if (MultiplayerSessionSubsystem==nullptr)
	{
		return;
	}
	GEngine->AddOnScreenDebugMessage(
		-1,//-1不覆盖之前的信息
		15.f,//持续15秒
		FColor::Red,//红色
		FString(TEXT("UMenu::OnFindSessions"))
	);
	for (auto Result : SessionResults)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,//-1不覆盖之前的信息
			15.f,//持续15秒
			FColor::Red,//红色
			FString(TEXT("Result"))
		);
		FString SettingsValue;
		//获取定义会话设置的键值对组合 显式实例化支持的类型以获取上面的模板 如果找到，则为 true，否则为 false
		//获取FName("MatchType")到SettingsValue
		Result.Session.SessionSettings.Get(FName("MatchType"), SettingsValue);
		if (SettingsValue == MatchType)
		{
			MultiplayerSessionSubsystem->JoinSession(Result);
			GEngine->AddOnScreenDebugMessage(
				-1,//-1不覆盖之前的信息
				15.f,//持续15秒
				FColor::Red,//红色
				FString(TEXT("StartJoin"))
			);
			return;
		}
	}
	if (!bWasSuccessful || SessionResults.Num() == 0)
	{
		JoinButton->SetIsEnabled(true);

	}
}

void UMenu::OnJoinSession(EOnJoinSessionCompleteResult::Type Result)
{
	GEngine->AddOnScreenDebugMessage(
		-1,//-1不覆盖之前的信息
		15.f,//持续15秒
		FColor::Red,//红色
		FString(TEXT("OnJoinSession"))
	);
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get();
	if (Subsystem)
	{
		IOnlineSessionPtr SessionInterface = Subsystem->GetSessionInterface();
		if (SessionInterface)
		{
			FString Address;
			SessionInterface->GetResolvedConnectString(NAME_GameSession, Address);
			APlayerController* PlayerController = GetGameInstance()->GetFirstLocalPlayerController();
			if (PlayerController)
			{
				GEngine->AddOnScreenDebugMessage(
					-1,//-1不覆盖之前的信息
					15.f,//持续15秒
					FColor::Red,//红色
					FString(TEXT("TravelLobby"))
				);
				PlayerController->ClientTravel(Address, ETravelType::TRAVEL_Absolute);
			}
		}
	}
	if (Result != EOnJoinSessionCompleteResult::Success)
	{

		JoinButton->SetIsEnabled(true);
	}
}


void UMenu::OnDestroySession(bool bWasSuccessful)
{
}

void UMenu::OnStartSession(bool bWasSuccessful)
{
}

void UMenu::HostButtonClicked()
{
	HostButton->SetIsEnabled (false);
	//调用子系统的类成员
	if (MultiplayerSessionSubsystem)
	{

		MultiplayerSessionSubsystem->CreateSession(NumPublicConnections, MatchType);
	
	}
}

void UMenu::JoinButtonClicked()
{
	JoinButton->SetIsEnabled(false);

	if (MultiplayerSessionSubsystem)
	{
		MultiplayerSessionSubsystem->FindSessions(10000);
    }
}

void UMenu::MenuTearDown()
{
	RemoveFromParent();
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* PlayerController = World->GetFirstPlayerController();
		if (PlayerController)
		{
			FInputModeGameOnly InputModeDate;		
			PlayerController->SetInputMode(InputModeDate);
			PlayerController->SetShowMouseCursor(false);
		}
	}
}
