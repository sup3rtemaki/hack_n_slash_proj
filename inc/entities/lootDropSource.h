#ifndef LOOTDROPSOURCE
#define LOOTDROPSOURCE

#include <cstdlib>
#include <map>

struct PendingItemDrop {
	int itemId;
	int quantity;
	int x;
	int y;
};

class LootDropSource {
public:
	virtual ~LootDropSource() = default;

	void dropItem(int itemId, int quantity, int x, int y);
	void checkIfDropsItem(float x, float y);
	bool takePendingItemDrop(PendingItemDrop& drop);
	bool hasDroppedEssence() const;
	void markEssenceDropped();
	int getEssence() const;

	int essence = 0;

protected:
	std::map<int, std::pair<std::pair<int, int>, int>> possibleDropItemsMap;

private:
	PendingItemDrop pendingItemDrop{};
	bool hasPendingItemDrop = false;
	bool essenceDropped = false;
};

#endif // !LOOTDROPSOURCE