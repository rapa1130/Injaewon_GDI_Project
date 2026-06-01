#include"NzWndBase.h"
#include"MyGame.h"

//int main()
//{
//	MyGame g;
//
//	if (!g.Initialize()) return -1;
//	g.Run();
//	g.Finalize();
//
//	return 0;
//}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	MyGame g;

	if (!g.Initialize()) return -1;
	g.Run();
	g.Finalize();

	return 0;
}