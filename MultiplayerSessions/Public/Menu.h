// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "Menu.generated.h"

/**
 * 
 */
UCLASS()
class MULTIPLAYERSESSIONS_API UMenu : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void MenuSetup(int32 NumberOfPublicConnection=4,FString TypeOfMatch= FString(TEXT("FreeForAll")),FString LobbyPath=FString(TEXT("/Game/Maps/Lobby")));
protected:
	virtual bool Initialize()override;
	virtual void OnLevelRemovedFromWorld(ULevel* InLevel, UWorld* InWorld)override;//Handles level is being removed from the world

	//Callbacks for the custom delegates on the MultiplayerSessionsSubsystem
	UFUNCTION()
	void OnCreateSession(bool bWasSuccessful);
	void OnFindSessions(const TArray<FOnlineSessionSearchResult>& SessionResults, bool bWasSuccessful);
	void OnJoinSession(EOnJoinSessionCompleteResult::Type Result);
	UFUNCTION()
	void OnDestroySession(bool bWasSuccessful);
	UFUNCTION()
    void OnStartSession(bool bWasSuccessful);
private:
	UPROPERTY(meta=(BindWidget))//����ͼ������ť���ӵ�C++�İ�ť����,C++������Ҫ��ȫһ��
	class UButton* HostButton;
	UPROPERTY(meta = (BindWidget))
	UButton* JoinButton;
	//���ð�ť�ص�
	UFUNCTION()
	void HostButtonClicked();
	UFUNCTION()
	void JoinButtonClicked();

	void MenuTearDown();
	//the subsystem designed to handle all online session functionality  ���˻Ự��ϵͳ����
	class UMultiplayerSessionsSubsystem* MultiplayerSessionSubsystem;

	int32 NumPublicConnections{ 4 };
	FString MatchType{ TEXT("FreeForAll") };

	FString PathToLobby{ TEXT("") };
};
