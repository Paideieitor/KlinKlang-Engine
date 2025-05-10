#include "algorithm"

#include "Engine.h"

#include "Project.h"
#include "Module.h"

#include "FileUtils.h"
#include "NarcUtils.h"
#include "StringUtils.h"

#include "Alle5Format.h"

#include "MenuBar.h"
#include "Patcher.h"
#include "PokemonSearch.h"
#include "PokemonForm.h"
#include "PokemonText.h"
#include "Personal.h"
#include "Learnset.h"
#include "Evolution.h"
#include "Child.h"

#include "Log.h"

Engine::Engine(Project* const project) : project(project)
{
	string fsPath = project->path + PATH_SEPARATOR + FILESYSTEM_NAME;
	if (PathExists(fsPath) && 
		RemoveFolder(fsPath) == -1)
	{
		Log(CRITICAL, "Error removing temp forlder!");
		return;
	}

	if (!Start())
	{
		Log(CRITICAL, "Error starting Engine!");
		return;
	}

	// Engine
	modules.emplace_back(new MenuBar(this, ENGINE_GROUP));
	modules.emplace_back(new Patcher(this, ENGINE_GROUP));
	// Pokémon
	modules.emplace_back(new PokemonSearch(this, POKEMON_GROUP));
	modules.emplace_back(new PokemonForm(this, POKEMON_GROUP));
	modules.emplace_back(new PokemonText(this, POKEMON_GROUP));
	modules.emplace_back(new Personal(this, POKEMON_GROUP));
	modules.emplace_back(new Learnset(this, POKEMON_GROUP));
	modules.emplace_back(new Evolution(this, POKEMON_GROUP));
	modules.emplace_back(new Child(this, POKEMON_GROUP));
}

Engine::~Engine()
{
	for (u32 idx = 0; idx < (u32)modules.size(); ++idx)
		delete modules[idx];

	for (u32 idx = 0; idx < (u32)reverseEvents.size(); ++idx)
		delete reverseEvents[idx].value;
}

ReturnState Engine::RenderGUI()
{
	if (GetCurrentPokemon() == nullptr)
		return ERROR;

	project->width = (u32)width;
	project->height = (u32)height;

	ReturnState moduleState = OK;
	for (u32 idx = 0; idx < (u32)modules.size(); ++idx)
	{
		if (modules[idx]->group == ENGINE_GROUP ||
			modules[idx]->group == project->group)
		{
			moduleState = modules[idx]->RenderGUI();
			if (moduleState != OK)
				break;
		}
	}
		
	return moduleState;
}

void Engine::SetCurrentPokemon(u32 idx, u32 form)
{
	if (project->selectedPkmIdx != idx)
		form = 0;

	project->selectedPkmIdx = idx;
	project->selectedPkmForm = form;

	Pokemon* pkm = &(pokemon[idx]);
	if (project->selectedPkmForm > 0)
		pkm = &(pkm->forms[form - 1]);
	currentPkm = pkm;
}

