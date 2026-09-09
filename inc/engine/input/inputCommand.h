#ifndef INPUTCOMMAND
#define INPUTCOMMAND

enum class InputCommandType {
	Move,
	Stop,
	Attack,
	Dash,
	UseItem,
	Action,
	NextQuickItem
};

enum class InputSource {
	Keyboard,
	Joystick
};

struct InputCommand {
	InputCommandType type;
	InputSource source;
	float angle = 0;
};

#endif