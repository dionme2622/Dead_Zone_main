#include "pch.h"
#include "SceneManager.h"
#include "Scene.h"

#include "Engine.h"
#include "Material.h"
#include "GameObject.h"
#include "MeshRenderer.h"
#include "Transform.h"
#include "Camera.h"
#include "Light.h"

#include "Resources.h"
#include "KeyInput.h"
#include "PlayerStats.h"
#include "Timer.h"

void SceneManager::Update()
{
	if (_activeScene == nullptr)
		return;
	

	_activeScene->Update();
	_activeScene->LateUpdate();
	_activeScene->FinalUpdate();

}

// TEMP
void SceneManager::Render()
{
	if (_activeScene)
		_activeScene->Render();
}

void SceneManager::LoadScene(SCENETYPE sceneName)
{
	switch (sceneName)
	{
	case SCENETYPE::ELOBBYSCENE:
		_lobbyScene = make_shared<LobbyScene>();
		_activeScene = static_pointer_cast<Scene>(_lobbyScene);
		break;
	case SCENETYPE::ESELECTROOMSCENE:
		_roomSelectScene = make_shared<RoomSelectScene>();
		_activeScene = static_pointer_cast<Scene>(_roomSelectScene);
		break;
	case SCENETYPE::EBATTLESCENE:
		_battleScene = make_shared<BattleScene>();
		_activeScene = static_pointer_cast<Scene>(_battleScene);
		break;
	case SCENETYPE::ERESULTSCENE:
		_resultScene = make_shared<ResultScene>();
		_activeScene = static_pointer_cast<Scene>(_resultScene);
		break;
	}

	_activeScene->LoadScene();
	_activeScene->Awake();
	_activeScene->Start();
}




