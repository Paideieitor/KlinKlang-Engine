#include "Project.h"

#include "FileUtils.h"

#include "KlinFormat.h"

#include "Log.h"

ReturnState LoadProjectSettings(Project& project, const string& name)
{
	project.name = name;

	project.path = string(PROJECTS_PATH) + PATH_SEPARATOR + project.name;
	if (IsFilePath(project.path))
	{
		Log(WARNING, "Project %s is not a folder", project.name.c_str());
		return ERROR;
	}

	project.settingsPath = project.path + PATH_SEPARATOR + SETTINGS_NAME + KLIN_TERMINATION;
	if (!PathExists(project.settingsPath))
	{
		Log(WARNING, "Could not find project file (%s)", project.settingsPath.c_str());
		return ERROR;
	}

	KlinHandler klin;
	LoadKlin(klin, project.settingsPath);
	if (!klin)
	{
		Log(WARNING, "Could not load project file (%s)", project.settingsPath.c_str());
		return STOP;
	}

	if (!GetKlinValueU32(klin, KLIN_PROJECT_ORDER, project.order))
	{
		Log(WARNING, "Could not find project order in project file (%s)", project.settingsPath.c_str());
		project.order = ~0;
	}

	if (!GetKlinValueString(klin, KLIN_CTRMAP_PROJECT_PATH, project.ctrMapProjectPath))
	{
		Log(WARNING, "Could not find project ctrmap path in project file (%s)", project.settingsPath.c_str());
		ReleaseKlin(klin);
		return STOP;
	}

	if (!GetKlinValueString(klin, KLIN_ROM_PATH, project.romPath))
	{
		Log(WARNING, "Could not find project rom path in project file (%s)", project.settingsPath.c_str());
		ReleaseKlin(klin);
		return STOP;
	}

	GetKlinValueU32(klin, KLIN_WINDOW_WIDTH, project.width);
	GetKlinValueU32(klin, KLIN_WINDOW_HEIGHT, project.height);
	GetKlinValueString(klin, KLIN_FONT, project.font);
	GetKlinValueU32(klin, KLIN_FONT_SIZE, project.fontSize);

	GetKlinValueU32(klin, KLIN_LEARNSET_SIZE, project.learnsetSize);
	GetKlinValueU32(klin, KLIN_EVOLUTION_SIZE, project.evolutionSize);
	GetKlinValueU32(klin, KLIN_POKEMON_COUNT, project.pokemonCount);
	GetKlinValueU32(klin, KLIN_TYPE_COUNT, project.typeCount);

	GetKlinValueU32(klin, KLIN_GROUP, project.group);

	GetKlinValueU32(klin, KLIN_SELECTED_PKM_IDX, project.selectedPkmIdx);
	GetKlinValueU32(klin, KLIN_SELECTED_PKM_FORM, project.selectedPkmForm);
	GetKlinValueU32(klin, KLIN_SEARCH_POKESTUDIO, project.showPokeStudio);

	GetKlinValueU32(klin, KLIN_MAX_EVENTS, project.maxEvents);

	GetKlinValueString(klin, KLIN_COMPILER_PATH, project.compilerPath);
	GetKlinValueString(klin, KLIN_JAVA_PATH, project.javaPath);
	GetKlinValueString(klin, KLIN_CTRMAP_PATH, project.ctrMapPath);
	GetKlinValueString(klin, KLIN_EXTLIB_PATH, project.extLibPath);
	GetKlinValueString(klin, KLIN_LIBRPM_PATH, project.libRPMPath);
	GetKlinValueString(klin, KLIN_NK_PATH, project.nkPath);
	GetKlinValueString(klin, KLIN_SWAN_PATH, project.swanPath);

	GetKlinListString(klin, KLIN_ENABLED_PATCHES, project.enabledPatches);

	ReleaseKlin(klin);
    return OK;
}

