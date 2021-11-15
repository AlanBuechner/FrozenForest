#ifndef INPUT_H
#define INPUT_H

#define BIT(n) (1 << (n))

enum Key
{
	A = BIT(0),
	B = BIT(1),
	X = BIT(10),
	Y = BIT(11),
	L = BIT(9),
	R = BIT(8),
	LEFT = BIT(5),
	RIGHT = BIT(4),
	UP = BIT(6),
	DOWN = BIT(7),
	START = BIT(3),
	SELECT = BIT(2)
};

namespace Input
{
	void Update();  
	bool IsKeyPressed(Key key);
	bool IsKeyDown(Key key);
}

#endif