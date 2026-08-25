#include "entities/lootDropSource.h"

void LootDropSource::dropItem(int itemId, int quantity, int x, int y) {
	pendingItemDrop = { itemId, quantity, x, y };
	hasPendingItemDrop = true;
}

void LootDropSource::checkIfDropsItem(float x, float y) {
	if (possibleDropItemsMap.empty()) {
		return;
	}

	for (const auto& dropEntry : possibleDropItemsMap) {
		int dropChance = rand() % 100;
		if (dropChance >= dropEntry.second.first.first &&
			dropChance < dropEntry.second.first.second) {
			int quantity = 1 + rand() % dropEntry.second.second;
			dropItem(dropEntry.first, quantity, static_cast<int>(x), static_cast<int>(y));
			return;
		}
	}
}

bool LootDropSource::takePendingItemDrop(PendingItemDrop& drop) {
	if (!hasPendingItemDrop) {
		return false;
	}

	drop = pendingItemDrop;
	hasPendingItemDrop = false;
	return true;
}

bool LootDropSource::hasDroppedEssence() const {
	return essenceDropped;
}

void LootDropSource::markEssenceDropped() {
	essenceDropped = true;
}

int LootDropSource::getEssence() const {
	return essence;
}