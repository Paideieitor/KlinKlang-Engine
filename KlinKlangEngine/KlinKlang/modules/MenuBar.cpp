#include "MenuBar.h"

#include "Engine.h"

MenuBar::MenuBar(Engine* const engine, u32 group) : Module(engine, group, "")
{
}

MenuBar::~MenuBar()
{
}

ReturnState MenuBar::RenderGUI()
{
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
		engine->commandInput = true;
	if (ImGui::IsKeyReleased(ImGuiKey_LeftCtrl))
		engine->commandInput = false;

	if (engine->commandInput &&
		ImGui::IsKeyPressed(ImGuiKey_S))
		engine->Save();

	if (engine->commandInput &&
		ImGui::IsKeyPressed(ImGuiKey_Z))
		engine->HandleReverseEvent();

	if (engine->commandInput &&
		ImGui::IsKeyPressed(ImGuiKey_1))
		ChangeGroup(POKEMON_GROUP);
	if (engine->commandInput &&
		ImGui::IsKeyPressed(ImGuiKey_2))
		ChangeGroup(MOVE_GROUP);
	if (engine->commandInput &&
		ImGui::IsKeyPressed(ImGuiKey_3))
		ChangeGroup(ENCOUNTER_GROUP);
	if (engine->commandInput &&
		ImGui::IsKeyPressed(ImGuiKey_4))
		ChangeGroup(TRAINER_GROUP);

	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Project"))
		{
			if (ImGui::MenuItem("Save", "Ctrl+S"))
				engine->Save();

			if (ImGui::MenuItem("Undo", "Ctrl+Z"))
				engine->HandleReverseEvent();

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Editors"))
		{
			if (ImGui::MenuItem(u8"Pokémon", "Ctrl+1"))
				ChangeGroup(POKEMON_GROUP);
			if (ImGui::MenuItem("Moves", "Ctrl+2"))
				ChangeGroup(MOVE_GROUP);
			if (ImGui::MenuItem("Encounters", "Ctrl+3"))
				ChangeGroup(ENCOUNTER_GROUP);
			if (ImGui::MenuItem("Trainers", "Ctrl+4"))
				ChangeGroup(TRAINER_GROUP);

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	return OK;
}

void MenuBar::HandleReverseEvent(const Event* reverseEvent)
{
	switch (reverseEvent->subType)
	{
	case 0:
		u32* value = (u32*)reverseEvent->value;
		engine->project->group = *value;
		break;
	}
}

void MenuBar::ChangeGroup(u32 newGroup)
{
	SIMPLE_REVERSE_EVENT(0, engine->project->group, newGroup);
	engine->project->group = newGroup;
}