bool Engine::LoadTextFiles()
{
	vector<u16> missingTextFiles;

	// The path where the text files are stored in the CTRMap project
	string ctrTextNarcPath = project->ctrMapProjectPath + PATH_SEPARATOR + CTRMAP_FILESYSTEM_PATH + PATH_SEPARATOR + TEXT_NARC_PATH;
	// The path to the text NARC file in the extracted ROM files
	string romTextNarcPath = project->romPath + PATH_SEPARATOR + ROM_FILESYSTEM_PATH + PATH_SEPARATOR + TEXT_NARC_PATH;
	// Create the path where the text files will be stored
	string textNarcPath = project->path + PATH_SEPARATOR + FILESYSTEM_NAME + PATH_SEPARATOR + TEXT_NARC_PATH;

	// If the file is not extracted in the CTRMap project
	// add all the needed text files to the NARC extraction list
	if (!PathExists(ctrTextNarcPath))
	{
		if (!PathExists(romTextNarcPath) || !IsFilePath(romTextNarcPath))
		{
			Log(CRITICAL, "Couldn't load project %s, game text NARC not found", project->name.c_str());
			return false;
		}
		
		missingTextFiles.emplace_back(PKM_NAME_FILE_ID);
		missingTextFiles.emplace_back(PKM_FORM_NAME_FILE_ID);
		missingTextFiles.emplace_back(PKM_DESCRIPTION_FILE_ID);
		missingTextFiles.emplace_back(PKM_TITLE_FILE_ID);

		missingTextFiles.emplace_back(MOVE_NAME_FILE_ID);
		missingTextFiles.emplace_back(MOVE_DESCRIPTION_FILE_ID);

		missingTextFiles.emplace_back(TYPE_NAME_FILE_ID);

		missingTextFiles.emplace_back(ABILITY_NAME_FILE_ID);

		missingTextFiles.emplace_back(ITEM_NAME_FILE_ID);
	}

	// The file where the Pokémon text data is stored
	string pkmNamePath = MAKE_FILE_PATH(textNarcPath, PKM_NAME_FILE_ID);
	string formNamePath = MAKE_FILE_PATH(textNarcPath, PKM_FORM_NAME_FILE_ID);
	string pkmDescriptionPath = MAKE_FILE_PATH(textNarcPath, PKM_DESCRIPTION_FILE_ID);
	string pkmTitlePath = MAKE_FILE_PATH(textNarcPath, PKM_TITLE_FILE_ID);

	// The file where the Move text data is stored
	string moveNamePath = MAKE_FILE_PATH(textNarcPath, MOVE_NAME_FILE_ID);
	string moveDescriptionPath = MAKE_FILE_PATH(textNarcPath, MOVE_DESCRIPTION_FILE_ID);

	// The file where the Type text data is stored
	string typeNamePath = MAKE_FILE_PATH(textNarcPath, TYPE_NAME_FILE_ID);

	// The file where the Abilities text data is stored
	string abilityNamePath = MAKE_FILE_PATH(textNarcPath, ABILITY_NAME_FILE_ID);

	// The file where the Item text data is stored
	string itemNamePath = MAKE_FILE_PATH(textNarcPath, ITEM_NAME_FILE_ID);

	// If the missing files are not already set
	if (!missingTextFiles.size())
	{
		// Check if the files we need are already extracted in the CTRMap project
		// If they are not, add them to the missing files list

		string ctrPkmNamePath = MAKE_FILE_PATH(ctrTextNarcPath, PKM_NAME_FILE_ID);
		if (!PathExists(ctrPkmNamePath))
			missingTextFiles.emplace_back(PKM_NAME_FILE_ID);
		else
			CopyFile(ctrPkmNamePath, pkmNamePath);

		string ctrFormNamePath = MAKE_FILE_PATH(ctrTextNarcPath, PKM_FORM_NAME_FILE_ID);
		if (!PathExists(ctrFormNamePath))
			missingTextFiles.emplace_back(PKM_FORM_NAME_FILE_ID);
		else
			CopyFile(ctrFormNamePath, formNamePath);

		string ctrPkmDescriptionPath = MAKE_FILE_PATH(ctrTextNarcPath, PKM_DESCRIPTION_FILE_ID);
		if (!PathExists(ctrPkmDescriptionPath))
			missingTextFiles.emplace_back(PKM_DESCRIPTION_FILE_ID);
		else
			CopyFile(ctrPkmDescriptionPath, pkmDescriptionPath);

		string ctrPkmTitlePath = MAKE_FILE_PATH(ctrTextNarcPath, PKM_TITLE_FILE_ID);
		if (!PathExists(ctrPkmTitlePath))
			missingTextFiles.emplace_back(PKM_TITLE_FILE_ID);
		else
			CopyFile(ctrPkmTitlePath, pkmTitlePath);

		string ctrMoveNamePath = MAKE_FILE_PATH(ctrTextNarcPath, MOVE_NAME_FILE_ID);
		if (!PathExists(ctrMoveNamePath))
			missingTextFiles.emplace_back(MOVE_NAME_FILE_ID);
		else
			CopyFile(ctrMoveNamePath, moveNamePath);

		string ctrMoveDescriptionPath = MAKE_FILE_PATH(ctrTextNarcPath, MOVE_DESCRIPTION_FILE_ID);
		if (!PathExists(ctrMoveDescriptionPath))
			missingTextFiles.emplace_back(MOVE_DESCRIPTION_FILE_ID);
		else
			CopyFile(ctrMoveDescriptionPath, moveDescriptionPath);

		string ctrTypeNamePath = MAKE_FILE_PATH(ctrTextNarcPath, TYPE_NAME_FILE_ID);
		if (!PathExists(ctrTypeNamePath))
			missingTextFiles.emplace_back(TYPE_NAME_FILE_ID);
		else
			CopyFile(ctrTypeNamePath, typeNamePath);

		string ctrAbilityNamePath = MAKE_FILE_PATH(ctrTextNarcPath, ABILITY_NAME_FILE_ID);
		if (!PathExists(ctrAbilityNamePath))
			missingTextFiles.emplace_back(ABILITY_NAME_FILE_ID);
		else
			CopyFile(ctrAbilityNamePath, abilityNamePath);

		string ctrItemNamePath = MAKE_FILE_PATH(ctrTextNarcPath, ITEM_NAME_FILE_ID);
		if (!PathExists(ctrItemNamePath))
			missingTextFiles.emplace_back(ITEM_NAME_FILE_ID);
		else
			CopyFile(ctrItemNamePath, itemNamePath);
	}

	// Extract the files we are missing
	if (missingTextFiles.size())
	{
		if (!NarcUnpackBundle(romTextNarcPath, textNarcPath, missingTextFiles))
		{
			Log(CRITICAL, "Couldn't load project %s, game text NARC not found", project->name.c_str());
			return false;
		}
	}

	// Load relevant Pokémon text data
	LoadAlle5File(pkmNamePath, pkmNames);
	LoadAlle5File(formNamePath, formNames);
	LoadAlle5File(pkmDescriptionPath, pkmDescriptions);
	LoadAlle5File(pkmTitlePath, pkmTitles);
	// Load relevant Move text data
	LoadAlle5File(moveNamePath, moveNames);
	LoadAlle5File(moveDescriptionPath, moveDescriptions);
	// Load relevant Type text data
	LoadAlle5File(typeNamePath, types);
	// Load relevant Ability text data
	LoadAlle5File(abilityNamePath, abilities);
	// Load relevant Item text data
	LoadAlle5File(itemNamePath, items);

	return true;
}

