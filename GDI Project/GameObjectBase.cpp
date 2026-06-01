#include "GameObjectBase.h"

void TraceFrame::Release()
{
	delete objectState;
	objectState = nullptr;
}
