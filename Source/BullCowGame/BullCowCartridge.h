// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Console/Cartridge.h"
#include "BullCowCartridge.generated.h"

struct FBullCowCount {
	int32 Cows = 0;
	int32 Bulls = 0;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BULLCOWGAME_API UBullCowCartridge : public UCartridge
{
	GENERATED_BODY()

	public:
	virtual void BeginPlay() override;
	virtual void OnInput(const FString& Input) override;
	void SetupGame();
	void EndGame();
	void ProcessGuess(const FString& Guess);
	static bool IsIsogram(const FString& Word);
	TArray<FString> GetValidWords(const TArray<FString>& Words) const;
	FBullCowCount GetBullCows(const FString& Guess) const;
	void LoadPreviousGuesses();

	private:
	FString HiddenWord;
	int32 NumLives;
	bool bGameOver;
	TArray<FString> PreviousGuesses;
	//TArray<FString> Words;
	TArray<FString> Isograms;
	
};
