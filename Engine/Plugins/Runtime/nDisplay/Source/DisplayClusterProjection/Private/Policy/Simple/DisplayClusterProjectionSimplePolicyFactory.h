// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Render/Projection/IDisplayClusterProjectionPolicyFactory.h"


/**
 * Implements projection policy factory for the 'simple' policy
 */
class FDisplayClusterProjectionSimplePolicyFactory
	: public IDisplayClusterProjectionPolicyFactory
{
public:
	//////////////////////////////////////////////////////////////////////////////////////////////
	// IDisplayClusterProjectionPolicyFactory
	//////////////////////////////////////////////////////////////////////////////////////////////
	virtual TSharedPtr<IDisplayClusterProjectionPolicy> Create(const FString& PolicyType, const FString& RHIName, const FString& ViewportId, const TMap<FString, FString>& Parameters) override;
};
