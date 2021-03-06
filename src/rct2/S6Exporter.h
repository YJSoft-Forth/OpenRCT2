#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#include "../common.h"

extern "C"
{
    #include "../scenario.h"
}

/**
 * Class to export RollerCoaster Tycoon 2 scenarios (*.SC6) and saved games (*.SV6).
 */
class S6Exporter
{
public:
    bool ExportObjects;
    bool RemoveTracklessRides;

    S6Exporter();

    void SaveGame(const utf8 * path);
    void SaveGame(SDL_RWops *rw);
    void SaveScenario(const utf8 * path);
    void SaveScenario(SDL_RWops *rw);
    void Export();

private:
    rct_s6_data _s6;

    void Save(SDL_RWops *rw, bool isScenario);
};
