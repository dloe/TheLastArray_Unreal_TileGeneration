// Fill out your copyright notice in the Description page of Project Settings.

//#include "Blueprint/UserWidget.h"
#include "Widgets/SWorldUserWidget.h"
#include <Runtime/Engine/Classes/Kismet/GameplayStatics.h>
#include <Runtime/UMG/Public/Blueprint/WidgetLayoutLibrary.h>
#include "Components/SizeBox.h"

/// <summary>
/// The Last Array - Unreal
/// World User Widget Script
/// Dylan Loe
/// 
/// Last Updated: 10/8/23
/// 
/// Notes:
///  - 
///     
/// </summary>

void USWorldUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	//if our enemy dies the ui might not have an atached actor anymore and when garbage collection kicks in, it will cause a crash
	if (!IsValid(AttachedActor))
	{
		RemoveFromParent();

		UE_LOG(LogTemp, Warning, TEXT("Attached no longer valid, removing Health Widget..."));
		return;
	}

	FVector2D ScreenPosition;
	bool bIsOnScreen = UGameplayStatics::ProjectWorldToScreen(GetOwningPlayer(), AttachedActor->GetActorLocation() + WorldOffset, ScreenPosition);

	if (bIsOnScreen)
	{
		//make sure the scale is right
		float Scale = UWidgetLayoutLibrary::GetViewportScale(this);

		ScreenPosition /= Scale;

		if (ParentSizeBox)
		{
			ParentSizeBox->SetRenderTranslation(ScreenPosition);
		}
	}

	if (ParentSizeBox)
	{
		ParentSizeBox->SetVisibility(bIsOnScreen ? ESlateVisibility::HitTestInvisible : ESlateVisibility::Collapsed);
	}

}
