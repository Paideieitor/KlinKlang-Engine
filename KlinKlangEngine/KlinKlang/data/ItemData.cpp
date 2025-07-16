#include "ItemData.h"

#include "Globals.h"

void ItemDataReset(ItemData& itemData)
{
	for (u32 idx = 0; idx < (u32)itemData.size(); ++idx)
		itemData[idx] = ITEMDATA_NULL;
}