u32 Engine::LoadDataNarc(const string& narcPath, string& outputPath)
{
	vector<u16> excludeFiles;

	// The path where the files are stored in the CTRMap project
	string ctrNarcPath = project->ctrMapProjectPath + PATH_SEPARATOR + CTRMAP_FILESYSTEM_PATH + PATH_SEPARATOR + narcPath;
	// The path to the NARC file in the extracted ROM
	string romNarcPath = project->romPath + PATH_SEPARATOR + ROM_FILESYSTEM_PATH + PATH_SEPARATOR + narcPath;
	// Create the path where the files will be stored
	outputPath = project->path + PATH_SEPARATOR + FILESYSTEM_NAME + PATH_SEPARATOR + narcPath;

	// If the NARC is partially extracted in the CTRMap project move
	// the extracted files and exclude them from the unpacking
	if (PathExists(ctrNarcPath))
	{
		vector<string> ctrFiles = GetFolderElementList(ctrNarcPath);
		for (u16 ctrIdx = 0; ctrIdx < ctrFiles.size(); ++ctrIdx)
		{
			string ctrFilePath = ctrNarcPath + PATH_SEPARATOR + ctrFiles[ctrIdx];
			if (!PathExists(ctrFilePath) || !IsFilePath(ctrFilePath))
			{
				Log(WARNING, "Couldn't find file %s in %s NARC in CTRMap", ctrFiles[ctrIdx].c_str(), narcPath.c_str());
				continue;
			}
			string engineFilePath = outputPath + PATH_SEPARATOR + ctrFiles[ctrIdx];
			// Copy the already extracted files from the CTRMap project
			if (!CopyFile(ctrFilePath, engineFilePath))
			{
				Log(WARNING, "Couldn't copy %s file in %s NARC", ctrFiles[ctrIdx].c_str(), narcPath.c_str());
				continue;
			}

			excludeFiles.emplace_back((u16)stoi(ctrFiles[ctrIdx]));
		}
	}

	// Create the NARC folder if necesary
	if (PathExists(outputPath))
	{
		vector<string> engineFiles = GetFolderElementList(outputPath);
		for (u16 engineIdx = 0; engineIdx < engineFiles.size(); ++engineIdx)
			excludeFiles.emplace_back((u16)stoi(engineFiles[engineIdx]));
	}
	else if (!CreateFolder(outputPath))
	{
		Log(CRITICAL, "Couldn't create %s NARC folder", narcPath.c_str());
		return 0;
	}

	if (!PathExists(romNarcPath) || !IsFilePath(romNarcPath))
	{
		Log(CRITICAL, "Couldn't load project %s, game text NARC not found", project->name.c_str());
		return 0;
	}

	// Extract the remaining files (excluding the ones found in the CTRMap project)
	int extractedFiles = NarcUnpackExclude(romNarcPath, outputPath, excludeFiles);
	if (extractedFiles < 0)
	{
		Log(CRITICAL, "Couldn't load project %s, data NARC not found", project->name.c_str());
		return 0;
	}

	return (u32)extractedFiles + (u32)excludeFiles.size() + 1;
}

bool Engine::LoadPokemon(Pokemon& pkm)
{
	string personalFilePath = MAKE_FILE_PATH(personalPath, pkm.dataIdx);
	if (!LoadPersonal(personal[pkm.dataIdx], personalFilePath))
	{
		Log(CRITICAL, u8"Pokémon %d lacks personal data", pkm.dataIdx);
		return false;
	}
	string learnsetFilePath = MAKE_FILE_PATH(learnsetPath, pkm.dataIdx);
	if (!LoadLearnset(learnset[pkm.dataIdx], learnsetFilePath))
	{
		Log(CRITICAL, u8"Pokémon %d lacks learnset data", pkm.dataIdx);
		return false;
	}
	string evolutionFilePath = MAKE_FILE_PATH(evolutionPath, pkm.dataIdx);
	if (!LoadEvolution(evolution[pkm.dataIdx], evolutionFilePath))
	{
		Log(CRITICAL, u8"Pokémon %d lacks evolution data", pkm.dataIdx);
		return false;
	}

	// Only non-PokéStar and non-Form Pokémon have a child file
	if (!pkm.pokeStudio && !PokemonIsForm(pkm))
	{
		string childFilePath = MAKE_FILE_PATH(childPath, pkm.idx);
		if (!LoadChild(child[pkm.dataIdx], childFilePath))
		{
			Log(CRITICAL, u8"Pokémon %d lacks child data", pkm.idx);
			return false;
		}
	}

	return true;
}

