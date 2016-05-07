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

#include "../../world/map.h"
#include "../track_paint.h"
#include "../track.h"
#include "../../paint/paint.h"
#include "../../interface/viewport.h"
#include "../../paint/supports.h"
#include "../ride.h"
#include "../ride_data.h"

enum
{
    SPR_METAL_FLOOR = 14567,
    SPR_METAL_FENCE_NE = 14568,
    SPR_METAL_FENCE_SE = 14569,
    SPR_METAL_FENCE_SW = 14570,
    SPR_METAL_FENCE_NW = 14571
};

enum
{
    SPR_CHAIRLIFT_CABLE_FLAT_SW_NE = 20500,
    SPR_CHAIRLIFT_CABLE_FLAT_SE_NW,
    SPR_20502,
    SPR_20503,
    SPR_20504,
    SPR_20505,
    SPR_CHAIRLIFT_STATION_COLUMN_NE_SW,
    SPR_CHAIRLIFT_STATION_COLUMN_SE_NW,
    SPR_20508,
    SPR_20509,
    SPR_20510,
    SPR_20511,
    SPR_20512,
    SPR_20513,
    SPR_20514,
    SPR_20515,
    SPR_CHAIRLIFT_CABLE_UP_SW_NE,
    SPR_CHAIRLIFT_CABLE_UP_NW_SE,
    SPR_CHAIRLIFT_CABLE_UP_NE_SW,
    SPR_CHAIRLIFT_CABLE_UP_SE_NW,
    SPR_20520,
    SPR_20521,
    SPR_20522,
    SPR_20523,
    SPR_20524,
    SPR_20525,
    SPR_20526,
    SPR_20527,
    SPR_CHAIRLIFT_CORNER_NW_SW,
    SPR_CHAIRLIFT_CORNER_NW_NE,
    SPR_CHAIRLIFT_CORNER_SE_NE,
    SPR_CHAIRLIFT_CORNER_SW_SE,
    SPR_20532,
    SPR_20533,
    SPR_20534,
    SPR_20535,
    SPR_20536,
    SPR_20537,
    SPR_20538,
    SPR_20539,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_1,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_2,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_3,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_4,
    SPR_CHAIRLIFT_STATION_END_CAP_NE,
    SPR_CHAIRLIFT_STATION_END_CAP_SE,
    SPR_CHAIRLIFT_STATION_END_CAP_SW,
    SPR_CHAIRLIFT_STATION_END_CAP_NW,
} SPR_CHAIRLIFT;

const uint32 chairlift_bullwheel_frames[] = {
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_1,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_2,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_3,
    SPR_CHAIRLIFT_BULLWHEEL_FRAME_4
};

enum
{
    SEGMENT_B4 = (1 << 0),
    SEGMENT_0 = SEGMENT_B4,
    SEGMENT_B8 = (1 << 1),
    SEGMENT_1 = SEGMENT_B8,
    SEGMENT_BC = (1 << 2),
    SEGMENT_2 = SEGMENT_BC,
    SEGMENT_C0 = (1 << 3),
    SEGMENT_3 = SEGMENT_C0,
    SEGMENT_C4 = (1 << 4),
    SEGMENT_4 = SEGMENT_C4,
    SEGMENT_C8 = (1 << 5),
    SEGMENT_5 = SEGMENT_C8,
    SEGMENT_CC = (1 << 6),
    SEGMENT_6 = SEGMENT_CC,
    SEGMENT_D0 = (1 << 7),
    SEGMENT_7 = SEGMENT_D0,
    SEGMENT_D4 = (1 << 8),
    SEGMENT_8 = SEGMENT_D4,
};

const int SEGMENTS_ALL = SEGMENT_B4 | SEGMENT_B8 | SEGMENT_BC | SEGMENT_C0 | SEGMENT_C4 | SEGMENT_C8 | SEGMENT_CC | SEGMENT_D0 | SEGMENT_D4;

static void paint_util_set_segment_support_height(int flags, uint16 height, uint8 segment_flags)
{
    for (int s = 0; s < 9; s++) {
        if (flags & (1 << s)) {
            RCT2_GLOBAL(0x0141E9B4 + s * 4, uint16) = height;
            RCT2_GLOBAL(0x0141E9B6 + s * 4, uint8) = segment_flags;
        }
    }
}

