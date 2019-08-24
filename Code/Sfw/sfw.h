#pragma once

struct IGame;
struct IGameFramework;

namespace sfw {
	IGame *CreateGame(IGameFramework* pGameFramework);
	int __fastcall GameUpdate(void* self, void *addr, bool p1, unsigned int p2);
}