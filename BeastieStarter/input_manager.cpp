// input_manager.cpp implements a class to interpret keyboard and mouse input
// source file created by Catastrophe 9/11/2017

#include "input_manager.h"

InputManager::InputManager()
{
}

void InputManager::updateInputState()
{
	previousKeyboardState = currentKeyboardState;
	previousMouseState = currentMouseState;

	al_get_keyboard_state(&currentKeyboardState);
	al_get_mouse_state(&currentMouseState);
}

int InputManager::getMouseX()
{
	return currentMouseState.x;
}

int InputManager::getMouseY()
{
	return currentMouseState.y;
}

bool InputManager::isKeyDown(int key)
{
	return al_key_down(&currentKeyboardState, key);
}

bool InputManager::isKeyJustDown(int key)
{
	return al_key_down(&currentKeyboardState, key) && !al_key_down(&previousKeyboardState, key);
}