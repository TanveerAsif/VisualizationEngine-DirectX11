#pragma once
class Dx11_Input
{
public:
	Dx11_Input();
	~Dx11_Input();

	void Initialize();
	void KeyDown(unsigned int index);
	void KeyUp(unsigned int index);

	bool IsKeyDown(unsigned int index);

private:
	bool m_bKeys[256];
};

