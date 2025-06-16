#pragma once
#include "Scene.h"
#include "Material.h"


class RoomSelectScene : public Scene
{
public:
	RoomSelectScene() {};
	virtual ~RoomSelectScene() {}

	virtual void LoadScene();
	virtual void Update();

	void UpdateArrowPosition();

	virtual void LoadingSceneRender();



public:
	array<shared_ptr<Material>, 2>	    _ButtonMaterials;
	vector<shared_ptr<GameObject>>		_buttons;

	vector<shared_ptr<GameObject>> _roomBoxes;
	shared_ptr<GameObject> _arrowMark;
	int _hoveredRoomIndex = -1;
};

