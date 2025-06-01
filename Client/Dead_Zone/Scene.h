#pragma once

class GameObject;
class Camera;

enum
{
	MAX_LAYER = 32
};



class Scene
{
public:
	Scene() {};
	virtual ~Scene() = default;

	void Awake();
	void Start();
	virtual void Update();

	virtual void LateUpdate();
	virtual void FinalUpdate();

	void Render();

	void ClearRTV();

	void RenderShadow();
	void RenderDeferred();
	void RenderLights();
	void RenderFinal();

	void RenderForward();

	void RenderPostProcessing();

	void SetLayerName(uint8 index, const wstring& name);
	const wstring& IndexToLayerName(uint8 index) { return _layerNames[index]; }
	uint8 LayerNameToIndex(const wstring& name);
private:
	void PushLightData();

public:
	void AddGameObject(shared_ptr<GameObject> gameObject);
	void RemoveGameObject(shared_ptr<GameObject> gameObject);

	const vector<shared_ptr<GameObject>>& GetGameObjects() { return _gameObjects; }

	virtual void LoadScene() = 0;

public:
	shared_ptr<Camera> GetMainCamera() { return _cameras[0]; }



private:
	vector<shared_ptr<GameObject>>		_gameObjects;
	vector<shared_ptr<class Camera>>	_cameras;
	vector<shared_ptr<class Light>>		_lights;

	array<wstring, MAX_LAYER>			_layerNames;
	map<wstring, uint8>					_layerIndex;
};

