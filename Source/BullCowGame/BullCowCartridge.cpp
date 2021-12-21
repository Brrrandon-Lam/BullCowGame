// Fill out your copyright notice in the Description page of Project Settings.
#include "BullCowCartridge.h"
// #include "Math/UnrealMathUtility.h"

//Text() changes our string from ANSI to TCHAR.
//Note that FString itself does not need the TEXT() macro. Just the string literals.


//FPaths::ProjectContentDir() will return the absolute path to the Content directory e.g.
 //The / operator has been overloaded to concatenate the two strings together ensuring a / inbetween them. 
//FFileHelper::LoadFileToStringArray loads a text file into a TArray of FStrings.
//The first argument is the TArray to populate and the second is the path to the file.
    
/*
you would need to add the WordLists directory to the list of 
Additional Non-Asset Directories to package in your Project Settings under Packaging.
*/
void UBullCowCartridge::BeginPlay() // When the game starts
{
    Super::BeginPlay();
    //Get words from text file and pass them into an array. We would need to process the isograms out of this list.
    const FString WordListPath = FPaths::ProjectContentDir() / TEXT("WordLists/HiddenWordList.txt");
    //FFileHelper::LoadFileToStringArray(Words, *WordListPath); //This is how we would load all of the words into it but there is a UE 4.25+ alternative.

    //use lambda function to process the text file on word length and isogram check
    FFileHelper::LoadFileToStringArrayWithPredicate(Isograms, *WordListPath, [](const FString& Word)
    {
        return Word.Len() >= 4 && Word.Len() <= 8 && IsIsogram(Word);
    });

    PrintLine(FString::Printf(TEXT("The number of possible words is %i"), Isograms.Num()));
    //Isograms = GetValidWords(Words);
    SetupGame();
}

void UBullCowCartridge::OnInput(const FString& PlayerInput) // When the player hits enter
{
    //Whenever the player makes a guess, add it to a running list.
    if(PlayerInput != (TEXT("Guesses"))) { //avoid adding the keyword to the list.
        PreviousGuesses.AddUnique(PlayerInput); //add the previous guess to this list
    }
     if(bGameOver) {
        ClearScreen(); //Clears the terminal
        PreviousGuesses.Empty();
        SetupGame(); //If the game is over, do clear screen and set up the game
    }
    else {
        ProcessGuess(PlayerInput);
    }
}

void UBullCowCartridge::SetupGame()
{
    int32 index = FMath::RandRange(0, Isograms.Num() - 1);
    HiddenWord = Isograms[index]; // Using GetValidWords we can get a random word from the list
    NumLives = HiddenWord.Len() * 2; //init NumLives as the length of the hidden word * 2
    bGameOver = false; //set game over as initially false.

    //Welcoming Messages
    PrintLine(TEXT("Welcome to the Bull Cow Game!"));
    PrintLine(FString::Printf(TEXT("Guess the %i letter isogram"), HiddenWord.Len()));
    PrintLine(FString::Printf(TEXT("You have %i lives to begin with."), NumLives));
    PrintLine(FString::Printf(TEXT("The hidden word is: %s."), *HiddenWord)); //debug line
    PrintLine(TEXT("Type in your guess and press Enter to continue..."));
}

void UBullCowCartridge::EndGame() 
{
    bGameOver = true;
    PrintLine(TEXT("The game is over! Press Enter to continue"));
    return;
}

void UBullCowCartridge::ProcessGuess(const FString& Guess)
{
    //If the user types in Guesses, print out their previous guesses.
    if(Guess == (TEXT("Guesses"))) {
        for(int32 i = 0; i < PreviousGuesses.Num(); i++) {
            PrintLine(FString::Printf(TEXT("%s"), *PreviousGuesses[i]));
        }
        return;
    }
    if(Guess == HiddenWord) { //if the word is correct
        PrintLine(TEXT("You win!"));
        EndGame();
        return;
    }
    else { //if the word is incorrect
        PrintLine(FString::Printf(TEXT("Wrong! You have %i lives remaining"), --NumLives));
        if(Guess.Len() != HiddenWord.Len() && NumLives > 0) { //if it is of the wrong length
            PrintLine(FString::Printf(TEXT("The hidden word is %i characters long."), HiddenWord.Len()));
        }
        if(!IsIsogram(Guess)) {
            PrintLine(TEXT("This word has repeating letters. Not an isogram."));
        }
        if(NumLives <= 0) {
            PrintLine(TEXT("You have no lives left"));
            PrintLine(FString::Printf(TEXT("The hidden word was: %s."), *HiddenWord));
            EndGame();
        }
    }
    FBullCowCount Score = GetBullCows(Guess);
    PrintLine(FString::Printf(TEXT("You have %i Bulls and %i Cows."), Score.Bulls, Score.Cows));
}

//This function does not modify any member variables, so we can make it const.
bool UBullCowCartridge::IsIsogram(const FString& Word)
{
    for(int32 i = 0; i < Word.Len() - 1; i++) {
        for(int32 j = i + 1; j < Word.Len(); j++) {
            if(Word[i] == Word[j]) {
                return false;
            }
        }
    }
    return true;
}

//When you keep copying stuff, you will run out of memory extremely quickly, so just don't. Use pass by reference.
TArray<FString> UBullCowCartridge::GetValidWords(const TArray<FString> &WordList) const
{
    TArray<FString> ValidWords;
    for(FString Word : WordList) {
        //check that the word is an isogram and that it is within a certain length.
        if(Word.Len() >= 4 && Word.Len() <= 8 && IsIsogram(Word)) {
            ValidWords.Emplace(Word);
        }
    }
    return ValidWords;
}

FBullCowCount UBullCowCartridge::GetBullCows(const FString& Guess) const
{
    FBullCowCount Count; //initialize struct
    //For every index guess, if the index of the Guess is the same as the index of the hidden word, increment the bull count.
    //if not a bull, was it a cow? if yes, increment the cow count.
    for(int32 i = 0; i < Guess.Len(); i++) {
        int32 FindIndex; //An out parameter that will pass the index from FindChar
        //find char searches the string for a character and returns true if the character was found in a string, false otherwise.
        //Also stores the index it was found in
        //bool FindChar(TCHAR InChar, int32& index) const
        if(HiddenWord.FindChar(Guess[i], FindIndex)) {
            if(FindIndex == i) {
                ++Count.Bulls;
            }
            else {
                ++Count.Cows;
            }
        }
    }
    return Count;
}