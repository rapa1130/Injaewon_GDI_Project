#include "MyGame.h"
#include"Utillity.h"
#include<assert.h>
#include"Player.h"
#include"Enemy.h"
#include"Bullet.h"

#ifdef _DEBUG
#include<iostream>
#endif 

constexpr int MAX_GAME_OBJECT_COUNT = 1000;

MyGame::MyGame()
    :m_remainRewindTime(m_TotalRewindTime)
{
    BRUSH_WHITE = CreateSolidBrush(RGB(255, 255, 255));
    BRUSH_BLUE = CreateSolidBrush(RGB(0, 0, 255));
    PEN_WHITE = CreatePen(PS_SOLID, 2, RGB(255, 255, 255));
    PEN_BLUE = CreatePen(PS_SOLID, 2, RGB(0, 0, 255));
}

MyGame::~MyGame()
{
    DeleteObject(BRUSH_WHITE);
    DeleteObject(BRUSH_BLUE);
    DeleteObject(PEN_WHITE);
    DeleteObject(PEN_BLUE);
}

bool MyGame::Initialize()
{
    m_pGameTimer = new GameTimer();
    m_pGameTimer->Reset();

    const wchar_t* className = L"MyFirstWndGame";
    const wchar_t* windowName = L"MyFirstWndGame";

    if (false == __super::Create(className, windowName, 1024, 720))
    {
        return false;
    }

    RECT rcClient = {};
    GetClientRect(m_hWnd, &rcClient);
    m_width = rcClient.right - rcClient.left;
    m_height = rcClient.bottom - rcClient.top;

    m_hFrontDC = GetDC(m_hWnd);
    m_hBackDC = CreateCompatibleDC(m_hFrontDC);
    m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);

    m_hDefaultBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);

    m_GameObjectPtrTable = new GameObjectBase * [MAX_GAME_OBJECT_COUNT];

    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        m_GameObjectPtrTable[i] = nullptr;
    }

    CreatePlayer();
    srand(static_cast<unsigned int>(time(NULL)));

    for (int i = 0; i < 10; i++)
    {
        CreateEnemy();
    }

    return true;
}