bool Engine::Start()
{
	// Extract the text data
	if (!LoadTextFiles())
		return false;

	// Extrat the data of every Pokémon
	int loadedFiles = LoadDataNarc(PERSONAL_NARC_PATH, personalPath);
	if (loadedFiles < 0)
		return false;
	personal.reserve(loadedFiles);
	for (u32 idx = 0; idx < (u32)loadedFiles; ++idx)
		personal.push_back(PersonalData());

	loadedFiles = LoadDataNarc(LEARNSET_NARC_PATH, learnsetPath);
	if (loadedFiles < 0)
		return false;
	learnset.reserve(loadedFiles);
	for (u32 idx = 0; idx < (u32)loadedFiles; ++idx)
		learnset.push_back(LearnsetData());

	loadedFiles = LoadDataNarc(EVOLUTION_NARC_PATH, evolutionPath);
	if (loadedFiles < 0)
		return false;
	evolution.reserve(loadedFiles);
	for (u32 idx = 0; idx < (u32)loadedFiles; ++idx)
		evolution.push_back(EvolutionData());

	loadedFiles = LoadDataNarc(CHILD_NARC_PATH, childPath);
	if (loadedFiles < 0)
		return false;
	child.reserve(loadedFiles);
	for (u32 idx = 0; idx < (u32)loadedFiles; ++idx)
		child.push_back(ChildData());

	// Use the loaded files to store all the Pokémon data
	pokemon.reserve(pkmNames.size());
	u32 formCount = 0;
	// Store loaded data in the Pokémon array
	for (u32 pkmIdx = 0; pkmIdx < (u32)pkmNames.size(); ++pkmIdx)
	{
		Pokemon pkm;
		pkm.idx = pkmIdx;
		// Base form Pokémon share their Pokédex, PML and Text indexes
		pkm.dataIdx = pkmIdx;
		pkm.textIdx = pkmIdx;

		// Pokémon names over the Pokédex amount are PokéStudio enemies
		pkm.pokeStudio = (bool)!((u32)pkmIdx < project->pokemonCount);
		// PokéStudio enemies don't have Text index since they don't have a description
		if (pkm.pokeStudio)
			pkm.textIdx = 0;
		
		// Load the data of the Pokémon
		if (!LoadPokemon(pkm))
			return false;

		if (!pkm.pokeStudio)
		{
			// Load the data of the Pokémon forms
			for (u8 form = 0; form < personal[pkmIdx][FORM_COUNT] - 1; ++form)
			{
				Pokemon formPkm;
				formPkm.idx = pkmIdx;

				// Form Pokémon data index is specified in the personal data of the base Pokémon
				formPkm.dataIdx = personal[pkmIdx][FORM_OFFSET] + form;
				// Form Pokémon without data must use the data of the base Pokémon
				if (formPkm.dataIdx == form)
					formPkm.dataIdx = pkm.dataIdx;

				// Decription texts skip the PokéStudion Entries
				// textIdx 650 is empty so we need to add 1
				formPkm.textIdx = project->pokemonCount + formCount + 1;

				// Only non-PokéStudio Pokémon have forms
				formPkm.pokeStudio = false;

				++formCount;

				// Load the data of the Form Pokémon
				if (!LoadPokemon(formPkm))
					return false;

				pkm.forms.push_back(formPkm);
			}
		}
		pokemon.push_back(pkm);
	}

	SetCurrentPokemon(project->selectedPkmIdx, project->selectedPkmForm);
	return true;
}

