#pragma once

#include "livingEntity.h"
#include "npcs/npcData.h"
#include "hero.h"

#include <memory>


class FriendlyNpc : public LivingEntity {
public:
    FriendlyNpc(const NpcData& npcData, AnimationSet* animSet = nullptr);

    virtual void updateDamages() override;
    virtual void die() override;

    virtual void changeAnimation(int newState, bool resetFrameToBeginning, string animName = "") override;
    virtual void update() override;

    // Novas funcionalidades
    void interact(Hero* player);
    const NpcData& getData() const {
        return data;
    }

private:
    NpcData data;

    void updateAnimation();

    void showDialogue();
    void openShop();
    void giveQuest();
};


