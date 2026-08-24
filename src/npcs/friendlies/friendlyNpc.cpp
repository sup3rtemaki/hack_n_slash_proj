// FriendlyNpc.cpp
#include "npcs/friendlies/friendlyNpc.h"

#include <iostream>

FriendlyNpc::FriendlyNpc(const NpcData& npcData) : data(npcData) {
    // Inicializa com dados do JSON e cria seu próprio AnimationSet
    list<DataGroupType> dataGroupTypes;
    DataGroupType colBoxType; colBoxType.groupName = "collisionBox"; colBoxType.dataType = DataGroupType::DATATYPE_BOX;
    DataGroupType hitBoxType; hitBoxType.groupName = "hitBox"; hitBoxType.dataType = DataGroupType::DATATYPE_BOX;
    DataGroupType dmgType; dmgType.groupName = "damage"; dmgType.dataType = DataGroupType::DATATYPE_NUMBER;
    dataGroupTypes.push_back(colBoxType);
    dataGroupTypes.push_back(hitBoxType);
    dataGroupTypes.push_back(dmgType);

    localAnimSet = std::make_unique<AnimationSet>();
    localAnimSet->loadAnimationSet("Assets\\Animations\\npc_1.fdset", dataGroupTypes, true, 0, true);
    this->animSet = localAnimSet.get();
    type = "friendly";
    collisionBoxW = 16;
    collisionBoxH = 16;
    collisionBoxYOffset = 0;
    direction = DIR_DOWN;

    if (this->animSet) {
        changeAnimation(0, true);
    }
    updateCollisionBox();
}

void FriendlyNpc::updateDamages() {
    // By default friendly NPCs don't take damage from the game's combat system.
    // Keep empty to prevent hostile interactions, override in subclasses if needed.
}

void FriendlyNpc::die() {
    // Friendly NPCs normally shouldn't die; log if it happens.
    std::cout << "FriendlyNpc '" << data.npcId << "' died (override expected)." << std::endl;
}

void FriendlyNpc::interact(Hero* player) {
    if (data.isMerchant) {
        openShop();
    }
    else if (data.hasQuests) {
        giveQuest();
    }
    else {
        showDialogue();
    }
}

void FriendlyNpc::showDialogue() {
    std::cout << "[NPC] " << data.npcName << ":" << std::endl;
    for (const auto& line : data.dialogueLines) {
        std::cout << "  - " << line << std::endl;
    }
}

void FriendlyNpc::openShop() {
    std::cout << "[NPC Shop] " << data.npcName << " inventory:" << std::endl;
    for (const auto& item : data.shopInventory) {
        std::cout << "  - " << item.itemId << " price=" << item.price << " stock=" << item.stock << std::endl;
    }
}

void FriendlyNpc::giveQuest() {
    std::cout << "[NPC Quest] " << data.npcName << " offers quests:" << std::endl;
    for (const auto& q : data.questIds) {
        std::cout << "  - " << q << std::endl;
    }
}

void FriendlyNpc::changeAnimation(int newState, bool resetFrameToBeginning, string animName) {
    state = newState;
    if (animSet == nullptr) return;

    // If an explicit animation name was provided, use it
    if (!animName.empty()) {
        currentAnim = animSet->getAnimation(animName);
    }
    else {
        // Try common idle animation names, fallback to first available
        if (direction == DIR_DOWN) {
            currentAnim = animSet->getAnimation("idleDown");
        }
        else if (direction == DIR_UP) {
            currentAnim = animSet->getAnimation("idleUp");
        }
        else if (direction == DIR_LEFT) {
            currentAnim = animSet->getAnimation("idleLeft");
        }
        else if (direction == DIR_RIGHT) {
            currentAnim = animSet->getAnimation("idleRight");
        }

        if (currentAnim == nullptr) {
            // fallback to a generic 'idle' or the first animation
            currentAnim = animSet->getAnimation("idleDown");
        }
    }

    if (currentAnim == nullptr) return;

    if (resetFrameToBeginning) {
        currentFrame = currentAnim->getFrame(0);
    }
    else {
        if (currentFrame != nullptr) currentFrame = currentAnim->getFrame(currentFrame->frameNumber);
        else currentFrame = currentAnim->getFrame(0);
    }
}

void FriendlyNpc::updateAnimation() {
    if (currentFrame == nullptr || currentAnim == nullptr) return;

    frameTimer += TimeController::timeController.dT;

    if (frameTimer >= currentFrame->duration) {
        if (currentFrame->frameNumber == currentAnim->getEndFrameNumber()) {
            // loop
            currentFrame = currentAnim->getFrame(0);
        }
        else {
            currentFrame = currentAnim->getNextFrame(currentFrame);
        }
        frameTimer = 0;
    }
}

void FriendlyNpc::update() {
    // Friendly NPCs are mostly static, but we need to keep animation and timers updated
    updateCollisionBox();
    updateHitBox();
    updateAnimation();
    updateInvincibleTimer();
}

