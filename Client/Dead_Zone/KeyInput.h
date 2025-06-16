#pragma once

enum class KEY_TYPE
{
	UP = VK_UP,
	DOWN = VK_DOWN,
	LEFT = VK_LEFT,
	RIGHT = VK_RIGHT,

	W = 'W',
	A = 'A',
	S = 'S',
	X = 'X',
	D = 'D',

	Q = 'Q',
	E = 'E',
	Z = 'Z',
	C = 'C',

	B = 'B',

	KEY_1 = '1',
	KEY_2 = '2',
	KEY_3 = '3',
	KEY_4 = '4',

	CTRL = VK_CONTROL,
	SPACE = VK_SPACE,


	ESC = VK_ESCAPE,


	// TEST
	SHIFT = VK_LSHIFT,
	RETURN = VK_RETURN,
	TAB = VK_TAB,
	RIGHTCLICK = RI_MOUSE_BUTTON_2_DOWN,
	LEFTCLICK = RI_MOUSE_BUTTON_1_DOWN,

	F9 = VK_F9,
	BACK = VK_BACK
};

enum class KEY_STATE
{
	NONE,
	PRESS,
	DOWN,
	UP,
	END
};

enum
{
	KEY_TYPE_COUNT = static_cast<int32>(UINT8_MAX + 1),
	KEY_STATE_COUNT = static_cast<int32>(KEY_STATE::END),
};

class KeyInput
{
	DECLARE_SINGLE(KeyInput);

public:
	void Init(HWND hwnd);
	void Update();

	// 누르고 있을 때
	bool GetButton(KEY_TYPE key) { return GetState(key) == KEY_STATE::PRESS; }
	// 맨 처음 눌렀을 때
	bool GetButtonDown(KEY_TYPE key) { return GetState(key) == KEY_STATE::DOWN; }
	// 맨 처음 눌렀다 뗐을 때
	bool GetButtonUp(KEY_TYPE key) { return GetState(key) == KEY_STATE::UP; }

private:
	inline KEY_STATE GetState(KEY_TYPE key) { return _states[static_cast<uint8>(key)]; }

private:
	HWND _hwnd;
	vector<KEY_STATE> _states;

};

