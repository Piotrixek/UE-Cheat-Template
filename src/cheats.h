#pragma once

#include "SDK.hpp"
#include <vector>
#include <mutex>
#include <string>


namespace Cheats
{
	struct ActorInfo
	{
		uintptr_t Address;
		std::string Name;
	};

	extern bool bShowActorList;

	extern std::vector<ActorInfo> g_ActorInfos;
	extern std::mutex g_ActorMutex;


	void UpdateActors();

	void GameLoop();
}
