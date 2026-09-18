// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarComponent.h"

#include "HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthBarPercentage(float percentage)
{
	if (HealthBarWidget == nullptr)
	{
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}

	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{
		HealthBarWidget->HealthBar->SetPercent(percentage);
	}
}
