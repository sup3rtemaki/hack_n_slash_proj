#include "hpBar.h"

void HPBar::draw() {
	if (entity != NULL) {
		SDL_SetRenderDrawColor(Globals::renderer, 240, 51, 159, SDL_ALPHA_OPAQUE);

		//draw hp bar borders (TODO add image)
		SDL_Rect barContainer = { x, y, barWidth, barHeight };
		SDL_RenderDrawRect(Globals::renderer, &barContainer);

		//percentage of entities health remaining
		float hpPercent = entity->hp / (entity->hpMax * 1.0f);//1.0f to return a float

		SDL_Rect hpRect = { x + 2, y + 2, (barWidth - 4) * hpPercent, (barHeight - 4) };

		SDL_RenderFillRect(Globals::renderer, &hpRect);
	}
}