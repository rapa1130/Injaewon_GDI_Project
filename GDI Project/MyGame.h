#pragma once
#include"NzWndBase.h"
#include"GameTimer.h"
#include"GameObject.h"
#include<vector>
#include<map>

#ifdef _DEBUG
#include<iostream>
#endif // _DEBUG

class GameObjectBase;

struct MOUSE_POS
{
	int x = 0;
	int y = 0;
	bool operator!=(const MOUSE_POS& other) const
	{
		return (x != other.x || y != other.y);
	}
};

class MyGame :public NzWndBase
{
	using Vector2f = learning::Vector2f;
public:
	MyGame();
	~MyGame() override;

	bool Initialize();
	void Run();
	void Finalize();

public:
	void CreateBullet(GameObjectBase* shooter, Vector2f targetPos);
	GameObject* GetPlayer() const { return (GameObject*)m_GameObjectPtrTable[0]; }
	void RegisterToDestroy(GameObjectBase* goBase);
	
	float GetFixedTimeScale() const;
	float GetGameTimeScale() const;
	float GetSlowFixedTimeScale() const;
	float GetRewindTime() const;

	void SetGameStop(bool bStop);
	bool IsGameStopped() const;

	bool IsRewinding() const;
		

private:
	void Update();
	void Render();

	void OnResize(int width, int height) override;
	void OnClose() override;

	void DrawUI();

	void OnMouseMove(int x, int y);
	void OnLButtonDown(int x, int y);
	void OnRButtonDown(int x, int y);

	void HandleKeyboardInput();
	void HandleMouseInput();


	void FixedUpdate();
	void LogicUpdate();


	void CreatePlayer();
	void CreateEnemy();
	

	void DestroyObject(GameObjectBase* goBase);
	void DestroyRegistedObjects();


	void UpdatePlayerInfo();
	void UpdateEnemyInfo();
	void UpdateCollision();

	bool IsTimeSlow() const;

private:
	int GetGoCount() const;

	HDC m_hFrontDC = nullptr;
	HDC m_hBackDC = nullptr;
	HBITMAP m_hBackBitmap = nullptr;
	HBITMAP m_hDefaultBitmap = nullptr;
	HBRUSH m_BgBrush = NULL;

	HBRUSH BRUSH_WHITE = NULL;
	HBRUSH BRUSH_BLUE = NULL;
	HPEN PEN_WHITE = NULL;
	HPEN PEN_BLUE = NULL;
	


	GameTimer* m_pGameTimer = nullptr;

	float m_fDeltaTime = 0.0f;
	float m_fFrameCount = 0.0f;
	float m_gameTimeScale = 1.0f;

	const float m_fixedTimeScale = 10.0f;
	const float m_slowFixedTimeScale = 1.0f;
	const float m_traceWindowTime = 1000.0f;
	const float m_rewindRecoverSpeed = 0.1f;

	const float m_TotalRewindTime = 2000.0f;
	float m_remainRewindTime;

	float m_totalElapsedTime = 0;

	GameObjectBase** m_GameObjectPtrTable = nullptr;
	std::vector<GameObjectBase*> m_destroyTable;
	MOUSE_POS m_MousePos = { 0, 0 };

	bool m_bRewinding = false;
	bool m_bGameStop = false;
};