static void paint_util_set_support_height(uint16 height, uint8 flags)
{
    if (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) < height) {
        RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, sint16) = height;
        RCT2_GLOBAL(0x141E9DA, uint8) = flags;
    }
}

enum
{
    TUNNEL_0,
    TUNNEL_FLAT = 6,
    TUNNEL_7 = 7,
    TUNNEL_UP = 8,
    TUNNEL_14 = 0x0E
};

static void paint_util_push_tunnel_left(uint16 height, uint8 type)
{
    uint32 eax = 0xFFFF0000 | ((height / 16) & 0xFF) | type << 8;
    RCT2_ADDRESS(0x009E3138, uint32)[RCT2_GLOBAL(0x141F56A, uint8) / 2] = eax;
    RCT2_GLOBAL(0x141F56A, uint8)++;
}

static void paint_util_push_tunnel_right(uint16 height, uint8 type)
{
    uint32 eax = 0xFFFF0000 | ((height / 16) & 0xFF) | type << 8;
    RCT2_ADDRESS(0x009E30B6, uint32)[RCT2_GLOBAL(0x141F56B, uint8) / 2] = eax;
    RCT2_GLOBAL(0x141F56B, uint8)++;
}

/* rct2: 0x007667AE */
static rct_xy16 loc_7667AE[] = {
    {.x = 0, .y = -1},
    {.x = 1, .y = 0},
    {.x = 0, .y = 1},
    {.x = -1, .y = 0},
};

/* rct2: 0x007667AC */
static rct_xy16 loc_7667AC[] = {
    {.x = -1, .y = 0},
    {.x = 0, .y = -1},
    {.x = 1, .y = 0},
    {.x = 0, .y = 1},
};

typedef enum EDGE
{
    EDGE_NE,
    EDGE_SE,
    EDGE_SW,
    EDGE_NW
} EDGE;

static bool paint_util_has_fence(EDGE edge, rct_xy16 position, rct_map_element * mapElement, rct_ride * ride, uint8 rotation)
{
    rct_xy16 offset;
    switch (edge) {
        case EDGE_NE:
            offset = loc_7667AC[rotation];
            break;
        case EDGE_SE:
            offset = loc_7667AC[(rotation + 1) & 3];
            break;
        case EDGE_SW:
            offset = loc_7667AE[(rotation + 1) & 3];
            break;
        case EDGE_NW:
            offset = loc_7667AE[rotation];
            break;
    };

    uint16 entranceLoc =
        ((position.x / 32) + offset.x) |
        (((position.y / 32) + offset.y) << 8);

    uint8 entranceId = (mapElement->properties.track.sequence & 0x70) >> 4;

    return (ride->entrances[entranceId] != entranceLoc && ride->exits[entranceId] != entranceLoc);
}

static void chairlift_paint_util_draw_supports(int supports, uint16 height)
{
    bool success = false;

    for (int s = 0; s < 9; s++) {
        if (!(supports & (1 << s))) {
            continue;
        }

        if (metal_a_supports_paint_setup(10, s, 0, height, RCT2_GLOBAL(0x00F4419C, uint32))) {
            success = true;
        }
    }

    if (success) {
        return;
    }

    for (int s = 0; s < 9; s++) {
        if (!(supports & (1 << s))) {
            continue;
        }
        uint16 temp = RCT2_GLOBAL(0x0141E9B4 + s * 4, uint16);
        RCT2_GLOBAL(0x0141E9B4 + s * 4, uint16) = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_PAINT_TILE_MAX_HEIGHT, uint16);
        metal_a_supports_paint_setup(10, s, 0, height, RCT2_GLOBAL(0x00F4419C, uint32));
        RCT2_GLOBAL(0x0141E9B4 + s * 4, uint16) = temp;
    }
}

rct_map_element * map_get_track_element_at_from_ride_fuzzy(int x, int y, int z, int rideIndex)
{
    rct_map_element * mapElement = map_get_first_element_at(x >> 5, y >> 5);
    do {
        if (map_element_get_type(mapElement) != MAP_ELEMENT_TYPE_TRACK) continue;
        if (mapElement->properties.track.ride_index != rideIndex) continue;
        if (mapElement->base_height != z && mapElement->base_height != z - 1) continue;

        return mapElement;
    } while (!map_element_is_last_for_tile(mapElement++));

    return NULL;
};

