// input_manager.cpp declares a class to interpret keyboard and mouse input
// source file created by Catastrophe 9/11/2017

#include <allegro5\allegro5.h>

class InputManager
{
public: 
	InputManager();

	void updateInputState();
	// precondition: assumed to be called once per frame
	// postcondition: updates internal input arrays which are required for other class functions

	int getMouseX();
	int getMouseY();
	// precondition: updateInputState() is being called once per frame
	// returns: the pixel the mouse is currently pointing at

	bool isKeyDown(int key);
	// precondition: updateInputState() is being called once per frame
	// returns: true if the key is currently held down, false otherwise

	bool isKeyJustDown(int key);
	// precondition: updateInputState() is being called once per frame
	// returns: true if the key was just pressed on this frame, false otherwise

private:
	ALLEGRO_KEYBOARD_STATE currentKeyboardState;
	ALLEGRO_MOUSE_STATE currentMouseState;
	ALLEGRO_KEYBOARD_STATE previousKeyboardState;
	ALLEGRO_MOUSE_STATE previousMouseState;
};