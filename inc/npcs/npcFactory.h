#pragma once

#include "npcs/friendlies/friendlyNpc.h"

#include <unordered_map>
#include <memory>
#include <filesystem>

class NpcFactory {
private:
	std::unordered_map<std::string, NpcData> npcDatabase;

public:
	NpcFactory() = default;

	// Carrega todos os NPCs de um diretório
	void loadAllNpcs(const std::string& directory);

	// Cria um NPC específico
	std::unique_ptr<FriendlyNpc> createNpc(const std::string& npcId, SDL_Renderer* renderer);

	// Verifica se NPC existe
	bool hasNpc(const std::string& npcId) const;
};