enum
{
    SPR_STATION_COVER_OFFSET_NE_SW_BACK_0 = 0,
    SPR_STATION_COVER_OFFSET_NE_SW_BACK_1,
    SPR_STATION_COVER_OFFSET_NE_SW_FRONT,
    SPR_STATION_COVER_OFFSET_SE_NW_BACK_0,
    SPR_STATION_COVER_OFFSET_SE_NW_BACK_1,
    SPR_STATION_COVER_OFFSET_SE_NW_FRONT,
    SPR_STATION_COVER_OFFSET_HIGH
} SPR_STATION_COVER;

static bool paint_util_draw_station_covers(EDGE edge, bool hasFence, rct_ride_entrance_definition * entranceStyle, uint8 direction, uint16 height)
{
    uint32 imageId;
    uint32 baseImageId = entranceStyle->flags;
    int imageOffset;
    rct_xyz16 offset, bounds, boundsOffset;

    offset = (rct_xyz16) {0, 0, height};
    switch (edge) {
        case EDGE_NE:
            bounds = (rct_xyz16) {1, 30, 0};
            boundsOffset = (rct_xyz16) {0, 1, height + 1};
            imageOffset = hasFence ? SPR_STATION_COVER_OFFSET_SE_NW_BACK_1 : SPR_STATION_COVER_OFFSET_SE_NW_BACK_0;
            break;
        case EDGE_SE:
            bounds = (rct_xyz16) {32, 32, 0};
            boundsOffset = (rct_xyz16) {1, 0, height + 31};
            imageOffset = SPR_STATION_COVER_OFFSET_NE_SW_FRONT;
            break;
        case EDGE_SW:
            bounds = (rct_xyz16) {32, 32, 0};
            boundsOffset = (rct_xyz16) {0, 0, height + 31};
            imageOffset = SPR_STATION_COVER_OFFSET_SE_NW_FRONT;
            break;
        case EDGE_NW:
            bounds = (rct_xyz16) {30, 1, 30};
            boundsOffset = (rct_xyz16) {1, 0, height + 1};
            imageOffset = hasFence ? SPR_STATION_COVER_OFFSET_NE_SW_BACK_1 : SPR_STATION_COVER_OFFSET_NE_SW_BACK_0;
            break;
    }

    if (RCT2_GLOBAL(0x00F441A0, uint32) != 0x20000000) {
        baseImageId &= 0x7FFFF;
    }

    if (baseImageId <= 0x20) {
        return false;
    }

    if (baseImageId & 0x40000000) {
        imageId = baseImageId & 0xBFFFFFFF + imageOffset;
        sub_98197C(imageId, offset.x, offset.y, bounds.x, bounds.y, bounds.z, offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z, get_current_rotation());

        uint32 edi = RCT2_GLOBAL(0x00F44198, uint32) & (0b11111 << 19);

        // weird jump
        imageId = baseImageId | edi + 0x3800000 + imageOffset + 12;
        sub_98199C(imageId, offset.x, offset.y, bounds.x, bounds.y, bounds.z, offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z, get_current_rotation());
        return true;
    }

    imageId = (baseImageId + imageOffset) | RCT2_GLOBAL(0x00F44198, uint32);
    sub_98197C(imageId, offset.x, offset.y, bounds.x, bounds.y, bounds.z, offset.z, boundsOffset.x, boundsOffset.y, boundsOffset.z, get_current_rotation());
    return true;
}

bool paint_utils_is_first_track(uint8 rideIndex, const rct_map_element * mapElement, rct_xy16 pos, uint8 trackType)
{
    if (mapElement->properties.track.type != TRACK_ELEM_BEGIN_STATION) {
        return false;
    }

    rct_xy16 delta = TileDirectionDelta[map_element_get_direction(mapElement)];
    rct_xy16 newPos = {
        .x = pos.x - delta.x,
        .y = pos.y - delta.y,
    };

    rct_map_element * nextTrack = map_get_track_element_at_from_ride_fuzzy(newPos.x, newPos.y, mapElement->base_height, rideIndex);

    return nextTrack == NULL;
}

