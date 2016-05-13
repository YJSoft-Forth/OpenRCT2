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

#include "../addresses.h"
#include "../audio/audio.h"
#include "../config.h"
#include "../game.h"
#include "../editor.h"
#include "../input.h"
#include "../scenario.h"
#include "../sprites.h"
#include "../localisation/localisation.h"
#include "../interface/themes.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../platform/platform.h"
#include "../ride/track_data.h"
#include "../title.h"
#include "../util/util.h"
#include "../world/footpath.h"
#include "../world/scenery.h"
#include "error.h"

enum {
	WIDX_PREVIOUS_IMAGE,		// 1
	WIDX_PREVIOUS_STEP_BUTTON,	// 2
	WIDX_NEXT_IMAGE,			// 4
	WIDX_NEXT_STEP_BUTTON,		// 8
};

static rct_widget window_editor_bottom_toolbar_widgets[] = {
	{ WWT_IMGBTN, 0, 0, 199, 0, 33, 0xFFFFFFFF, 0xFFFF },			// 1		0x9A9958
	{ WWT_FLATBTN, 0, 2, 197, 2, 31, 0xFFFFFFFF, 0xFFFF },			// 2		0x9A9968
	{ WWT_IMGBTN, 0, 440, 639, 0, 33, 0xFFFFFFFF, 0xFFFF },			// 4		0x9A9978
	{ WWT_FLATBTN, 0, 442, 637, 2, 31, 0xFFFFFFFF, 0xFFFF },		// 8		0x9A9988
	{ WIDGETS_END },
};

static void window_editor_bottom_toolbar_mouseup(rct_window *w, int widgetIndex);
static void window_editor_bottom_toolbar_invalidate(rct_window *w);
static void window_editor_bottom_toolbar_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_editor_bottom_toolbar_jump_back_to_object_selection();
static void window_editor_bottom_toolbar_jump_back_to_landscape_editor();
static void window_editor_bottom_toolbar_jump_back_to_invention_list_set_up();
static void window_editor_bottom_toolbar_jump_back_to_options_selection();

static void window_editor_bottom_toolbar_jump_forward_from_object_selection();
static void window_editor_bottom_toolbar_jump_forward_to_invention_list_set_up();
static void window_editor_bottom_toolbar_jump_forward_to_options_selection();
static void window_editor_bottom_toolbar_jump_forward_to_objective_selection();
static void window_editor_bottom_toolbar_jump_forward_to_save_scenario();

static rct_window_event_list window_editor_bottom_toolbar_events = {
	NULL,
	window_editor_bottom_toolbar_mouseup,	//0x0066f5ae,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_editor_bottom_toolbar_invalidate, //0x0066f1c9,
	window_editor_bottom_toolbar_paint, //0x0066f25c,
	NULL
};

static EMPTY_ARGS_VOID_POINTER *previous_button_mouseup_events[] = {
	NULL,
	window_editor_bottom_toolbar_jump_back_to_object_selection,
	window_editor_bottom_toolbar_jump_back_to_landscape_editor,
	window_editor_bottom_toolbar_jump_back_to_invention_list_set_up,
	window_editor_bottom_toolbar_jump_back_to_options_selection,
	NULL,
	window_editor_bottom_toolbar_jump_back_to_object_selection,
	NULL
};

static EMPTY_ARGS_VOID_POINTER *next_button_mouseup_events[] = {
	window_editor_bottom_toolbar_jump_forward_from_object_selection,
	window_editor_bottom_toolbar_jump_forward_to_invention_list_set_up,
	window_editor_bottom_toolbar_jump_forward_to_options_selection,
	window_editor_bottom_toolbar_jump_forward_to_objective_selection,
	window_editor_bottom_toolbar_jump_forward_to_save_scenario,
	NULL,
	NULL,
	NULL
};

static void sub_6DFED0();

