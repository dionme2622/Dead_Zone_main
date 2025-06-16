#pragma once
#include "Scene.h"
#include "GameObject.h"
#include "Material.h"
#include "MeshRenderer.h"

class LobbyScene : public Scene
{
public:
	LobbyScene() {}
	virtual ~LobbyScene() {}

	virtual void LoadScene();

	virtual void Update();

	void UpdateButton();

	void HandleIdInput();
private:
	vector<shared_ptr<GameObject>>		_buttons;
	array<shared_ptr<Material>, 2>	    _ButtonMaterials;
	
	shared_ptr<Material>				_backgroundMaterial;
	float								_currentTime = 0.0f;

	string								_inputId;
};