bool paint_utils_is_last_track(uint8 rideIndex, const rct_map_element * mapElement, rct_xy16 pos, uint8 trackType)
{
    if (mapElement->properties.track.type != TRACK_ELEM_END_STATION) {
        return false;
    }

    rct_xy16 delta = TileDirectionDelta[map_element_get_direction(mapElement)];
    rct_xy16 newPos = {
        .x = pos.x + delta.x,
        .y = pos.y + delta.y,
    };

    rct_map_element * nextTrack = map_get_track_element_at_from_ride_fuzzy(newPos.x, newPos.y, mapElement->base_height, rideIndex);

    return nextTrack == NULL;
}

static void chairlift_paint_station_ne_sw(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    const rct_xy16 pos = {RCT2_GLOBAL(0x009DE56A, sint16), RCT2_GLOBAL(0x009DE56E, sint16)};
    uint8 trackType = mapElement->properties.track.type;
    rct_ride * ride = get_ride(rideIndex);
    uint32 imageId;

    bool isStart = paint_utils_is_first_track(rideIndex, mapElement, pos, trackType);;
    bool isEnd = paint_utils_is_last_track(rideIndex, mapElement, pos, trackType);

    const rct_ride_entrance_definition * entranceStyle = &RideEntranceDefinitions[ride->entrance_style];

    wooden_a_supports_paint_setup(0, 0, height, RCT2_GLOBAL(0x00F441A0, uint32), NULL);

    if (!isStart && !isEnd) {
        imageId = ((direction == 0) ? SPR_20502 : SPR_20504) | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());
    }

    imageId = SPR_METAL_FLOOR | RCT2_GLOBAL(0x00F4419C, uint32);
    sub_98197C(imageId, 0, 0, 32, 32, 1, height, 0, 0, height, get_current_rotation());

    bool hasFence = paint_util_has_fence(EDGE_NW, pos, mapElement, ride, get_current_rotation());
    if (hasFence) {
        imageId = SPR_METAL_FENCE_NW | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98199C(imageId, 0, 0, 32, 1, 7, height, 0, 2, height + 2, get_current_rotation());
    }
    paint_util_draw_station_covers(EDGE_NW, hasFence, entranceStyle, direction, height);

    if ((direction == 2 && isStart) || (direction == 0 && isEnd)) {
        imageId = SPR_METAL_FENCE_NE | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98199C(imageId, 0, 0, 1, 28, 7, height, 2, 2, height + 4, get_current_rotation());
    }

    if (RCT2_GLOBAL(0x0141E9DB, uint8) & 3) {
		hasFence = paint_util_has_fence(EDGE_SE, pos, mapElement, ride, get_current_rotation());
        if (hasFence) {
            imageId = SPR_METAL_FENCE_SE | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 32, 1, 27, height, 0, 30, height + 2, get_current_rotation());
        }
        paint_util_draw_station_covers(EDGE_SE, hasFence, entranceStyle, direction, height);
    }

    bool drawFrontColumn = true;
    bool drawBackColumn = true;
    if ((direction == 0 && isStart) || (direction == 2 && isEnd)) {
        imageId = SPR_METAL_FENCE_SW | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 0, 0, 1, 28, 27, height, 30, 2, height + 4, get_current_rotation());

        imageId = chairlift_bullwheel_frames[ride->chairlift_bullwheel_rotation / 16384] | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4, get_current_rotation());

        imageId = SPR_CHAIRLIFT_STATION_END_CAP_NE | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98199C(imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4, get_current_rotation());

        drawFrontColumn = false;
    } else if ((direction == 2 && isStart) || (direction == 0 && isEnd)) {
        imageId = chairlift_bullwheel_frames[ride->chairlift_bullwheel_rotation / 16384] | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4, get_current_rotation());

        imageId = SPR_CHAIRLIFT_STATION_END_CAP_SW | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98199C(imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4, get_current_rotation());

        drawBackColumn = false;
    }

    if (drawBackColumn) {
        imageId = SPR_CHAIRLIFT_STATION_COLUMN_NE_SW | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 0, 16, 1, 1, 7, height + 2, 1, 16, height + 2, get_current_rotation());
    }

    if (drawFrontColumn) {
        imageId = SPR_CHAIRLIFT_STATION_COLUMN_NE_SW | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 30, 16, 1, 1, 7, height + 2, 1, 16, height + 2, get_current_rotation()); // bound offset x is wrong?
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_push_tunnel_left(height, TUNNEL_FLAT);
    paint_util_set_support_height(height + 32, 0x20);
}

