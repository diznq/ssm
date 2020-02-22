#include "StdAfx.h"
#include "SSMLua.h"
#include "SSM.h"
#include "SSMUtils.h"
#include <Windows.h>

namespace ssm {

	SSMLua::SSMLua(ISystem* pSystem, IScriptSystem* pSS) : m_pSystem(pSystem), m_pSS(pSS) {
		Init(pSS, pSystem);
		SetGlobalName("CPPAPI");
		RegisterMethods();
		bool BTRUE = true;
		pSS->SetGlobalValue("IS100DLLLOADED", BTRUE);
		pSS->SetGlobalValue("IS121THOUGH", BTRUE);
		pSS->SetGlobalValue("GAME_VER", 6156);
	}

	void SSMLua::RegisterMethods() {
#undef SCRIPT_REG_CLASSNAME
#define SCRIPT_REG_CLASSNAME &ssm::SSMLua::
		SetGlobalName("CPPAPI");
		SCRIPT_REG_TEMPLFUNC(FSetCVar, "cvar, value");
		SCRIPT_REG_TEMPLFUNC(Random, "");
		SCRIPT_REG_TEMPLFUNC(GetIP, "host");
		SCRIPT_REG_TEMPLFUNC(GetLocalIP, "");
		SCRIPT_REG_TEMPLFUNC(GetMapName, "");
		SCRIPT_REG_TEMPLFUNC(AsyncConnectWebsite, "host, page, port, http11, timeout, methodGet");
		SCRIPT_REG_TEMPLFUNC(DoAsyncChecks, "");
		SCRIPT_REG_TEMPLFUNC(SHA256, "text");
		SCRIPT_REG_TEMPLFUNC(GetLocaleInformation, "");
		SCRIPT_REG_TEMPLFUNC(SetExplosiveRemovalTime, "ms");
		SCRIPT_REG_TEMPLFUNC(FileEncrypt, "file, out");
		SCRIPT_REG_TEMPLFUNC(LoadScript, "file");
		SCRIPT_REG_TEMPLFUNC(LoadSSMScript, "file");
		SCRIPT_REG_TEMPLFUNC(SendMessageToClient, "rpcId, method, params...");
		SCRIPT_REG_TEMPLFUNC(CheckRPCID, "rpcId, channelId");
		SCRIPT_REG_TEMPLFUNC(CloseRPCID, "rpcId");
		SCRIPT_REG_TEMPLFUNC(GetTime, "future");
		SCRIPT_REG_TEMPLFUNC(ReloadCoreScripts, "");
	}

	void SSMLua::OnLoad() {
		auto pSSM = SSM::GetInstance();
		bindings.emplace_back(pSSM->AddCallback(ssm::eCB_OnChatMessage, [&pSSM](ssm::IParams* params) -> bool {
			ssm::OnChatMessageParams* event = static_cast<ssm::OnChatMessageParams*>(params);
			IGameFramework* pFW = pSSM->GetGameFramework();
			IScriptSystem* pSS = pFW->GetISystem()->GetIScriptSystem();
			if (pSS) {
				if (pSS->BeginCall("g_gameRules", "OnChatMessage")) {
					ScriptAnyValue values[2];
					pSS->PushFuncParam(event->type);
					pSS->PushFuncParam(event->source);
					pSS->PushFuncParam(event->target);
					pSS->PushFuncParam(event->msg.c_str());
					if (pSS->EndCallAnyN(2, values)) {
						if (values[0].GetVarType() == ScriptVarType::svtBool && values[1].GetVarType() == ScriptVarType::svtString) {
							if (!values[0].b) return false;
							event->msg = values[1].str;
						}
					}
				}
			}
			return true;
		}));

		bindings.emplace_back(pSSM->AddCallback(ssm::eCB_OnPlayerRename, [&pSSM](ssm::IParams* params) -> bool {
			ssm::OnPlayerRenameParams* event = static_cast<ssm::OnPlayerRenameParams*>(params);
			IGameFramework* pFW = pSSM->GetGameFramework();
			IScriptSystem* pSS = pFW->GetISystem()->GetIScriptSystem();
			if (pSS) {
				if (pSS->BeginCall("g_gameRules", "OnPlayerRename")) {
					ScriptAnyValue values[2];
					pSS->PushFuncParam(event->player->GetEntityId());
					pSS->PushFuncParam(event->oldName.c_str());
					pSS->PushFuncParam(event->newName.c_str());
					if (pSS->EndCallAnyN(2, values)) {
						if (values[0].GetVarType() == ScriptVarType::svtBool && values[1].GetVarType() == ScriptVarType::svtString) {
							if (!values[0].b) return false;
							event->newName = values[1].str;
						}
					}
				}
			}
			return true;
		}));

		bindings.emplace_back(pSSM->AddCallback(ssm::eCB_OnUpdate, [&pSSM, this](ssm::IParams* params) -> bool {
			LoadScripts();
			return true;
		}));
	}

	void SSMLua::OnUnload() {
		for (auto& desc : bindings) {
			SSM::GetInstance()->RemoveCallback(desc);
		}
	}

	bool SSMLua::LoadScripts() {
		ScriptAnyValue a;
		if (gEnv->pScriptSystem->GetGlobalAny("g_gameRules", a) && a.GetVarType() == ScriptVarType::svtObject) {
			bool v = false;
			if (!a.table->GetValue("IsModified", v)) {
				if (   !LoadSSMScript(0, "Files\\SafeWritingUTF8.lua")
					|| !LoadSSMScript(0, "Files\\SafeWritingUtilities.lua")
					|| !LoadSSMScript(0, "Files\\SafeWritingUpdater.lua")
					|| !LoadSSMScript(0, "Files\\SafeWritingMain.lua")
					|| !LoadSSMScript(0, "Files\\SafeWritingGameRules.lua")) {
					//a.table->Release();
					gEnv->pSystem->Quit();
					return false;
				} else {
					return true;
				}
			}
		}
		return false;
	}

