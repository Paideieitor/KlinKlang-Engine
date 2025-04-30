#include "Module.h"

#include "FileUtils.h"

#include "Engine.h"

#define MODULE_CTR_PATH(fsPath) engine->project->ctrPath + PATH_SEPARATOR + CTRMAP_FILESYSTEM_PATH + PATH_SEPARATOR + fsPath

Module::Module(Engine* const engine, u32 group, const string& fsPath) : engine(engine), group(group), 
	moduleIdx((u32)engine->modules.size()), savePath(MODULE_CTR_PATH(fsPath))
{
}
