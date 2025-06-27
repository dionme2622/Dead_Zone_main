#pragma once
#include "Scene.h"
#include "LobbyScene.h"
#include "RoomSelectScene.h"
#include "BattleScene.h"
#include "ResultScene.h"

class Scene;
class LobbyScene;
class BattleScene;
class ResultScene;
class Player;

//enum
//{
//	MAX_LAYER = 32
//};

enum class SCENETYPE : uint8
{
	ELOBBYSCENE,
	ESELECTROOMSCENE,
	EBATTLESCENE,
	ERESULTSCENE,

	END
};

#define ACTIVESCENE GetSceneName()
#define LOBBYSCENE  LoadLobyyScene()
#define BATTLESCENE LoadBattleScene()


class SceneManager
{
	DECLARE_SINGLE(SceneManager);

public:
	void Update();
	void Render();
	void LoadScene(SCENETYPE sceneName);


	/*void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return _layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);*/
public:
	shared_ptr<Scene> GetActiveScene() { return _activeScene; }
	uint8 GetSceneName() { return static_cast<uint8>(_sceneName); }



private:
	shared_ptr<LobbyScene> _lobbyScene = make_shared<LobbyScene>();
	shared_ptr<RoomSelectScene> _roomSelectScene = make_shared<RoomSelectScene>();
	shared_ptr<BattleScene> _battleScene = make_shared<BattleScene>();
	shared_ptr<ResultScene> _resultScene = make_shared<ResultScene>();

private:
	shared_ptr<Scene>			_activeScene;
	SCENETYPE					_sceneName;

	/*array<wstring, MAX_LAYER>	_layerNames;
	map<wstring, uint8>			_layerIndex;*/

	shared_ptr<Player>			_player;
};

