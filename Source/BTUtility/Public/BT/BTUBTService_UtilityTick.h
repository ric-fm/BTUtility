// Copyright (c) 2026 Ricardo Franco.
// Licensed under the MIT License. See LICENSE file in the project root for full license information.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTUBTService_UtilityTick.generated.h"

class UBTUBTCompositeNode_UtilitySelector;


/**
 * @class UBTUBTService_UtilityTick
 *	@see UBTUBTCompositeNode_UtilitySelector
 *	
 * A specialized service meant to be placed on a UBTUBTCompositeNode_UtilitySelector.
 * It periodically triggers utility re-evaluations to allow reactive branch switching
 * based on dynamic utility scores.
 */
UCLASS(DisplayName = "Utility Tick", Category="Utility")
class BTUTILITY_API UBTUBTService_UtilityTick : public UBTService
{
	GENERATED_BODY()
	
public:
	UBTUBTService_UtilityTick(const FObjectInitializer& ObjectInitializer);
	
#pragma region UBTService
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
#pragma endregion
	
private:
	/* Helper to geta and cache te UBTCompositeNode_UtilitySelector this service is attached to. */ 
	const UBTUBTCompositeNode_UtilitySelector* GetUtilitySelectorNode();
	
	/** Cached reference to the selector node this service is attached to. */
	UPROPERTY(Transient)
	TObjectPtr<const UBTUBTCompositeNode_UtilitySelector> CachedUtilitySelector;
};