ReturnState SaveProjectSettings(const Project& project)
{
	KlinHandler klin;
	LoadKlin(klin, project.settingsPath);
	if (!klin)
	{
		Log(WARNING, "Could not save project file (%s)", project.settingsPath.c_str());
		return ERROR;
	}

	SetKlinValueString(klin, KLIN_PROJECT_NAME, project.name);
	SetKlinValueU32(klin, KLIN_PROJECT_ORDER, project.order);

	SetKlinValueString(klin, KLIN_CTRMAP_PROJECT_PATH, project.ctrMapProjectPath);
	SetKlinValueString(klin, KLIN_ROM_PATH, project.romPath);

	if (project.width != DEFAULT_WINDOW_WIDTH)
		SetKlinValueU32(klin, KLIN_WINDOW_WIDTH, project.width);
	if (project.height != DEFAULT_WINDOW_HEIGHT)
		SetKlinValueU32(klin, KLIN_WINDOW_HEIGHT, project.height);
	if (project.font != DEFAULT_FONT)
		SetKlinValueString(klin, KLIN_FONT, project.font);
	if (project.fontSize != DEFAULT_FONT_SIZE)
		SetKlinValueU32(klin, KLIN_FONT_SIZE, project.fontSize);

	if (project.learnsetSize != DEFAULT_LEARNSET_SIZE)
		SetKlinValueU32(klin, KLIN_LEARNSET_SIZE, project.learnsetSize);
	if (project.evolutionSize != DEFAULT_EVOLUTION_SIZE)
		SetKlinValueU32(klin, KLIN_EVOLUTION_SIZE, project.evolutionSize);
	if (project.pokemonCount != DEFAULT_POKEMON_COUNT)
		SetKlinValueU32(klin, KLIN_POKEMON_COUNT, project.pokemonCount);
	if (project.typeCount != DEFAULT_TYPE_COUNT)
		SetKlinValueU32(klin, KLIN_TYPE_COUNT, project.typeCount);

	if (project.group != DEFAULT_GROUP)
		SetKlinValueU32(klin, KLIN_GROUP, project.group);

	if (project.selectedPkmIdx != DEFAULT_SELECTED_PKM_IDX)
		SetKlinValueU32(klin, KLIN_SELECTED_PKM_IDX, project.selectedPkmIdx);
	if (project.selectedPkmForm != DEFAULT_SELECTED_PKM_FORM)
		SetKlinValueU32(klin, KLIN_SELECTED_PKM_FORM, project.selectedPkmForm);
	if (project.showPokeStudio != DEFAULT_SHOW_POKESTUDIO)
		SetKlinValueU32(klin, KLIN_SEARCH_POKESTUDIO, project.showPokeStudio);

	if (project.maxEvents != DEFAULT_MAX_EVENTS)
		SetKlinValueU32(klin, KLIN_MAX_EVENTS, project.maxEvents);

	if (project.compilerPath != DEFAULT_COMPILER_PATH)
		SetKlinValueString(klin, KLIN_COMPILER_PATH, project.compilerPath);
	if (project.javaPath != DEFAULT_JAVA_PATH)
		SetKlinValueString(klin, KLIN_JAVA_PATH, project.javaPath);
	if (project.ctrMapPath != DEFAULT_CTRMAP_PATH)
		SetKlinValueString(klin, KLIN_CTRMAP_PATH, project.ctrMapPath);
	if (project.extLibPath != DEFAULT_EXTLIB_PATH)
		SetKlinValueString(klin, KLIN_EXTLIB_PATH, project.extLibPath);
	if (project.libRPMPath != DEFAULT_LIBRPM_PATH)
		SetKlinValueString(klin, KLIN_LIBRPM_PATH, project.libRPMPath);
	if (project.nkPath != DEFAULT_NK_PATH)
		SetKlinValueString(klin, KLIN_NK_PATH, project.nkPath);
	if (project.swanPath != DEFAULT_SWAN_PATH)
		SetKlinValueString(klin, KLIN_SWAN_PATH, project.swanPath);

	if (project.enabledPatches.size())
		SetKlinListString(klin, KLIN_ENABLED_PATCHES, project.enabledPatches);

	SaveKlin(klin, project.settingsPath, true);
	return OK;
}
