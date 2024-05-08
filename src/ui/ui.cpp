#include "ui/ui.h"

const string Ui::HUD_TEXTURES_PATH = "\\Assets\\Textures\\HUD\\";
const string Ui::FONTS_PATH = "\\Assets\\Fonts\\";

Ui::Ui() {
}

Ui::~Ui() {
}

void Ui::drawHorizontalGradientBox(SDL_Renderer* renderer,
    const int x, const int y, const int w, const int h, const float steps,
    const SDL_Color c1, const SDL_Color c2, const int fill) {

    /* Acumulator initial position */
    float yt = y;
    float xt = x;
    float rt = c1.r;
    float gt = c1.g;
    float bt = c1.b;
    float at = c1.a;

    /* Changes in each attribute */
    float ys = h / steps;
    float xs = w / steps;
    float rs = (c2.r - c1.r) / steps;
    float gs = (c2.g - c1.g) / steps;
    float bs = (c2.b - c1.b) / steps;
    float as = (c2.a - c1.a) / steps;

    for (int i = 0; i < steps; i++)
    {
        /* Create an horizontal rectangle sliced by the number of steps */
        SDL_Rect rect = { x, yt, w, ys+1 };

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        /* Sets the rectangle color based on iteration */
        SDL_SetRenderDrawColor(renderer, rt, gt, bt, at);

        /* Paint it or coverit*/
        if (fill)
            SDL_RenderFillRect(renderer, &rect);
        else
            SDL_RenderDrawRect(renderer, &rect);

        /* Update colors and positions */
        yt += ys;
        xt += xs;
        rt += rs;
        gt += gs;
        bt += bs;
        at += as;
    }
}

void Ui::drawVerticalGradientBox(SDL_Renderer* renderer,
    const int x, const int y, const int w, const int h, const float steps,
    const SDL_Color c1, const SDL_Color c2, const int fill) {

    /* Acumulator initial position */
    float yt = y;
    float xt = x;
    float rt = c1.r;
    float gt = c1.g;
    float bt = c1.b;
    float at = c1.a;

    /* Changes in each attribute */
    float ys = h / steps;
    float xs = w / steps;
    float rs = (c2.r - c1.r) / steps;
    float gs = (c2.g - c1.g) / steps;
    float bs = (c2.b - c1.b) / steps;
    float as = (c2.a - c1.a) / steps;

    for (int i = 0; i < steps; i++)
    {
        /* Create an vertical rectangle sliced by the number of steps */
        SDL_Rect rect = { xt, y, xs + 1, h };

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        /* Sets the rectangle color based on iteration */
        SDL_SetRenderDrawColor(renderer, rt, gt, bt, at);

        /* Paint it or coverit*/
        if (fill)
            SDL_RenderFillRect(renderer, &rect);
        else
            SDL_RenderDrawRect(renderer, &rect);

        /* Update colors and positions */
        yt += ys;
        xt += xs;
        rt += rs;
        gt += gs;
        bt += bs;
        at += as;
    }
}
