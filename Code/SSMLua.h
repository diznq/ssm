#pragma once
#include "ISSM.h"

namespace ssm {
	class SSMLua : public IPlugin, public CScriptableBase {
		std::vector<CallbackDesc> bindings;
	public:
		SSMLua(ISystem* pSystem, IScriptSystem* pSS);
		void OnLoad() override;
		void OnUnload() override;

		bool LoadScripts();

		int ConnectWebsite(IFunctionHandler* pH, char* host, char* page, int port, bool http11 = false, int timeout = 15, bool methodGet = true, bool alive = false);
		int FSetCVar(IFunctionHandler* pH, const char* cvar, const char* val);
		int AsyncConnectWebsite(IFunctionHandler* pH, char* host, char* page, int port, bool http11 = false, int timeout = 15, bool methodGet = true, bool alive = false);
		int GetIP(IFunctionHandler* pH, char* host);
		int GetMapName(IFunctionHandler* pH);
		int Random(IFunctionHandler* pH);
		int GetLocalIP(IFunctionHandler* pH);
		int DoAsyncChecks(IFunctionHandler* pH);
		int SHA256(IFunctionHandler* pH, const char* text);
		int GetLocaleInformation(IFunctionHandler* pH);
		int SetExplosiveRemovalTime(IFunctionHandler* pH, float fval);
		int FileEncrypt(IFunctionHandler* pH, const char* file, const char* out);
		int LoadScript(IFunctionHandler* pH, const char* file);
		int LoadSSMScript(IFunctionHandler* pH, const char* file);

		int SendMessageToClient(IFunctionHandler* pH, const char* id, const char* method, SmartScriptTable params);
		int CheckRPCID(IFunctionHandler* pH, const char* id, int channelId);
		int CloseRPCID(IFunctionHandler* pH, const char* id);
		int GetTime(IFunctionHandler* pH, int future = 0);
		int ReloadCoreScripts(IFunctionHandler* pH);

	protected:
		void RegisterMethods();
		ISystem* m_pSystem;
		IScriptSystem* m_pSS;
	};
}