/**
* Creates the main editor top toolbar window.
* rct2: 0x0066F052 (part of 0x0066EF38)
*/
void window_editor_bottom_toolbar_open()
{
	rct_window* window;

	window = window_create(0, gScreenHeight - 32,
		gScreenWidth, 32,
		&window_editor_bottom_toolbar_events,
		WC_BOTTOM_TOOLBAR, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_BACKGROUND);
	window->widgets = window_editor_bottom_toolbar_widgets;

	window->enabled_widgets |=
		(1 << WIDX_PREVIOUS_STEP_BUTTON) |
		(1 << WIDX_NEXT_STEP_BUTTON) |
		(1 << WIDX_PREVIOUS_IMAGE) |
		(1 << WIDX_NEXT_IMAGE);

	window_init_scroll_widgets(window);
	sub_6DFED0();
}

/**
*
*  rct2: 0x0066F619
*/
void window_editor_bottom_toolbar_jump_back_to_object_selection() {
	window_close_all();
	g_editor_step = EDITOR_STEP_OBJECT_SELECTION;
	gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x006DFED0
 */
static void sub_6DFED0()
{
	for (int i = 0; i < 56; i++) {
		gResearchedSceneryItems[i] = 0xFFFFFFFF;
	}
}

/**
*
*  rct2: 0x0066F62C
*/
void window_editor_bottom_toolbar_jump_back_to_landscape_editor() {
	window_close_all();
	sub_6DFED0();
	scenery_set_default_placement_configuration();
	g_editor_step = EDITOR_STEP_LANDSCAPE_EDITOR;
	window_map_open();
	gfx_invalidate_screen();
}

/**
*
*  rct2: 0x0066F64E
*/
void window_editor_bottom_toolbar_jump_back_to_invention_list_set_up() {
	window_close_all();
	window_editor_inventions_list_open();
	g_editor_step = EDITOR_STEP_INVENTIONS_LIST_SET_UP;
	gfx_invalidate_screen();
}

/**
*
*  rct2: 0x0066F666
*/
void window_editor_bottom_toolbar_jump_back_to_scenario_options() {
	window_close_all();
	window_editor_scenario_options_open();
	g_editor_step = EDITOR_STEP_OPTIONS_SELECTION;
	gfx_invalidate_screen();
}

/**
*
*  rct2: 0x0066F64E
*/
void window_editor_bottom_toolbar_jump_back_to_options_selection() {
	window_close_all();
	window_editor_scenario_options_open();
	g_editor_step = EDITOR_STEP_OPTIONS_SELECTION;
	gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x006AB1CE
 */
bool window_editor_bottom_toolbar_check_object_selection()
{
	rct_window *w;

	int missingObjectType = editor_check_object_selection();
	if (missingObjectType < 0) {
		window_close_by_class(WC_EDITOR_OBJECT_SELECTION);
		return true;
	}

	window_error_open(STR_INVALID_SELECTION_OF_OBJECTS, gGameCommandErrorText);
	w = window_find_by_class(WC_EDITOR_OBJECT_SELECTION);
	if (w != NULL) {
		// Click tab with missing object
		window_event_mouse_up_call(w, 4 + missingObjectType);
	}
	return false;
}

/**
 *
 *  rct2: 0x0066F6E3
 */
static void sub_66F6E3()
{
	for (int i = 0; i < 4; i++) {
		gResearchedRideTypes[i] = 0xFFFFFFFF;
	}

	for (int i = 0; i < countof(RideTypePossibleTrackConfigurations); i++) {
		gResearchedTrackTypesA[i] = (RideTypePossibleTrackConfigurations[i]         ) & 0xFFFFFFFFULL;
		gResearchedTrackTypesB[i] = (RideTypePossibleTrackConfigurations[i] >> 32ULL) & 0xFFFFFFFFULL;
	}

	for (int i = 0; i < 8; i++) {
		gResearchedRideEntries[i] = 0xFFFFFFFF;
	}

	window_new_ride_open();
	RCT2_GLOBAL(0x0141F570, uint8) = 6;
	gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x0066F6B0
 */
void window_editor_bottom_toolbar_jump_forward_from_object_selection()
{
	if (!window_editor_bottom_toolbar_check_object_selection())
		return;

	if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) {
		sub_66F6E3();
	} else {
		sub_6DFED0();
		scenery_set_default_placement_configuration();
		RCT2_GLOBAL(0x00141F570, uint8) = 1;
		window_map_open();
		gfx_invalidate_screen();
	}
}

/**
*
*  rct2: 0x0066F758
*/
void window_editor_bottom_toolbar_jump_forward_to_invention_list_set_up() {
	if (editor_check_park()) {
		window_close_all();
		window_editor_inventions_list_open();
		g_editor_step = EDITOR_STEP_INVENTIONS_LIST_SET_UP;
	} else {
		window_error_open(STR_CANT_ADVANCE_TO_NEXT_EDITOR_STAGE, gGameCommandErrorText);
	}

	gfx_invalidate_screen();
}

/**
*
*  rct2: 0x0066f790
*/
void window_editor_bottom_toolbar_jump_forward_to_options_selection() {
	window_close_all();
	window_editor_scenario_options_open();
	g_editor_step = EDITOR_STEP_OPTIONS_SELECTION;
	gfx_invalidate_screen();
}

/**
*
*  rct2: 0x0066f7a8
*/
void window_editor_bottom_toolbar_jump_forward_to_objective_selection() {
	window_close_all();
	window_editor_objective_options_open();
	g_editor_step = EDITOR_STEP_OBJECTIVE_SELECTION;
	gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x0066F7C0
 */
void window_editor_bottom_toolbar_jump_forward_to_save_scenario()
{
	rct_s6_info *s6Info = (rct_s6_info*)0x0141F570;

	if (!scenario_prepare_for_save()) {
		window_error_open(STR_UNABLE_TO_SAVE_SCENARIO_FILE, gGameCommandErrorText);
		gfx_invalidate_screen();
		return;
	}

	window_close_all();
	window_loadsave_open(LOADSAVETYPE_SAVE | LOADSAVETYPE_SCENARIO, s6Info->name);
}

/**
*
*  rct2: 0x0066F5AE
*/
static void window_editor_bottom_toolbar_mouseup(rct_window *w, int widgetIndex)
{
	if (widgetIndex == WIDX_PREVIOUS_STEP_BUTTON) {
		if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) ||
			(gSpriteListCount[SPRITE_LIST_NULL] == MAX_SPRITES && !(gParkFlags & PARK_FLAGS_18))
		) {
			previous_button_mouseup_events[g_editor_step]();
		}
	} else if (widgetIndex == WIDX_NEXT_STEP_BUTTON) {
		next_button_mouseup_events[g_editor_step]();
	}
}

void hide_previous_step_button() {
	window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_STEP_BUTTON].type = WWT_EMPTY;
	window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].type = WWT_EMPTY;
}