static void chairlift_paint_station_se_nw(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    const rct_xy16 pos = {RCT2_GLOBAL(0x009DE56A, sint16), RCT2_GLOBAL(0x009DE56E, sint16)};
    uint8 trackType = mapElement->properties.track.type;
    rct_ride * ride = get_ride(rideIndex);
    uint32 imageId;

    bool isStart = paint_utils_is_first_track(rideIndex, mapElement, pos, trackType);;
    bool isEnd = paint_utils_is_last_track(rideIndex, mapElement, pos, trackType);

    const rct_ride_entrance_definition * entranceStyle = &RideEntranceDefinitions[ride->entrance_style];

    wooden_a_supports_paint_setup(1, 0, height, RCT2_GLOBAL(0x00F441A0, uint32), NULL);

    if (!isStart && !isEnd) {
        imageId = ((direction == 1) ? SPR_20503 : SPR_20505) | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());
    }

    imageId = SPR_METAL_FLOOR | RCT2_GLOBAL(0x00F4419C, uint32);
    sub_98197C(imageId, 0, 0, 32, 32, 1, height, 0, 0, height, get_current_rotation());

    bool hasFence = paint_util_has_fence(EDGE_NE, pos, mapElement, ride, get_current_rotation());
    if (hasFence) {
        imageId = SPR_METAL_FENCE_NE | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98199C(imageId, 0, 0, 1, 32, 7, height, 2, 0, height + 2, get_current_rotation());
    }
    paint_util_draw_station_covers(EDGE_NE, hasFence, entranceStyle, direction, height);

    if ((direction == 1 && isStart) || (direction == 3 && isEnd)) {
        imageId = SPR_METAL_FENCE_NW | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98199C(imageId, 0, 0, 28, 1, 7, height, 2, 2, height + 4, get_current_rotation());
    }

    if (RCT2_GLOBAL(0x0141E9DB, uint8) & 3) {
        hasFence = paint_util_has_fence(EDGE_SW, pos, mapElement, ride, get_current_rotation());
        if (hasFence) {
            imageId = SPR_METAL_FENCE_SW | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 1, 32, 27, height, 30, 0, height + 2, get_current_rotation());
        }
        paint_util_draw_station_covers(EDGE_SW, hasFence, entranceStyle, direction, height);
    }

    bool drawRightColumn = true;
    bool drawLeftColumn = true;
    if ((direction == 1 && isStart) || (direction == 3 && isEnd)) {
        imageId = chairlift_bullwheel_frames[ride->chairlift_bullwheel_rotation / 16384] | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4, get_current_rotation());

        imageId = SPR_CHAIRLIFT_STATION_END_CAP_SE | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98199C(imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4, get_current_rotation());

        drawLeftColumn = false;
    } else if ((direction == 3 && isStart) || (direction == 1 && isEnd)) {
        imageId = SPR_METAL_FENCE_SE | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 0, 0, 28, 1, 27, height, 2, 30, height + 4, get_current_rotation());

        imageId = chairlift_bullwheel_frames[ride->chairlift_bullwheel_rotation / 16384] | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4, get_current_rotation());

        imageId = SPR_CHAIRLIFT_STATION_END_CAP_NW | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98199C(imageId, 0, 0, 4, 4, 26, height, 14, 14, height + 4, get_current_rotation());

        drawRightColumn = false;
    }

    if (drawLeftColumn) {
        imageId = SPR_CHAIRLIFT_STATION_COLUMN_SE_NW | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 16, 0, 1, 1, 7, height + 2, 16, 1, height + 2, get_current_rotation());
    }

    if (drawRightColumn) {
        imageId = SPR_CHAIRLIFT_STATION_COLUMN_SE_NW | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 16, 30, 1, 1, 7, height + 2, 16, 1, height + 2, get_current_rotation()); // bound offset x is wrong?

        paint_util_push_tunnel_right(height, TUNNEL_FLAT);
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_support_height(height + 32, 0x20);
}

