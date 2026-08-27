#ifndef RESOURCECONFIG
#define RESOURCECONFIG

#include <string>

namespace ResourcePaths {
inline const std::string MAPS = "Maps\\";
inline const std::string TEXTURES = "Assets\\Textures\\";
inline const std::string HUD_TEXTURES = TEXTURES + "HUD\\";
inline const std::string FONTS = "Assets\\Fonts\\";
inline const std::string SOUNDS = "Assets\\Sounds\\";
inline const std::string ANIMATIONS = "Assets\\Animations\\";
inline const std::string ALL_WHITE_TEXTURE = TEXTURES + "allwhite.png";
}

namespace SoundIds {
inline const std::string HIT = "hit";
inline const std::string ENEMY_HIT = "enemyHit";
inline const std::string SWING = "swing";
inline const std::string DASH = "dash";
inline const std::string GROWL = "growl";
inline const std::string ENEMY_DIE = "enemyDie";
inline const std::string CRASH = "crash";
inline const std::string SMASH = "smash";
inline const std::string SHOOT = "shoot";
inline const std::string LAUGH = "laugh";
}

#endif