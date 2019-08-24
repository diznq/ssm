/*************************************************************************
	Crytek Source File.
	Copyright (C), Crytek Studios, 2001-2004.
	-------------------------------------------------------------------------
	$Id$
	$DateTime$
	Description: Game DLL entry point.

	-------------------------------------------------------------------------
	History:
	- 2:8:2004   10:38 : Created by Márcio Martins

*************************************************************************/
#include "StdAfx.h"
#include "Game.h"
#include "Sfw/sfw.h"

#include <CryLibrary.h>
#include <platform_impl.h>

namespace sfw {
	extern IGame *pGame;
}

extern "C"
{
	GAME_API IGame *CreateGame(IGameFramework* pGameFramework)
	{
		void *pGame = sfw::CreateGame(pGameFramework);
		if (pGame) return (IGame*)pGame;
		ModuleInitISystem(pGameFramework->GetISystem());

		static char pGameBuffer[sizeof(CGame)];
		IGame* pGameObj = new ((void*)pGameBuffer) CGame();
		sfw::pGame = pGameObj;
		return pGameObj;
	}
}
