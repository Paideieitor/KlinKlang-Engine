#include "nativefiledialog/nfd.h"

#include "Patcher.h"

#include "Engine.h"

Patcher::Patcher(Engine* const engine, u32 group) : Module(engine, group, "")
{
	engine->patchesModuleIdx = moduleIdx;
	engine->LoadPatches(false);
}

Patcher::~Patcher()
{
}

ReturnState Patcher::RenderGUI()
{
	if (engine->patcherSettingsMenu && 
		ImGui::Begin("Build Settings", &engine->patcherSettingsMenu))
	{
		TextInput(&engine->project->compilerPath, "Compiler Path", 1);
		TextInput(&engine->project->javaPath, "Java Path", 2);
		TextInput(&engine->project->ctrMapPath, "CTRMap Path", 3);
		TextInput(&engine->project->swanPath, "SWAN Path", 7);
		TextInput(&engine->project->nkPath, "NitroKernel Path", 6);
		TextInput(&engine->project->extLibPath, "ExtLib Path", 4);
		TextInput(&engine->project->libRPMPath, "LibRPM Path", 5);

		ImGui::End();
	}

	for (u32 patchIdx = 0; patchIdx < (u32)engine->patches.size(); ++patchIdx)
	{
		Patch& patch = engine->patches[patchIdx];
		if (patch.open)
		{
			ImGui::Begin(patch.name.c_str(), &patch.open);
			PatchMenu(&patch);
			ImGui::End();
		}
	}


	return OK;
}

void Patcher::HandleReverseEvent(const Event* reverseEvent)
{
	switch (reverseEvent->subType)
	{
	case 0:
	{
		bool* value = (bool*)reverseEvent->value;
		break;
	}
	default:
	{
		KlangVar* var = (KlangVar*)reverseEvent->subType;
		int* value = (int*)reverseEvent->value;
		var->SetValue(*value);
		break;
	}
	}
}

void Patcher::PatchMenu(Patch* patch)
{
	CheckBox(patch, "Enabled");

	for (u32 varIdx = 0; varIdx < (u32)patch->settings.vars.size(); ++varIdx)
	{
		KlangVar& var = patch->settings.vars[varIdx];
		InputInt(&var, var.name.c_str());

		for (u32 depenIdx = 0; depenIdx < (u32)var.dependentVars.size(); ++depenIdx)
		{
			KlangVar& depenVar = var.dependentVars[depenIdx];
			ImGui::Indent(25.0f);
			InputInt(&depenVar, depenVar.name.c_str());
			ImGui::Unindent(25.0f);
		}
	}
}

void Patcher::TextInput(string* text, const char* label, u32 eventID)
{
	ImGui::Text(label);
	string previous = *text;
	if (ImGui::InputText((string("##") + label).c_str(), &previous))
	{
		SIMPLE_REVERSE_EVENT(eventID, *text, previous);
		*text = previous;
	}

	ImGui::SameLine();

	if (ImGui::Button((string("...##") + label).c_str()))
	{
		char* outPath = NULL;
		nfdresult_t result = NFD_PickFolder(NULL, &outPath);
		
		switch (result)
		{
		case NFD_ERROR:
			Log(CRITICAL, "%s", NFD_GetError());
			break;
		case NFD_OKAY:
		{
			string newPath(outPath);

			SIMPLE_REVERSE_EVENT(eventID, *text, newPath);
			*text = newPath;
			break;
		}
		case NFD_CANCEL:
			break;
		}
	}
}

void Patcher::InputInt(KlangVar* var, const char* label)
{
	int value = var->Value();
	ImGui::SetNextItemWidth(100.0f);
	if (ImGui::InputInt(LABEL(label, (int)var), &value))
	{
		int oldValue = var->Value();
		if (var->SetValue(value))
			SIMPLE_REVERSE_EVENT((u32)var, oldValue, value);
	}
}

void Patcher::CheckBox(Patch* patch, const char* label)
{
	bool value = patch->enabled;
	if (ImGui::Checkbox(label, &value))
	{
		SIMPLE_REVERSE_EVENT((u32)nullptr, patch->enabled, value);
		patch->enabled = (int)value;
	}
}