bool Engine::LoadPersonal(PersonalData& personalData, const string& file)
{
	PersonalReset(personalData);

	FileStream fileStream;
	if (!LoadFileStream(fileStream, file))
		return false;
	u32 currentByte = 0;

	for (u32 stat = 0; stat < STAT_COUNT; ++stat)
		personalData[(u32)BASE_HP + stat] = (int)FileStreamReadUpdate<u8>(fileStream, currentByte);

	for (u32 type = 0; type < TYPE_COUNT; ++type)
		personalData[(u32)TYPE_1 + type] = (int)FileStreamReadUpdate<u8>(fileStream, currentByte);

	personalData[CATCH_RATE] = (int)FileStreamReadUpdate<u8>(fileStream, currentByte);
	personalData[EVOLUTION_STAGE] = (int)FileStreamReadUpdate<u8>(fileStream, currentByte);

	u16 evYield = FileStreamReadUpdate<u16>(fileStream, currentByte);
	u16 mask = 0x0003;
	for (u32 stat = 0; stat < STAT_COUNT; ++stat)
	{
		personalData[(u32)EV_HP + stat] = (u8)((evYield & mask) >> (stat * 2));
		mask = mask << 2;
	}
	personalData[GROUNDED_SPRITE] = (int)((evYield & 0x1000) != 0);

	u16 expandedAbilBits[ABILITY_COUNT];
	for (u32 item = 0; item < WILD_ITEM_COUNT; ++item)
	{
		u16 itemValue = FileStreamReadUpdate<u16>(fileStream, currentByte);
		if (expandedAbilities)
		{
			expandedAbilBits[item] = itemValue & 0xC000;
			// Remove the ability related bits from the item value
			itemValue -= expandedAbilBits[item];
		}
		personalData[(u32)WILD_ITEM_50 + item] = (int)itemValue;
	}

	personalData[SEX_CHANCE] = (int)FileStreamReadUpdate<u8>(fileStream, currentByte);

	personalData[EGG_HAPPINESS] = (int)FileStreamReadUpdate<u8>(fileStream, currentByte);
	personalData[BASE_HAPPINESS] = (int)FileStreamReadUpdate<u8>(fileStream, currentByte);

	personalData[GROWTH_RATE] = (int)FileStreamReadUpdate<u8>(fileStream, currentByte);

	for (u32 eggGroup = 0; eggGroup < EGG_GROUP_COUNT; ++eggGroup)
		personalData[(u32)EGG_GROUP_1 + eggGroup] = (int)FileStreamReadUpdate<u8>(fileStream, currentByte);

	for (u32 ability = 0; ability < ABILITY_COUNT; ++ability)
	{
		u16 abilityValue = FileStreamReadUpdate<u8>(fileStream, currentByte);
		if (expandedAbilities)
		{
			// Add the ability bits stored in the item slot to the ability value
			abilityValue += (expandedAbilBits[ability] >> 6);
		}
		personalData[(u32)ABILITY_1 + ability] = (int)abilityValue;
	}

	personalData[ESCAPE_RATE] = (int)FileStreamReadUpdate<u8>(fileStream, currentByte);

	personalData[FORM_OFFSET] = (int)FileStreamReadUpdate<u16>(fileStream, currentByte);
	personalData[FORM_SPRITE_OFFSET] = (int)FileStreamReadUpdate<u16>(fileStream, currentByte);
	personalData[FORM_COUNT] = (int)FileStreamReadUpdate<u8>(fileStream, currentByte);

	u8 colorValue = FileStreamReadUpdate<u8>(fileStream, currentByte);
	personalData[COLOR] = (int)(colorValue & 0x3F);
	personalData[SPRITE_FLIP] = (int)((colorValue & 0x40) >> 6);
	personalData[FORM_RARE] = (bool)((colorValue & 0x80) >> 7);

	personalData[BASE_EXP] = (int)FileStreamReadUpdate<u16>(fileStream, currentByte);

	personalData[WEIGHT] = (int)FileStreamReadUpdate<u16>(fileStream, currentByte);
	personalData[HEIGHT] = (int)FileStreamReadUpdate<u16>(fileStream, currentByte);

	for (u32 tmhm = 0; tmhm < TM_HM_COUNT; ++tmhm)
		personalData[(u32)TM_HM_1 + tmhm] = (int)FileStreamReadUpdate<u32>(fileStream, currentByte);

	personalData[TYPE_TUTORS] = (int)FileStreamReadUpdate<u32>(fileStream, currentByte);

	for (u32 specialTutor = 0; specialTutor < SPECIAL_TUTOR_COUNT; ++specialTutor)
		personalData[(u32)SPECIAL_TUTORS_1 + specialTutor] = FileStreamReadUpdate<u32>(fileStream, currentByte);

	ReleaseFileStream(fileStream);
	return true;
}

