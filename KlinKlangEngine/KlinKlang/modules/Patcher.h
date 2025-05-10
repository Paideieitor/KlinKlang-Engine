#ifndef _PATCHER_H
#define _PATCHER_H

#include "Globals.h"

#include "Module.h"

struct Patch;
class KlangVar;
class Patcher : public Module
{
public:
	Patcher() = delete;
	Patcher(Engine* const engine, u32 group);
	~Patcher();

	virtual ReturnState RenderGUI() override;

	virtual void HandleReverseEvent(const Event* reverseEvent) override;
	virtual void HandleSaveEvent(Event* saveEvent) override {}

private:

	void PatchMenu(Patch* patch);

	void TextInput(string* text, const char* label, u32 eventID);
	void InputInt(KlangVar* var, const char* label);
	void CheckBox(Patch* patch, const char* label);
};

#endif // _MENU_BAR_H
