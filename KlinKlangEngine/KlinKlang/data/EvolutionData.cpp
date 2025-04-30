#include "EvolutionData.h"

#include "Globals.h"

void EvolutionReset(EvolutionData& evolution)
{
	for (u32 evoIdx = 0; evoIdx < (u32)evolution.size(); ++evoIdx)
		for (u32 idx = 0; idx < (u32)evolution[evoIdx].size(); ++idx)
			evolution[evoIdx][idx] = EVOLUTION_NULL;
}
