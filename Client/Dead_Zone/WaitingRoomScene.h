#pragma once
#include "Scene.h"

class WaitingRoomScene : public Scene
{
public:
	WaitingRoomScene() {}
	~WaitingRoomScene() {}

	virtual void LoadScene();



private:
	virtual void Update();

};

