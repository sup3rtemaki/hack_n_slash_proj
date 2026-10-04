#pragma once

#include <deque>
#include "SDL.h"

class PheromoneTrailSource {
public:
	virtual ~PheromoneTrailSource() = default;
	virtual const std::deque<SDL_Point>& getPheromoneTrail() const = 0;
};