/** rct2: 0x00744068 */
static void chairlift_paint_station(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    if (direction % 2) {
        chairlift_paint_station_se_nw(rideIndex, trackSequence, direction, height, mapElement);
    } else {
        chairlift_paint_station_ne_sw(rideIndex, trackSequence, direction, height, mapElement);
    }
}

static void chairlift_paint_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    uint32 imageId;
    if (direction & 1) {
        imageId = SPR_CHAIRLIFT_CABLE_FLAT_SE_NW | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());
        paint_util_push_tunnel_right(height, TUNNEL_FLAT);
    } else {
        imageId = SPR_CHAIRLIFT_CABLE_FLAT_SW_NE | RCT2_GLOBAL(0x00F44198, uint32);
        sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());
        paint_util_push_tunnel_left(height, TUNNEL_FLAT);
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_support_height(height + 32, 0x20);
}

/** rct2: 0x00743FD8 */
static void chairlift_paint_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    uint32 imageId;

    switch (direction) {
        case 0:
            imageId = SPR_CHAIRLIFT_CABLE_UP_SW_NE | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());
            paint_util_push_tunnel_left(height - 8, TUNNEL_7);
            break;

        case 1:
            imageId = SPR_CHAIRLIFT_CABLE_UP_NW_SE | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());
            paint_util_push_tunnel_right(height + 8, TUNNEL_UP);
            break;

        case 2:
            imageId = SPR_CHAIRLIFT_CABLE_UP_NE_SW | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());
            paint_util_push_tunnel_left(height + 8, TUNNEL_UP);
            break;

        case 3:
            imageId = SPR_CHAIRLIFT_CABLE_UP_SE_NW | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());
            paint_util_push_tunnel_right(height - 8, TUNNEL_7);
            break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_support_height(height + 56, 0x20);
}

/** rct2: 0x00743FD8 */
static void chairlift_paint_flat_to_25_deg_up(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    uint32 imageId;

    switch (direction) {
        case 0:
            imageId = SPR_20508 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());

            imageId = SPR_20520 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_left(height, TUNNEL_FLAT);
            break;

        case 1:
            imageId = SPR_20509 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());

            imageId = SPR_20521 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_right(height, TUNNEL_UP);
            break;

        case 2:
            imageId = SPR_20510 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());

            imageId = SPR_20522 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_left(height, TUNNEL_UP);
            break;

        case 3:
            imageId = SPR_20511 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());

            imageId = SPR_20523 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_right(height, TUNNEL_FLAT);
            break;
    }

    chairlift_paint_util_draw_supports(SEGMENT_4, height);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_support_height(height + 48, 0x20);
}

/** rct2: 0x00743FF8 */
static void chairlift_paint_25_deg_up_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    uint32 imageId;

    switch (direction) {
        case 0:
            imageId = SPR_20512 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());

            imageId = SPR_20524 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_left(height - 8, TUNNEL_FLAT);
            break;

        case 1:
            imageId = SPR_20513 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());

            imageId = SPR_20525 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_right(height + 8, TUNNEL_14);
            break;

        case 2:
            imageId = SPR_20514 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 32, 6, 2, height, 0, 13, height + 28, get_current_rotation());

            imageId = SPR_20526 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_left(height + 8, TUNNEL_14);
            break;

        case 3:
            imageId = SPR_20515 | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 6, 32, 2, height, 13, 0, height + 28, get_current_rotation());

            imageId = SPR_20527 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 4, 4, 25, height, 14, 14, height + 1, get_current_rotation());

            paint_util_push_tunnel_right(height - 8, TUNNEL_FLAT);
            break;

    }

    chairlift_paint_util_draw_supports(SEGMENT_4, height);
    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_support_height(height + 40, 0x20);
}

