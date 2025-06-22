#include "cheats.h"
#include "imgui.h"
#include <string>
#include <vector>
#include <algorithm> 
#include <Windows.h> 


namespace Cheats
{
	bool bShowActorList = true; 
	std::vector<ActorInfo> g_ActorInfos;
	std::mutex g_ActorMutex;
	int g_CurrentPage = 0; 


	void UpdateActors()
	{
		SDK::UWorld* World = SDK::UWorld::GetWorld();
		if (!World || World->Levels.Num() == 0)
		{
			std::lock_guard<std::mutex> lock(g_ActorMutex);
			if (!g_ActorInfos.empty())
			{
				g_ActorInfos.clear();
			}
			return;
		}

		std::vector<ActorInfo> newActorInfos;
		newActorInfos.reserve(4096);

		for (SDK::ULevel* Level : World->Levels)
		{
			if (Level && Level->Actors.Num() > 0)
			{
				for (SDK::AActor* Actor : Level->Actors)
				{
					if (Actor && !Actor->IsActorBeingDestroyed())
					{
						ActorInfo info;
						info.Address = reinterpret_cast<uintptr_t>(Actor);
						info.Name = Actor->GetName();
						newActorInfos.push_back(info);
					}
				}
			}
		}

		std::lock_guard<std::mutex> lock(g_ActorMutex);
		g_ActorInfos.swap(newActorInfos);
	}

	void GameLoop()
	{
		if (!bShowActorList)
		{
			return;
		}

		std::vector<ActorInfo> actorInfosCopy;
		{
			std::lock_guard<std::mutex> lock(g_ActorMutex);
			actorInfosCopy = g_ActorInfos;
		}

		ImGui::SetNextWindowSize(ImVec2(520, 450), ImGuiCond_FirstUseEver);
		ImGui::Begin("Actor List");

		if (actorInfosCopy.empty())
		{
			ImGui::Text("No actors found or world not loaded.");
		}
		else
		{
			const int itemsPerPage = 100;
			const int totalItems = actorInfosCopy.size();
			int maxPage = (totalItems > 0) ? ((totalItems - 1) / itemsPerPage) : 0;

			if (g_CurrentPage > maxPage) g_CurrentPage = maxPage;
			if (g_CurrentPage < 0) g_CurrentPage = 0;

			if (ImGui::Button("Previous"))
			{
				if (g_CurrentPage > 0) g_CurrentPage--;
			}
			ImGui::SameLine();
			if (ImGui::Button("Next"))
			{
				if (g_CurrentPage < maxPage) g_CurrentPage++;
			}
			ImGui::SameLine();
			ImGui::Text("Page %d / %d (%d actors)", g_CurrentPage + 1, maxPage + 1, totalItems);
			ImGui::Separator();

			if (ImGui::BeginTable("ActorTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY))
			{
				ImGui::TableSetupColumn("Address", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("Name");
				ImGui::TableHeadersRow();

				const int pageStart = g_CurrentPage * itemsPerPage;
				const int pageEnd = (std::min)(pageStart + itemsPerPage, totalItems);

				ImGuiListClipper clipper;
				clipper.Begin(pageEnd - pageStart);
				while (clipper.Step())
				{
					for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
					{
						const int itemIndex = pageStart + i;
						if (itemIndex < actorInfosCopy.size()) 
						{
							const auto& info = actorInfosCopy[itemIndex];

							ImGui::TableNextRow();

							ImGui::TableSetColumnIndex(0);
							ImGui::Text("0x%p", (void*)info.Address);

							ImGui::TableSetColumnIndex(1);
							ImGui::Text("%s", info.Name.c_str());
						}
					}
				}
				clipper.End();

				ImGui::EndTable();
			}
		}
		ImGui::End();
	}
}
