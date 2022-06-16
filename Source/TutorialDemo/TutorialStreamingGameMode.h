/* © Copyright 2022 Graham Chabas, All Rights Reserved. */


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "TutorialStreamingGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TUTORIALDEMO_API ATutorialStreamingGameMode : public AGameModeBase
{
public:
	virtual void StartPlay() override;
	virtual void StartToLeaveMap() override;


protected:


private:

	UFUNCTION()
	void SubLevelLoaded();
	
	UFUNCTION()
	void SubLevelUnloaded();

	GENERATED_BODY()
};
