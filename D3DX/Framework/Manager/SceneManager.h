#pragma once

class Scene
{
protected:
	void *	m_pData[10];

public:
	Scene() { ZeroMemory(m_pData, sizeof(void*) * 10); }
	virtual ~Scene() {}
	virtual void Release() = 0;
	virtual void Init() = 0;
	virtual void Render() = 0;
	virtual void Update() = 0;

	void SetData(int index, void * data) { m_pData[index] = data; }
};

class SceneManager
{
	SINGLETONE(SceneManager)

private:
	map<string,Scene*>		m_mScene;
	Scene*					m_pCurrentScene;
	vector<Scene*>			m_vecPrevScene;

public:
	void AddScene(string name, Scene * scene);
	void ChangeScene(string name, bool init = true);
	Scene * GetCurrentScene() { return m_pCurrentScene; }

	void Update();
	void Render();

	void Destroy();
};

#define SCENE SceneManager::GetInstance()