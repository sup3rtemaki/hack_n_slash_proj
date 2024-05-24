#include "ui/hpBar.h"

const int HERO_HP_BAR_X = 10;
const int HERO_HP_BAR_Y = 10;
const int HERO_HP_BAR_W = 10;
const int HERO_HP_BAR_H = 10;

const int HERO_ST_BAR_X = 10;
const int HERO_ST_BAR_Y = 25;
const int HERO_ST_BAR_W = 10;
const int HERO_ST_BAR_H = 25;

const int BOSS_HP_BAR_W = 200;
const int BOSS_HP_BAR_H = 30;

const int HERO_HP_BAR_R = 240;
const int HERO_HP_BAR_G = 51;
const int HERO_HP_BAR_B= 51;

const int HERO_ST_BAR_R = 51;
const int HERO_ST_BAR_G = 240;
const int HERO_ST_BAR_B = 51;

const int BOSS_HP_BAR_R = 240;
const int BOSS_HP_BAR_G = 51;
const int BOSS_HP_BAR_B = 51;

HPBar::HPBar(LivingEntity* livingEntity, BarType barType) : 
	entity(livingEntity), barType(barType) {
	setUp();
}

void HPBar::draw() {
	if (entity == nullptr) return;

	SDL_SetRenderDrawColor(Globals::renderer, 240, 51, 159, SDL_ALPHA_OPAQUE);

	//draw hp bar borders (TODO add image)
	SDL_Rect barContainer = { x, y, barWidth, barHeight };
	SDL_RenderDrawRect(Globals::renderer, &barContainer);

	//percentage of entities health remaining
	float hpPercent = entity->hp / (entity->hpMax * 1.0f);//1.0f to return a float

	SDL_Rect hpRect = { x + 2, y + 2, (barWidth - 4) * hpPercent, (barHeight - 4) };

	SDL_RenderFillRect(Globals::renderer, &hpRect);
}

void HPBar::setUp() {
	if (entity == nullptr) return;

	switch (barType) {
		case BarType::HERO_HEALTH_BAR:
			x = HERO_HP_BAR_X;
			y = HERO_HP_BAR_Y;
			barWidth = entity->hpMax * HERO_HP_BAR_W;
			barHeight = HERO_HP_BAR_H;
			color.r = HERO_HP_BAR_R;
			color.g = HERO_HP_BAR_G;
			color.b = HERO_HP_BAR_B;
			break;
		case BarType::HERO_STAMINA_BAR:
			x = HERO_ST_BAR_X;
			y = HERO_ST_BAR_Y;
			barWidth = entity->hpMax * HERO_ST_BAR_W;
			barHeight = HERO_ST_BAR_H;
			color.r = HERO_ST_BAR_R;
			color.g = HERO_ST_BAR_G;
			color.b = HERO_ST_BAR_B;
			break;
		case BarType::BOSS_HEALTH_BAR:
			x = Globals::ScreenWidth / 2.0f - (barWidth / 2.0f); // centered horizontally
			y = Globals::ScreenHeight - barHeight - 20; // 20 pixels off the bottom
			barWidth = BOSS_HP_BAR_W;
			barHeight = BOSS_HP_BAR_H;
			color.r = BOSS_HP_BAR_R;
			color.g = BOSS_HP_BAR_G;
			color.b = BOSS_HP_BAR_B;
			break;

		default:
			break;
	}
}
