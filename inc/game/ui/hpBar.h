#ifndef HPBAR
#define HPBAR

#include "ui.h"

#include "livingEntity.h"

enum class BarType {
	HERO_HEALTH_BAR,
	HERO_STAMINA_BAR,
	BOSS_HEALTH_BAR
};

class HPBar : public Ui {
public:
	int x, y;
	int barWidth = 200, barHeight = 30;
	LivingEntity* entity = nullptr;

	HPBar(LivingEntity* livingEntity, BarType barType);

	virtual void draw() override;
	virtual void setUp() override;

private:
	BarType barType;
	SDL_Color color;
};

#endif // !HPBARBOSS