bool Engine::SavePersonal(const PersonalData& personalData, const string& file)
{
	FileStream fileStream;
	if (!LoadEmptyFileStream(fileStream))
		return false;

	for (u32 stat = 0; stat < STAT_COUNT; ++stat)
		FileStreamPutBack<u8>(fileStream, (u8)personalData[(u32)BASE_HP + stat]);

	for (u32 type = 0; type < TYPE_COUNT; ++type)
		FileStreamPutBack<u8>(fileStream, (u8)personalData[(u32)TYPE_1 + type]);

	FileStreamPutBack<u8>(fileStream, (u8)personalData[CATCH_RATE]);
	FileStreamPutBack<u8>(fileStream, (u8)personalData[EVOLUTION_STAGE]);

	u16 evYield = 0;
	for (int stat = STAT_COUNT - 1; stat >= 0; --stat)
	{
		evYield |= (personalData[(u32)EV_HP + stat] & 0x0003);
		evYield = evYield << 2;
	}
	if (personalData[GROUNDED_SPRITE] != 0)
		evYield |= 0x1000;
	FileStreamPutBack<u16>(fileStream, evYield);

	for (u32 item = 0; item < WILD_ITEM_COUNT; ++item)
	{
		u16 itemValue = (u16)personalData[(u32)WILD_ITEM_50 + item];
		if (expandedAbilities)
		{
			itemValue &= 0x3FFF;
			// Save the 2 most significant ability bits at the end of the item value
			itemValue += ((u16)personalData[(u32)ABILITY_1 + item] & 0x0300) << 6;
		}
		FileStreamPutBack<u16>(fileStream, itemValue);
	}

	FileStreamPutBack<u8>(fileStream, (u8)personalData[SEX_CHANCE]);

	FileStreamPutBack<u8>(fileStream, (u8)personalData[EGG_HAPPINESS]);
	FileStreamPutBack<u8>(fileStream, (u8)personalData[BASE_HAPPINESS]);

	FileStreamPutBack<u8>(fileStream, (u8)personalData[GROWTH_RATE]);

	for (u32 eggGroup = 0; eggGroup < EGG_GROUP_COUNT; ++eggGroup)
		FileStreamPutBack<u8>(fileStream, (u8)personalData[(u32)EGG_GROUP_1 + eggGroup]);

	for (u32 ability = 0; ability < ABILITY_COUNT; ++ability)
		FileStreamPutBack<u8>(fileStream, (u8)personalData[(u32)ABILITY_1 + ability]);

	FileStreamPutBack<u8>(fileStream, (u8)personalData[ESCAPE_RATE]);

	FileStreamPutBack<u16>(fileStream, (u16)personalData[FORM_OFFSET]);
	FileStreamPutBack<u16>(fileStream, (u16)personalData[FORM_SPRITE_OFFSET]);
	FileStreamPutBack<u8>(fileStream, (u8)personalData[FORM_COUNT]);

	u8 color = (u8)personalData[COLOR] & 0x3F;
	if (personalData[SPRITE_FLIP] != 0)
		color |= 0x40;
	if (personalData[FORM_RARE] != 0)
		color |= 0x80;
	FileStreamPutBack<u8>(fileStream, color);

	FileStreamPutBack<u16>(fileStream, (u16)personalData[BASE_EXP]);

	FileStreamPutBack<u16>(fileStream, (u16)personalData[WEIGHT]);
	FileStreamPutBack<u16>(fileStream, (u16)personalData[HEIGHT]);

	for (u32 tmhm = 0; tmhm < TM_HM_COUNT; ++tmhm)
		FileStreamPutBack<u32>(fileStream, personalData[(u32)TM_HM_1 + tmhm]);

	FileStreamPutBack<u32>(fileStream, (u32)personalData[TYPE_TUTORS]);

	for (u32 specialTutor = 0; specialTutor < SPECIAL_TUTOR_COUNT; ++specialTutor)
		FileStreamPutBack<u32>(fileStream, personalData[(u32)SPECIAL_TUTORS_1 + specialTutor]);

	SaveFileStream(fileStream, file);
	ReleaseFileStream(fileStream);
	return true;
}

bool Engine::LoadLearnset(LearnsetData& learnData, const string& file)
{
	LearnsetReset(learnData);

	FileStream fileStream;
	if (!LoadFileStream(fileStream, file))
		return false;
	u32 currentByte = 0;

	for (u32 learnIdx = 0; learnIdx < project->learnsetSize && learnIdx < LEARNSET_DATA_MAX; ++learnIdx)
	{
		u16 moveID = FileStreamReadUpdate<u16>(fileStream, currentByte);
		if (moveID == LEARNSET_NULL)
			break;
		u16 level = FileStreamReadUpdate<u16>(fileStream, currentByte);
		if (level == LEARNSET_NULL)
			break;

		learnData[learnIdx][LEARN_MOVE_ID] = (int)moveID;
		learnData[learnIdx][LEARN_LEVEL] = (int)level;
	}

	ReleaseFileStream(fileStream);
	return true;
}

bool Engine::SaveLearnset(const LearnsetData& learnData, const string& file)
{
	FileStream fileStream;
	if (!LoadEmptyFileStream(fileStream))
		return false;

	u32 learnIdx = 0;
	vector<pair<u32, int>> orderedSet = Learnset::GetOrderedLearnset(learnData);
	for (; learnIdx < orderedSet.size(); ++learnIdx)
	{
		u32 idx = orderedSet[learnIdx].first;
		FileStreamPutBack<u16>(fileStream, (u16)learnData[idx][LEARN_MOVE_ID]);
		FileStreamPutBack<u16>(fileStream, (u16)learnData[idx][LEARN_LEVEL]);
	}
	if (learnIdx < project->learnsetSize)
	{
		FileStreamPutBack<u16>(fileStream, (u16)LEARNSET_NULL);
		FileStreamPutBack<u16>(fileStream, (u16)LEARNSET_NULL);
	}
		

	SaveFileStream(fileStream, file);
	ReleaseFileStream(fileStream);
	return true;
}