	//------------------------------------------------
	// Lua bindings
	//------------------------------------------------

	int SSMLua::ReloadCoreScripts(IFunctionHandler* pH) {
		return pH->EndFunction(LoadScripts());
	}
	int SSMLua::GetTime(IFunctionHandler* pH, int future) {
		time_t t = time(0) + future;
		tm* info = localtime(&t);
		static char bf[64];
		sprintf(bf, "%04d%02d%02d%02d%02d%02d", info->tm_year + 1900, info->tm_mon + 1, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec);
		return pH->EndFunction(bf);
	}
	int SSMLua::CheckRPCID(IFunctionHandler* pH, const char* rpcId, int channelId) {
		return pH->EndFunction(true);
	}
	int SSMLua::CloseRPCID(IFunctionHandler* pH, const char* rpcId) {
		return pH->EndFunction(true);
	}
	int SSMLua::SendMessageToClient(IFunctionHandler* pH, const char* id, const char* method, SmartScriptTable params) {
		return pH->EndFunction(true);
	}
	int SSMLua::FileEncrypt(IFunctionHandler* pH, const char* file, const char* out) {
		utils::FileEncrypt(file, out);
		return pH->EndFunction();
	}
	int SSMLua::LoadScript(IFunctionHandler* pH, const char* name) {
		char* main = 0;
		int len = utils::FileDecrypt(name, &main);
		if (len) {
			bool ok = true;
			if (m_pSS) {
				if (!m_pSS->ExecuteBuffer(main, len)) {
					ok = false;
				}
			}
			for (int i = 0; i < len; i++) main[i] = 0;
			delete[] main;
			main = 0;
			if(pH) return pH->EndFunction(ok);
			return ok;
		}
		if(pH) return pH->EndFunction(false);
		return false;
	}
	int SSMLua::LoadSSMScript(IFunctionHandler* pH, const char* name) {
		char path[2 * MAX_PATH];
		utils::GetGameFolder(path);
		sprintf(path, "%s\\Mods\\SafeWriting\\%s", path, name);
		bool result = gEnv->pScriptSystem->ExecuteFile(path, true, true);
		if(pH) return pH->EndFunction(result);
		return result;
	}
	int SSMLua::SetExplosiveRemovalTime(IFunctionHandler* pH, float fval) {
		return pH->EndFunction(true);
	}
	int SSMLua::SHA256(IFunctionHandler* pH, const char* text) {
		unsigned char digest[32];
		char hash[80];
		utils::sha256((const unsigned char*)text, strlen(text), digest);
		for (int i = 0; i < 32; i++) {
			sprintf(hash + i * 2, "%02X", digest[i] & 255);
		}
		return pH->EndFunction(hash);
	}
	int SSMLua::GetLocaleInformation(IFunctionHandler* pH) {
		char buffer[32];
#ifndef LOCALE_SNAME
#define LOCALE_SNAME 0x5C
#endif
		GetLocaleInfoA(LOCALE_USER_DEFAULT, LOCALE_SNAME, buffer, sizeof(buffer));
		TIME_ZONE_INFORMATION tzinfo;
		GetTimeZoneInformation(&tzinfo);
		return pH->EndFunction(buffer, tzinfo.Bias);
	}
	int SSMLua::FSetCVar(IFunctionHandler* pH, const char* cvar, const char* val) {
		if (ICVar* cVar = gEnv->pConsole->GetCVar(cvar))
			cVar->ForceSet(val);
		return pH->EndFunction(true);
	}
	int SSMLua::Random(IFunctionHandler* pH) {
		static bool set = false;
		if (!set) {
			srand(time(0) ^ clock());
			set = true;
		}
		return pH->EndFunction(rand());
	}
	int SSMLua::ConnectWebsite(IFunctionHandler* pH, char* host, char* page, int port, bool http11, int timeout, bool methodGet, bool alive) {
		return pH->EndFunction("");
	}
	int SSMLua::GetIP(IFunctionHandler* pH, char* host) {
		if (strlen(host) > 0) {
			return pH->EndFunction(utils::GetIP(host));
		}
		return pH->EndFunction();
	}
	int SSMLua::GetLocalIP(IFunctionHandler* pH) {
		char hostn[255];
		if (gethostname(hostn, sizeof(hostn)) != SOCKET_ERROR) {
			struct hostent* host = gethostbyname(hostn);
			if (host) {
				for (int i = 0; host->h_addr_list[i] != 0; ++i) {
					struct in_addr addr;
					memcpy(&addr, host->h_addr_list[i], sizeof(struct in_addr));
					return pH->EndFunction(inet_ntoa(addr));
				}
			}
		}
		return pH->EndFunction();
	}
	int SSMLua::GetMapName(IFunctionHandler* pH) {
		return pH->EndFunction(gEnv->pGame->GetIGameFramework()->GetLevelName());
	}
	int SSMLua::DoAsyncChecks(IFunctionHandler* pH) {
		return pH->EndFunction(0);
	}
	int SSMLua::AsyncConnectWebsite(IFunctionHandler* pH, char* host, char* page, int port, bool http11, int timeout, bool methodGet, bool alive) {
		return pH->EndFunction(0);
	}
}