void hide_next_step_button() {
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_STEP_BUTTON].type = WWT_EMPTY;
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].type = WWT_EMPTY;
}

/**
*
*  rct2: 0x0066F1C9
*/
void window_editor_bottom_toolbar_invalidate(rct_window *w)
{
	colour_scheme_update_by_class(w, (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) ? WC_EDITOR_SCENARIO_BOTTOM_TOOLBAR : WC_EDITOR_TRACK_BOTTOM_TOOLBAR);

	uint16 screenWidth = gScreenWidth;
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].left = screenWidth - 200;
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].right = screenWidth - 1;
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_STEP_BUTTON].left = screenWidth - 198;
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_STEP_BUTTON].right = screenWidth - 3;

	window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_STEP_BUTTON].type = WWT_FLATBTN;
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_STEP_BUTTON].type = WWT_FLATBTN;
	window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].type = WWT_IMGBTN;
	window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].type = WWT_IMGBTN;

	if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
		hide_previous_step_button();
		hide_next_step_button();
	} else {
		if (g_editor_step == EDITOR_STEP_OBJECT_SELECTION) {
			hide_previous_step_button();
		} else if (g_editor_step == EDITOR_STEP_ROLLERCOASTER_DESIGNER) {
			hide_next_step_button();
		} else if (!(gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)) {
			if (gSpriteListCount[SPRITE_LIST_NULL] != MAX_SPRITES || gParkFlags & PARK_FLAGS_18) {
				hide_previous_step_button();
			}
		}
	}
}