bool Engine::LoadEvolution(EvolutionData& evoData, const string& file)
{
	EvolutionReset(evoData);

	FileStream fileStream;
	if (!LoadFileStream(fileStream, file))
		return false;
	u32 currentByte = 0;

	for (u32 evoIdx = 0; evoIdx < project->evolutionSize && evoIdx < EVOLUTION_DATA_MAX; ++evoIdx)
	{
		evoData[evoIdx][EVOLUTION_METHOD] = (int)FileStreamReadUpdate<u16>(fileStream, currentByte);
		evoData[evoIdx][EVOLUTION_PARAM] = (int)FileStreamReadUpdate<u16>(fileStream, currentByte);
		evoData[evoIdx][EVOLUTION_SPECIES] = (int)FileStreamReadUpdate<u16>(fileStream, currentByte);
	}

	ReleaseFileStream(fileStream);
	return true;
}

bool Engine::SaveEvolution(const EvolutionData& evoData, const string& file)
{
	FileStream fileStream;
	if (!LoadEmptyFileStream(fileStream))
		return false;

	for (u32 evoIdx = 0; evoIdx < project->evolutionSize && evoIdx < EVOLUTION_DATA_MAX; ++evoIdx)
	{
		FileStreamPutBack<u16>(fileStream, (u16)evoData[evoIdx][EVOLUTION_METHOD]);
		FileStreamPutBack<u16>(fileStream, (u16)evoData[evoIdx][EVOLUTION_PARAM]);
		FileStreamPutBack<u16>(fileStream, (u16)evoData[evoIdx][EVOLUTION_SPECIES]);
	}

	SaveFileStream(fileStream, file);
	ReleaseFileStream(fileStream);
	return true;
}

bool Engine::LoadChild(ChildData& childData, const string& file)
{
	childData = CHILD_NULL;

	FileStream fileStream;
	if (!LoadFileStream(fileStream, file))
		return false;

	childData = (int)FileStreamRead<u16>(fileStream, 0);

	ReleaseFileStream(fileStream);

	return true;
}

bool Engine::SaveChild(const ChildData& childData, const string& file)
{
	FileStream fileStream;
	if (!LoadEmptyFileStream(fileStream))
		return false;

	FileStreamPutBack<u16>(fileStream, (u16)childData);

	SaveFileStream(fileStream, file);
	ReleaseFileStream(fileStream);
	return true;
}

void Engine::HandleReverseEvent()
{
	if (!reverseEvents.size())
		return;

	Event& topEvent = reverseEvents.back();

	modules[topEvent.type]->HandleReverseEvent(&topEvent);
		
	delete topEvent.value;
	reverseEvents.pop_back();
}

void Engine::Save()
{
	for (u32 idx = 0; idx < (u32)saveEvents.size(); ++idx)
	{
		Event* saveEvent = &(saveEvents[idx]);
		modules[saveEvent->type]->HandleSaveEvent(saveEvent);
	}

	SaveEnabledPatches();

	SaveProjectSettings(*project);
}

void Engine::LoadPatches(bool reload)
{
	// Save the enabled state of the old patches before deleting the data when reloading
	if (reload)
		SaveEnabledPatches();

	patches.clear();
	// Delete any reverse event that affects the Patcher module
	for (u32 eventIdx = 0; eventIdx < (u32)reverseEvents.size(); ++eventIdx)
	{
		if (reverseEvents[eventIdx].type == patchesModuleIdx)
		{
			reverseEvents.erase(reverseEvents.begin() + eventIdx);
			--eventIdx;
		}
	}

	// Check that patches can be installed to CTRMap
	string ctrPatchesFolder = project->ctrMapProjectPath + PATH_SEPARATOR + CTRMAP_PATCHES_PATH;
	if (!PathExists(ctrPatchesFolder))
	{
		patchesEnabled = false;
		return;
	}
	patchesEnabled = true;

	// Load each patch in the patches folder
	vector<string> patchesNames = GetFolderElementList(PATCHES_PATH);
	for (u32 patchIdx = 0; patchIdx < (u32)patchesNames.size(); ++patchIdx)
	{
		Patch patch;
		patch.name = patchesNames[patchIdx];

		string patchPath = string(PATCHES_PATH) + PATH_SEPARATOR + patch.name + PATH_SEPARATOR + KLANG_PATH;
		if (!LoadKlang(patch.settings, patchPath))
		{
			Log(WARNING, "Couldn't load %s patch", patch.name);
			break;
		}

		// If the patch was enabled before loading, keep it that way
		for (u32 enabledIdx = 0; enabledIdx < (u32)project->enabledPatches.size(); ++enabledIdx)
		{
			if (patch.name == project->enabledPatches[enabledIdx])
			{
				patch.enabled = true;
				break;
			}
		}

		patches.push_back(patch);
	}
}