/** rct2: 0x00744008 */
static void chairlift_paint_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    chairlift_paint_25_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x00744018 */
static void chairlift_paint_flat_to_25_deg_down(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    chairlift_paint_25_deg_up_to_flat(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x00744028 */
static void chairlift_paint_25_deg_down_to_flat(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    chairlift_paint_flat_to_25_deg_up(rideIndex, trackSequence, (direction + 2) % 4, height, mapElement);
}

/** rct2: 0x00744038 */
static void chairlift_paint_left_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    uint32 imageId;

    switch (direction) {
        case 0:
            imageId = SPR_CHAIRLIFT_CORNER_NW_SW | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 16, 16, 2, height, 16, 0, height + 28, get_current_rotation());

            imageId = SPR_20532 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 16, 4, height, get_current_rotation());

            imageId = SPR_20536 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 28, 4, height, get_current_rotation());

            paint_util_push_tunnel_left(height, TUNNEL_FLAT);
            chairlift_paint_util_draw_supports(SEGMENT_5 | SEGMENT_7, height);
            break;

        case 1:
            imageId = SPR_CHAIRLIFT_CORNER_NW_NE | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 16, 16, 2, height, 0, 0, height + 28, get_current_rotation());

            imageId = SPR_20533 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 16, 4, height, get_current_rotation());

            imageId = SPR_20537 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 4, 16, height, get_current_rotation());

            chairlift_paint_util_draw_supports(SEGMENT_5 | SEGMENT_6, height);
            break;

        case 2:
            imageId = SPR_CHAIRLIFT_CORNER_SE_NE | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 16, 16, 2, height, 0, 16, height + 28, get_current_rotation());

            imageId = SPR_20534 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 4, 16, height, get_current_rotation());

            imageId = SPR_20538 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 16, 28, height, get_current_rotation());

            paint_util_push_tunnel_right(height, TUNNEL_FLAT);
            chairlift_paint_util_draw_supports(SEGMENT_6 | SEGMENT_8, height);
            break;

        case 3:
            imageId = SPR_CHAIRLIFT_CORNER_SW_SE | RCT2_GLOBAL(0x00F44198, uint32);
            sub_98197C(imageId, 0, 0, 16, 16, 2, height, 16, 16, height + 28, get_current_rotation());

            imageId = SPR_20535 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 28, 16, height, get_current_rotation());

            imageId = SPR_20539 | RCT2_GLOBAL(0x00F4419C, uint32);
            sub_98197C(imageId, 0, 0, 2, 2, 27, height, 16, 28, height, get_current_rotation());

            paint_util_push_tunnel_left(height, TUNNEL_FLAT);
            paint_util_push_tunnel_right(height, TUNNEL_FLAT);
            chairlift_paint_util_draw_supports(SEGMENT_7 | SEGMENT_8, height);
            break;
    }

    paint_util_set_segment_support_height(SEGMENTS_ALL, 0xFFFF, 0);
    paint_util_set_support_height(height + 32, 0x20);
}

/** rct2: 0x00744048 */
static void chairlift_paint_right_quarter_turn_1_tile(uint8 rideIndex, uint8 trackSequence, uint8 direction, int height, rct_map_element * mapElement)
{
    chairlift_paint_left_quarter_turn_1_tile(rideIndex, trackSequence, (direction + 3) % 4, height, mapElement);
}

/* 0x008AAA0C */
TRACK_PAINT_FUNCTION get_track_paint_function_chairlift(int trackType, int direction)
{
    switch (trackType) {
        case TRACK_ELEM_BEGIN_STATION:
        case TRACK_ELEM_MIDDLE_STATION:
        case TRACK_ELEM_END_STATION:
            return chairlift_paint_station;

        case TRACK_ELEM_FLAT:
            return chairlift_paint_flat;

        case TRACK_ELEM_FLAT_TO_25_DEG_UP:
            return chairlift_paint_flat_to_25_deg_up;
        case TRACK_ELEM_25_DEG_UP:
            return chairlift_paint_25_deg_up;
        case TRACK_ELEM_25_DEG_UP_TO_FLAT:
            return chairlift_paint_25_deg_up_to_flat;

        case TRACK_ELEM_FLAT_TO_25_DEG_DOWN:
            return chairlift_paint_flat_to_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN:
            return chairlift_paint_25_deg_down;
        case TRACK_ELEM_25_DEG_DOWN_TO_FLAT:
            return chairlift_paint_25_deg_down_to_flat;

        case TRACK_ELEM_LEFT_QUARTER_TURN_1_TILE:
            return chairlift_paint_left_quarter_turn_1_tile;
        case TRACK_ELEM_RIGHT_QUARTER_TURN_1_TILE:
            return chairlift_paint_right_quarter_turn_1_tile;
    }

    return NULL;
}