/**
*
*  rct2: 0x0066F25C
*/
void window_editor_bottom_toolbar_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	bool drawPreviousButton = false;
	bool drawNextButton = false;

	if (g_editor_step == EDITOR_STEP_OBJECT_SELECTION) {
		drawNextButton = true;
	}
	else if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) {
		drawPreviousButton = true;
	}
	else if (gSpriteListCount[SPRITE_LIST_NULL] != MAX_SPRITES) {
		drawNextButton = true;
	}
	else if (gParkFlags & PARK_FLAGS_18) {
		drawNextButton = true;
	}
	else {
		drawPreviousButton = true;
	}

	if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
		if (drawPreviousButton) {
			gfx_fill_rect(dpi,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].left + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].top + w->y,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].right + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].bottom + w->y, 0x2000033);
		}

		if ((drawPreviousButton || drawNextButton) && g_editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER) {
			gfx_fill_rect(dpi,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].left + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].top + w->y,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].right + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].bottom + w->y, 0x2000033);
		}
	}

	window_draw_widgets(w, dpi);

	if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {


		if (drawPreviousButton) {
			gfx_fill_rect_inset(dpi,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].left + 1 + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].top + 1 + w->y,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].right - 1 + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].bottom - 1 + w->y,
				w->colours[1], 0x30);
		}

		if ((drawPreviousButton || drawNextButton) && g_editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER) {
			gfx_fill_rect_inset(dpi,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].left + 1 + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].top + 1 + w->y,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].right - 1 + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].bottom - 1 + w->y,
				w->colours[1], 0x30);
		}

		short stateX =
			(window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].right +
			window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].left) / 2 + w->x;
		short stateY = w->height - 0x0C + w->y;
		gfx_draw_string_centred(dpi, STR_OBJECT_SELECTION_STEP + g_editor_step,
			stateX, stateY, (w->colours[2] & 0x7F) | 0x20, 0);

		if (drawPreviousButton) {
			gfx_draw_sprite(dpi, SPR_PREVIOUS,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].left + 6 + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].top + 6 + w->y, 0);

			int textColour = w->colours[1] & 0x7F;
			if (gHoverWidget.window_classification == WC_BOTTOM_TOOLBAR &&
				gHoverWidget.widget_index == WIDX_PREVIOUS_STEP_BUTTON
			) {
				textColour = 2;
			}

			short textX = (window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].left + 30 +
				window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].right) / 2 + w->x;
			short textY = window_editor_bottom_toolbar_widgets[WIDX_PREVIOUS_IMAGE].top + 6 + w->y;

			short stringId = STR_OBJECT_SELECTION_STEP + g_editor_step - 1;
			if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
				stringId = STR_OBJECT_SELECTION_STEP;

			gfx_draw_string_centred(dpi, STR_BACK_TO_PREVIOUS_STEP, textX, textY, textColour, 0);
			gfx_draw_string_centred(dpi, stringId, textX, textY + 10, textColour, 0);
		}

		if ((drawPreviousButton || drawNextButton) && g_editor_step != EDITOR_STEP_ROLLERCOASTER_DESIGNER) {
			gfx_draw_sprite(dpi, SPR_NEXT,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].right - 29 + w->x,
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].top + 6 + w->y, 0);

			int textColour = w->colours[1] & 0x7F;

			if (gHoverWidget.window_classification == WC_BOTTOM_TOOLBAR &&
				gHoverWidget.widget_index == WIDX_NEXT_STEP_BUTTON
			) {
				textColour = 2;
			}

			short textX = (window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].left +
				window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].right - 30) / 2 + w->x;
			short textY = window_editor_bottom_toolbar_widgets[WIDX_NEXT_IMAGE].top + 6 + w->y;

			short stringId = STR_OBJECT_SELECTION_STEP + g_editor_step + 1;
			if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
				stringId = STR_ROLLERCOASTER_DESIGNER_STEP;

			gfx_draw_string_centred(dpi, STR_FORWARD_TO_NEXT_STEP, textX, textY, textColour, 0);
			gfx_draw_string_centred(dpi, stringId, textX, textY + 10, textColour, 0);

		}
	}
}
