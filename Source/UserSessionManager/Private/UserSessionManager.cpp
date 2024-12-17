// Copyright (c) 2024 Hunan MangoXR Tech Co., Ltd. All Rights Reserved.

#include "UserSessionManager.h"

void FUserSessionManager::StartupModule()
{
	IModuleInterface::StartupModule();
	
}

void FUserSessionManager::ShutdownModule()
{
	IModuleInterface::ShutdownModule();
	
}


IMPLEMENT_MODULE(FUserSessionManager, UserSessionManager)