enum CompileType
{
	DONT_COMPILE = 0,
	CPP = 1,
	ARM = 2,
};
void Engine::BuildPatches()
{
	if (!patchesEnabled)
		return;

	for (u32 patchIdx = 0; patchIdx < (u32)patches.size(); ++patchIdx)
	{
		Patch& patch = patches[patchIdx];
		if (patch.enabled)
		{
			string patchPath = string(PATCHES_PATH) + PATH_SEPARATOR + patch.name;

			string esdbPath;
			vector<string> pathFiles = GetFolderElementList(patchPath);
			for (u32 fileIdx = 0; fileIdx < (u32)pathFiles.size(); ++fileIdx)
			{
				string extension = LowerCase(GetFileExtension(pathFiles[fileIdx]));
				if (extension == "yml")
				{
					esdbPath = patchPath + PATH_SEPARATOR + pathFiles[fileIdx];
					break;
				}
			}
			if (esdbPath.empty())
			{
				Log(WARNING, "%s patch is missing an ESDB!", patch.name.c_str());
				continue;
			}

			string buildPath = patchPath + PATH_SEPARATOR + "build";
			CreateFolder(buildPath);

			FileStream fileStream;
			LoadEmptyFileStream(fileStream);

			string sourcePath = patchPath + PATH_SEPARATOR + "source";
			vector<string> sourceFiles = GetFolderElementList(sourcePath);

			// Compile each source file
			vector<string> elfFiles;
			for (u32 fileIdx = 0; fileIdx < (u32)sourceFiles.size(); ++fileIdx)
			{
				string extension = LowerCase(GetFileExtension(sourceFiles[fileIdx]));

				CompileType compType = DONT_COMPILE;
				if (extension == "cpp")
					compType = CPP;
				else if (extension == "s")
					compType = ARM;
				else
					continue;

				string fileName = sourceFiles[fileIdx].substr(0, sourceFiles[fileIdx].length() - extension.length() - 1);
				
				string compileCommand;
				if (project->compilerPath.length())
					compileCommand += project->compilerPath + PATH_SEPARATOR;
				compileCommand += "arm-none-eabi-g++ ";
				compileCommand += GetAbsolutePath(sourcePath) + PATH_SEPARATOR;
				compileCommand += sourceFiles[fileIdx] + " ";

				string elfPath = buildPath + PATH_SEPARATOR;
				switch (compType)
				{
				case CPP:
				{
					elfPath += fileName + ".o";

					compileCommand += string("-I ") + project->extLibPath + " ";
					compileCommand += string("-I ") + project->libRPMPath + " ";
					compileCommand += string("-I ") + project->nkPath + " ";
					compileCommand += string("-I ") + project->swanPath + " ";
					compileCommand += string("-o ") + elfPath + " ";
					compileCommand += "-r -mthumb -march=armv5t -Os";

					break;
				}
				case ARM:
				{
					elfPath += fileName + "ARM.o";

					compileCommand += string("-o ") + elfPath + " ";
					compileCommand += "-r -mthumb -march=armv5t -Os";

					break;
				}
				}
				compileCommand += "\n";

				elfFiles.push_back(elfPath);
				FileStreamBufferWriteBack(fileStream, (u8*)compileCommand.c_str(), compileCommand.length());
			}

			// Merge all the compiled files in a single ELF file
			string elfPath = buildPath + PATH_SEPARATOR + patch.name + ".elf";
			string mergeCommand;
			if (project->compilerPath.length())
				mergeCommand += project->compilerPath + PATH_SEPARATOR;
			mergeCommand += "arm-none-eabi-ld -r ";
			mergeCommand += string("-o ") + elfPath + " ";
			for (u32 elfIdx = 0; elfIdx < (u32)elfFiles.size(); ++elfIdx)
				mergeCommand += elfFiles[elfIdx] + " ";
			mergeCommand += "\n";
			FileStreamBufferWriteBack(fileStream, (u8*)mergeCommand.c_str(), mergeCommand.length());

			string dllPath = project->ctrMapProjectPath + PATH_SEPARATOR + CTRMAP_PATCHES_PATH + PATH_SEPARATOR + patch.name + ".dll";

			// Link the patch using CTRMap
			string linkCommand;
			if (project->javaPath.length())
				linkCommand += project->javaPath + PATH_SEPARATOR;
			linkCommand += "java ";
			linkCommand += string("-cp ") + project->ctrMapPath + PATH_SEPARATOR + "CTRMap.jar ";
			linkCommand += "rpm.cli.RPMTool ";
			linkCommand += string("-i ") + elfPath + " ";
			linkCommand += string("-o ") + dllPath + " ";
			linkCommand += string("--esdb ") + esdbPath + " ";
			linkCommand += "--fourcc DLXF --generate-relocations";
			linkCommand += "\n";
			FileStreamBufferWriteBack(fileStream, (u8*)linkCommand.c_str(), linkCommand.length());

			string buildFile = patchPath + PATH_SEPARATOR + patch.name;
#ifdef _WIN32
			buildFile += ".bat";
#else
			buildFile += ".sh";
#endif
			SaveFileStream(fileStream, buildFile);
			system(buildFile.c_str());
		}
	}
}

void Engine::SaveEnabledPatches()
{
	project->enabledPatches.clear();
	for (u32 patchIdx = 0; patchIdx < (u32)patches.size(); ++patchIdx)
		if (patches[patchIdx].enabled)
			project->enabledPatches.push_back(patches[patchIdx].name);
}