void MyGame::Run()
{
    MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_LBUTTONDOWN)
            {
                MyGame::OnLButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else if (msg.message == WM_RBUTTONDOWN)
            {
                MyGame::OnRButtonDown(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else if (msg.message == WM_MOUSEMOVE)
            {
                MyGame::OnMouseMove(LOWORD(msg.lParam), HIWORD(msg.lParam));
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            Update();
            Render();
        }
    }
}

void MyGame::Finalize()
{
    SafeDelete(m_pGameTimer);
    __super::Destroy();
}

void MyGame::Update()
{
    m_pGameTimer->Tick();
    LogicUpdate();

    m_fDeltaTime = m_pGameTimer->DeltaTimeMS();
    m_fFrameCount += m_fDeltaTime;

    while (m_fFrameCount >= m_fixedTimeScale)
    {
        FixedUpdate();
        m_fFrameCount -= m_fixedTimeScale;
    }
}


void MyGame::Render()
{
    ::PatBlt(m_hBackDC, 0, 0, m_width, m_height, WHITENESS);
    
    RECT rc = { 0,0,m_width ,m_height }; 
    if (m_bRewinding)   FillRect(m_hBackDC, &rc, BRUSH_BLUE);
    else                FillRect(m_hBackDC, &rc, BRUSH_WHITE);

    DrawUI();

    for (int i = 0; i < MAX_GAME_OBJECT_COUNT; ++i)
    {
        if (!m_GameObjectPtrTable[i]) break;
        m_GameObjectPtrTable[i]->Render(m_hBackDC);
    }

    ::BitBlt(m_hFrontDC, 0, 0, m_width, m_height, m_hBackDC, 0, 0, SRCCOPY);
}

void MyGame::OnResize(int width, int height)
{
    learning::SetScreenSize(width, height);

    __super::OnResize(width, height);

    m_hBackBitmap = CreateCompatibleBitmap(m_hFrontDC, m_width, m_height);

    HANDLE hPrevBitmap = (HBITMAP)SelectObject(m_hBackDC, m_hBackBitmap);

    DeleteObject(hPrevBitmap);
}

void MyGame::OnClose()
{
    SelectObject(m_hBackDC, m_hDefaultBitmap);

    DeleteObject(m_hBackBitmap);
    DeleteDC(m_hBackDC);

    ReleaseDC(m_hWnd, m_hFrontDC);
}

void MyGame::DrawUI()
{
    RECT rc = {0,50,m_width,70};
    wchar_t msg[1024] = L"";
    swprintf(msg, 1024, L"버틴 시간 : %d", (int)(m_totalElapsedTime/1000.0f));
    SetBkMode(m_hBackDC, TRANSPARENT);
    SetTextColor(m_hBackDC, RGB(0, 0, 255));
    DrawText(m_hBackDC, msg, -1, &rc, DT_CENTER | DT_TOP | DT_SINGLELINE);
        
    if (m_bRewinding)
    {
        SelectObject(m_hBackDC, BRUSH_WHITE);
        SelectObject(m_hBackDC, PEN_BLUE);
    }
    else
    {
        SelectObject(m_hBackDC, BRUSH_BLUE);
        SelectObject(m_hBackDC, PEN_WHITE);
    }

    float ratio = m_remainRewindTime / m_TotalRewindTime;
    ratio = std::max(0.0f, std::min(1.0f, ratio));

    int barLeft = 30;
    int barRight = m_width - 30;

    int fillRight = barLeft + static_cast<int>((barRight - barLeft) * ratio);

    Rectangle(m_hBackDC, barLeft, 10, fillRight, 20);
}


void MyGame::OnMouseMove(int x, int y)
{
    m_MousePos = { x, y };
}

void MyGame::OnLButtonDown(int x, int y)
{
}

void MyGame::OnRButtonDown(int x, int y)
{
}

void MyGame::HandleKeyboardInput()
{
    using Vector2f = learning::Vector2f;
    
    Vector2f dir = Vector2f(0, 0);
    if (GetAsyncKeyState('W') & 0x8000)  dir += Vector2f(0, -1);
    if (GetAsyncKeyState('A') & 0x8000) dir += Vector2f(-1, 0);
    if (GetAsyncKeyState('S') & 0x8000) dir += Vector2f(0, 1);
    if (GetAsyncKeyState('D') & 0x8000) dir += Vector2f(1, 0);
    m_GameObjectPtrTable[0]->SetDirection(dir.Normalized());
}

void MyGame::HandleMouseInput()
{
    if (GetAsyncKeyState(VK_RBUTTON) & 0x8000) m_bRewinding = true;
    else if (GetAsyncKeyState(VK_LBUTTON) & 0x8000) m_bRewinding = true;
    else m_bRewinding = false;
}


void MyGame::FixedUpdate()
{
    using learning::Collider;
    
    int goCount = GetGoCount();

    std::cout << goCount << std::endl;

    if (m_bRewinding)
    {
        if (m_remainRewindTime > 0)
        {
            for (int i = 0; i < goCount; i++)
            {
                m_GameObjectPtrTable[i]->RestoreTrace();
                m_GameObjectPtrTable[i]->RemoveRecentTrace();
            }
            m_remainRewindTime -= m_fixedTimeScale;
            if (!m_bGameStop) m_totalElapsedTime -= m_fixedTimeScale;
        }
    }
    else {
        if(m_remainRewindTime < m_TotalRewindTime && !m_bGameStop) m_remainRewindTime += m_gameTimeScale * m_rewindRecoverSpeed;
        if(!m_bGameStop) m_totalElapsedTime += m_gameTimeScale;
    }
     
    if (IsTimeSlow())  m_gameTimeScale = m_slowFixedTimeScale;
    else               m_gameTimeScale = m_fixedTimeScale;
       

    UpdateCollision();
    if (!m_bRewinding) UpdateEnemyInfo();

    //if (!m_bRewinding && !m_bGameStop)
    //{

    //}
    for (int i = 0; i < goCount; i++)
    {
        if (m_GameObjectPtrTable[i] == nullptr) break;
        m_GameObjectPtrTable[i]->Update(m_gameTimeScale);
    }

    DestroyRegistedObjects();
}

void MyGame::LogicUpdate()
{
    MyGame::HandleKeyboardInput();
    MyGame::HandleMouseInput();
}

void MyGame::CreatePlayer()
{
    assert(m_GameObjectPtrTable[0] == nullptr && "Player object already exists!");

    GameObject* pNewObject = new GameObjectPlayer(this);
    //GameObject* pNewObject = new GameObject(ObjectType::PLAYER);

    //pNewObject->SetName("Player"); ★
    pNewObject->SetPosition(m_width / 2, m_height/2);
    pNewObject->SetSpeed(0.4f); 

    pNewObject->SetColliderCircle(10.0f);
    //pNewObject->SetBitmapInfo(m_pPlayerBitmapInfo); ★ 

    pNewObject->SetWidth(100);
    pNewObject->SetHeight(100);
    m_GameObjectPtrTable[0] = pNewObject;

}

void MyGame::CreateEnemy()
{
    int index = GetGoCount();
    if (index == -1) return;
    GameObject* pNewObject = new EnemyGameObject(this);

    float x = ((float)rand()/RAND_MAX) * m_width;
    float y = ((float)rand() / RAND_MAX)*500 -700;

    pNewObject->SetPosition(x, y);
    pNewObject->SetSpeed(0.06f); 

    //pNewObject->SetBitmapInfo(m_pEnemyBitmapInfo);
    pNewObject->SetWidth(100);
    pNewObject->SetHeight(100);

    pNewObject->SetColliderCircle(20.0f); 

    
    m_GameObjectPtrTable[index] = pNewObject;
}

void MyGame::CreateBullet(GameObjectBase* shooter,Vector2f targetPos)
{
    using learning::ColliderCircle;

    int index = GetGoCount();
    if (index == -1) return;

    BulletGameObject* pNewBullet = new BulletGameObject(this, shooter);

    if (shooter->GetType() == ObjectType::ENEMY)
        pNewBullet->SetColliderColor(RGB(255, 0, 0));
    else if (shooter->GetType() == ObjectType::PLAYER)
        pNewBullet->SetBulletColor(RGB(0, 0, 255));

    

    Vector2f dirN = (targetPos - shooter->GetPosition()).Normalized();
    float bulletRadius = 5.0f;

    ColliderCircle* pShooterCol = dynamic_cast<ColliderCircle*>(shooter->GetCollider());
    if (!pShooterCol)
    {
        std::cout << "shooter의 Collider를 CircleCollider로 변환할 수 없습니다.";
        return;
    }
   
    float dist = pShooterCol->m_radius + bulletRadius;
    Vector2f startPos = shooter->GetPosition() + dirN * dist;

    pNewBullet->SetDirection(dirN);
    pNewBullet->SetSpeed(1.1f);
    pNewBullet->SetColliderCircle(bulletRadius);
    pNewBullet->SetPosition(startPos.x, startPos.y);
    
    m_GameObjectPtrTable[index] = pNewBullet;
}

void MyGame::RegisterToDestroy(GameObjectBase* goBase)
{
    m_destroyTable.push_back(goBase);
}

void MyGame::DestroyObject(GameObjectBase* goBase)
{
    int indexToRemove = -1;
    int goCount = GetGoCount();

    for (int i = 0; i < goCount; i++)
    {
        if (m_GameObjectPtrTable[i] == goBase) {
            indexToRemove = i;
            break;
        }
    }
    if (indexToRemove == -1) return;


    delete m_GameObjectPtrTable[indexToRemove];
    m_GameObjectPtrTable[indexToRemove] = nullptr;
    std::swap(m_GameObjectPtrTable[indexToRemove], m_GameObjectPtrTable[goCount - 1]);


}

void MyGame::DestroyRegistedObjects()
{
    while (!m_destroyTable.empty())
    {
        int index = m_destroyTable.size() - 1;
        if (m_destroyTable[index] != nullptr)
        {
            DestroyObject(m_destroyTable[index]);
        }
        m_destroyTable.pop_back();
    }
}

void MyGame::UpdatePlayerInfo()
{

}

void MyGame::UpdateEnemyInfo()
{
    using Vector2f = learning::Vector2f;
    static GameObject* player = GetPlayer();
    for (int i = 1; i < MAX_GAME_OBJECT_COUNT; i++)
    {
        if (m_GameObjectPtrTable[i] == nullptr) break;
        if (m_GameObjectPtrTable[i]->GetType() != ObjectType::ENEMY) continue;

        Vector2f pPos = player->GetPosition();
        Vector2f pEnm = m_GameObjectPtrTable[i]->GetPosition();

        m_GameObjectPtrTable[i]->SetDirection((pPos - pEnm).Normalized());
    }
}

void MyGame::UpdateCollision()
{
    using learning::Collider;
    int goCount = GetGoCount();
    m_bGameStop = false;

    for (int i = 0; i < goCount; i++)
    {
        if (m_GameObjectPtrTable[i] == nullptr) break;
        GameObjectBase* go = m_GameObjectPtrTable[i];
        Collider* col = go->GetCollider();
        col->SetIsCollide(false);
    }

    for (int i = 0; i < goCount; i++)
    {
        if (m_GameObjectPtrTable[i] == nullptr) break;
        GameObjectBase* goFirst = m_GameObjectPtrTable[i];
        Collider* colFirst = goFirst->GetCollider();
        bool bCollide = false;
        for (int j = i + 1; j < goCount; j++)
        {
            if (m_GameObjectPtrTable[j] == nullptr) break;
            GameObjectBase* goSecond = m_GameObjectPtrTable[j];
            Collider* colSecond = goSecond->GetCollider();

            if (colFirst->Intersect(colSecond))
            {
                colFirst->SetIsCollide(true);
                colSecond->SetIsCollide(true);
                goFirst->OnCollision(goSecond);
            }
        }
    }
}

bool MyGame::IsTimeSlow() const
{
    return m_GameObjectPtrTable[0]->GetDirection().Length() < 0.01f;
}

float MyGame::GetGameTimeScale() const
{
    return m_gameTimeScale;
}

float MyGame::GetSlowFixedTimeScale() const
{
    return m_slowFixedTimeScale;
}

float MyGame::GetRewindTime() const
{
    return m_traceWindowTime;
}

void MyGame::SetGameStop(bool bStop)
{
    m_bGameStop = bStop;
}

bool MyGame::IsGameStopped() const
{
    return m_bGameStop;
}

bool MyGame::IsRewinding() const
{
    return m_bRewinding;
}



float MyGame::GetFixedTimeScale() const
{
    return m_fixedTimeScale;
}


int MyGame::GetGoCount() const
{
    int i = 0;
    for (; i < MAX_GAME_OBJECT_COUNT; i++)
    {
        if (m_GameObjectPtrTable[i] == nullptr) break;
    }

    if (i == MAX_GAME_OBJECT_COUNT)
    {
        std::cout << "Game Object 테이블이 꽉 찼습니다" << std::endl;
        return -1;
    }
    return i;
}
