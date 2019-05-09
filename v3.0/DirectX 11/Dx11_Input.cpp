#include "Dx11_Input.h"


Dx11_Input::Dx11_Input()
{
}


Dx11_Input::~Dx11_Input()
{
}


void Dx11_Input::Initialize()
{
	for (unsigned int i = 0; i < 256; i++)
	{
		m_bKeys[i] = false;
	}
}

void Dx11_Input::KeyDown(unsigned int index)
{
	m_bKeys[index] = true;
}


void Dx11_Input::KeyUp(unsigned int index)
{
	m_bKeys[index] = false;
}

bool Dx11_Input::IsKeyDown(unsigned int index)
{
	return m_bKeys[index];
}