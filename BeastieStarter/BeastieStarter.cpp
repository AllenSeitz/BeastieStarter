// BeastieStarter.cpp implements a basic, graphical application launcher
// source file created by Allen Seitz 7/5/2019

#include <string>

#include "allegro5/allegro.h"
#include "allegro5/allegro_font.h"
#include "allegro5/allegro_image.h"
#include "allegro5/allegro_ttf.h"
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/config.h>

#include "input_manager.h"

int reverseLookupKeycodeFromString(const char* keyName, int defaultValue);
// precondition: keyName and defaultValue should be from the Allegro library, such as "ALLEGRO_KEY_LEFT"
// postcondition: returns the integer value that would cause al_keycode_to_name(int) to return keyName

int main()
{
	ALLEGRO_DISPLAY *Screen = NULL;
	ALLEGRO_EVENT_QUEUE *EventQueue = NULL;
	ALLEGRO_TIMER *RedrawTimer = NULL;
	InputManager input;

	bool quit = false;
	bool needRedraw = false;
	bool needLogic = false;
	int selectionIndex = 1;

	// initialize Allegro
	if (!al_init())
	{
		al_show_native_message_box(NULL, "Error!", "Allegro has failed to initialize.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	al_init_primitives_addon();
	al_install_keyboard();
	al_install_mouse();
	al_init_image_addon();
	al_init_font_addon();
	al_init_ttf_addon();

	// load the config file first
	ALLEGRO_CONFIG* config = al_load_config_file("config.txt");
	if (config == NULL)
	{
		al_show_native_message_box(NULL, "Error!", "Could not open config.txt.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	int resolutionX = atoi(al_get_config_value(config, NULL, "resolutionX"));
	int resolutionY = atoi(al_get_config_value(config, NULL, "resolutionY"));
	int refreshRate = atoi(al_get_config_value(config, NULL, "refreshRate"));
	int numberOfApplications = atoi(al_get_config_value(config, NULL, "numberOfApplications"));
	const char* leftButtonTemp = al_get_config_value(config, NULL, "selectionLeft");
	const char* rightButtonTemp = al_get_config_value(config, NULL, "selectionRight");
	const char* selectionButtonTemp = al_get_config_value(config, NULL, "selectionChoose");
	int leftButton = reverseLookupKeycodeFromString(leftButtonTemp, ALLEGRO_KEY_LEFT);
	int rightButton = reverseLookupKeycodeFromString(rightButtonTemp, ALLEGRO_KEY_RIGHT);
	int selectionButton = reverseLookupKeycodeFromString(selectionButtonTemp, ALLEGRO_KEY_ENTER);
	al_destroy_config(config);

	// translate "ALLEGRO_KEY_LEFT" into its enumerated value, because this makes the config file much nicer

	// sanity checks on the config file
	if (numberOfApplications < 2)
	{
		al_show_native_message_box(NULL, "Error!", "Too few applications. Please set numberOfApplications to be between 2 and 9 inclusive", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	if (numberOfApplications > 9)
	{
		al_show_native_message_box(NULL, "Error!", "Too many applications. Please set numberOfApplications to be between 2 and 9 inclusive.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	if (resolutionX == 0 || resolutionY == 0 || refreshRate == 0)
	{
		resolutionX = 800;
		resolutionY = 600;
		refreshRate = 60;
	}

	// create the display using the last known preferences (or a fullscreen window by default)
	al_set_new_display_flags(ALLEGRO_GENERATE_EXPOSE_EVENTS | ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE);
	Screen = al_create_display(resolutionX, resolutionY);
	if (Screen == NULL)
	{
		al_show_native_message_box(Screen, "Error!", "Failed to create the display.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	al_set_window_position(Screen, 100, 100);

	// creating the event queue should never fail, but the tutorial checks anyway
	EventQueue = al_create_event_queue();
	if (EventQueue == NULL)
	{
		al_show_native_message_box(Screen, "Error!", "Failed to create an event queue?", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	al_register_event_source(EventQueue, al_get_display_event_source(Screen));
	al_register_event_source(EventQueue, al_get_keyboard_event_source());

	// creating the 60 fps timer for flipping the backbuffer should also never fail
	RedrawTimer = al_create_timer(1.0 / refreshRate);
	if (RedrawTimer == NULL)
	{
		al_show_native_message_box(Screen, "Error!", "Failed to create a timer.", 0, 0, ALLEGRO_MESSAGEBOX_ERROR);
		return -1;
	}
	al_register_event_source(EventQueue, al_get_timer_event_source(RedrawTimer));
	al_start_timer(RedrawTimer);

	// create input
	al_install_keyboard();
	al_install_mouse();
	input.updateInputState();

	// load the icons for each application
	ALLEGRO_BITMAP* applicationIcons[10];
	for (int i = 0; i < 10; i++)
	{
		char filename[] = "0.png";
		filename[0] = i + '0';
		applicationIcons[i] = al_load_bitmap(filename); // its fine if this returns null
	}

	while (!quit)
	{
		ALLEGRO_EVENT ev;

		// WINDOWS EVENT LOOP AND INPUT
		while (!al_is_event_queue_empty(EventQueue))
		{
			al_wait_for_event(EventQueue, &ev);

			// this happens when the close box is clicked on the window
			if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
			{
				quit = true;
			}

			// this utility only has one timer, which is set to 60 fps
			if (ev.type == ALLEGRO_EVENT_TIMER && ev.timer.source == RedrawTimer)
			{
				needRedraw = true;
				needLogic = true; // this could be changed to make the program only react to input
				input.updateInputState();
			}

			// an alternate way to quit the program just in case the close box is unresponsive or unreachable
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN)
			{
				if (ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
				{
					quit = true;
				}
			}
		}

		// MAIN LOGIC
		if (needLogic)
		{
			needLogic = false;

			if (input.isKeyJustDown(leftButton))
			{
				selectionIndex = selectionIndex <= 1 ? numberOfApplications : selectionIndex - 1;
			}
			if (input.isKeyJustDown(rightButton))
			{
				selectionIndex = selectionIndex >= numberOfApplications ? 1 : selectionIndex + 1;
			}
			if (input.isKeyJustDown(selectionButton))
			{
				char filename[] = "0.bat";
				filename[0] = selectionIndex + '0';
				system(filename);
			}
		}

		// MAIN RENDER
		if (needRedraw == true && al_is_event_queue_empty(EventQueue) == true)
		{
			needRedraw = false;

			al_clear_to_color(al_map_rgb(0, 0, 0));
			if (applicationIcons[selectionIndex] == NULL)
			{
				al_draw_textf(NULL, al_map_rgb(255, 255, 255), 40, 40, 0, "Missing graphic (%d.png) for application %d.", selectionIndex, selectionIndex);
			}
			else
			{
				al_draw_scaled_bitmap(applicationIcons[selectionIndex], 0, 0, 
					al_get_bitmap_width(applicationIcons[selectionIndex]), al_get_bitmap_height(applicationIcons[selectionIndex]), 0, 0, resolutionX, resolutionY, 0);
			}

			al_flip_display();
		}
	}

	al_destroy_timer(RedrawTimer);
	al_destroy_event_queue(EventQueue);
	al_destroy_display(Screen);

	return 0;
}

int reverseLookupKeycodeFromString(const char* keyName, int defaultValue)
{
	for (int i = 0; i < ALLEGRO_KEY_MAX; i++)
	{
		const char* temp = al_keycode_to_name(i);
		if (strcmp(keyName, al_keycode_to_name(i)) == 0)
		{
			return i;
		}
	}

	return defaultValue;
}
