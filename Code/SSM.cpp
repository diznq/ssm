#include "StdAfx.h"
#include "SSM.h"
#include <algorithm>
#include <thread>
#include <Windows.h>
#include <IGame.h>
#include <Psapi.h>

#pragma comment(lib, "Psapi")

namespace ssm {

	void SSM::Worker(SSM* pSSM) {
		while (true) {
			ITask* task;
			pSSM->tasks.wait_dequeue(task);
			if (task) {
				task->exec();
				pSSM->finishedTasks.enqueue(task);
			}
		}
	}

	SSM::SSM() {
		Init(gEnv->pGame);
	}

	void SSM::Init(IGame* pGame) {
		game = pGame;
		if (initialized) return;
		initialized = true;
		unsigned int threads = std::thread::hardware_concurrency();
		for (unsigned int i = 0; i < threads; i++) {
			std::thread thr(Worker, this);
			thr.detach();
		}
		HMODULE CryNetwork = GetModuleHandle("CryNetwork.dll");

		char m_crymp_net[] = "m.crymp.net";

		if (CryNetwork != INVALID_HANDLE_VALUE) {
			MODULEINFO modInfo;
			if (GetModuleInformation(GetCurrentProcess(), CryNetwork, &modInfo, sizeof(modInfo))) {
				const char* base = (const char*)modInfo.lpBaseOfDll;
				DWORD size = modInfo.SizeOfImage;

				for (DWORD i = 1; i < size; i++) {
					if (base[i] == 'g' && !strncmp(base + i, "gamespy.com", 11)) {
						const char* stringBase = base + i;
						while (stringBase[-1]) stringBase--;
						gamespyAddresses.push_back((void*)(stringBase));
						DWORD flags;
						if (VirtualProtect((void*)(base + i), 16, PAGE_READWRITE, &flags)) {
							memcpy((void*)(base + i), m_crymp_net, 11);
							VirtualProtect((void*)(base + i), 16, flags, 0);
						}
						i += 11;
					}
				}
			} else {
				gEnv->pLog->LogError("[ssm] Failed to get module information");
			}
		} else {
			gEnv->pLog->LogError("[ssm] Failed to get CryNetwork handle");
		}
	}

	IGameFramework* SSM::GetGameFramework() {
		return game->GetIGameFramework();
	}

	SSM::~SSM() {

	}

	void SSM::OnUpdate(OnUpdateParams* params) {
		ITask* task = 0;
		while (finishedTasks.try_dequeue(task)) {
			if (task) {
				task->finish();
				delete task;
			}
		}
		for (auto& cb : callbacks[eCB_OnUpdate]) {
			if (!cb.function(params)) break;
		}
	}

	bool SSM::OnChatMessage(OnChatMessageParams* params) {
		for (auto& cb : callbacks[eCB_OnChatMessage]) {
			if (!cb.function(params)) return false;
		}
		return true;
	}

	void SSM::OnPlayerConnect(OnPlayerConnectParams* params) {
		for (auto& cb : callbacks[eCB_OnPlayerConnect]) {
			if (!cb.function(params)) break;
		}
	}

	void SSM::OnPlayerDisconnect(OnPlayerDisconnectParams* params) {
		for (auto& cb : callbacks[eCB_OnPlayerDisconnect]) {
			if (!cb.function(params)) break;
		}
	}

	bool SSM::OnPlayerRename(OnPlayerRenameParams* params) {
		for (auto& cb : callbacks[eCB_OnPlayerRename]) {
			if (!cb.function(params)) return false;
		}
		return true;
	}

	void SSM::OnCheatDetected(OnCheatDetectedParams* params) {
		for (auto& cb : callbacks[eCB_OnCheatDetected]) {
			if (!cb.function(params)) break;
		}
	}

	void SSM::OnShoot(OnShootParams* params) {
		for (auto& cb : callbacks[eCB_OnShoot]) {
			if (!cb.function(params)) break;
		}
	}

	bool SSM::ProcessEvent(const char* event, IParams* params) {
		for (auto& cb : callbacks[event]) {
			if (!cb.function(params)) return false;
		}
		return true;
	}

	CallbackDesc SSM::AddCallback(const char* event, const std::function<bool(IParams*)>& callback, int priority) {
		CallbackDesc desc;
		desc.handle = callbackCounter++;
		desc.function = callback;
		desc.priority = priority;
		desc.type = event;
		callbacks[event].emplace_back(desc);
		std::sort(callbacks[event].rbegin(), callbacks[event].rend());
		return desc;
	}

	std::vector<CallbackDesc>& SSM::GetCallbacks(const char* event) {
		return callbacks[event];
	}

	void SSM::UpdateCallback(const CallbackDesc& desc) {
		RemoveCallback(desc);
		callbacks[desc.type].emplace_back(desc);
		std::sort(callbacks[desc.type].rbegin(), callbacks[desc.type].rend());
	}

	void SSM::RemoveCallback(const CallbackDesc& desc) {
		bool found = false;
		for (auto it = callbacks[desc.type].begin(); it != callbacks[desc.type].end(); it++) {
			if (it->handle == desc.handle) {
				callbacks[desc.type].erase(it);
				return;
			}
		}
	}

	void SSM::ExecuteAsync(std::function<void()> fn) {
		AddTask(new Task<int>([&fn]() -> int {
			fn();
			return 0;
		}));
	}

	void SSM::ExecuteOnMainThread(std::function<void()> fn) {
		AddTask(new Task<int>([]() -> int { return 0; }, [&fn](int p) -> void {
			fn();
		}));
	}

	void SSM::AddTask(ITask* task) {
		tasks.enqueue(task);
	}

	bool SSM::LoadPlugin(const char* name, IPlugin* pPlugin) {
		auto it = plugins.find(name);
		if (it != plugins.end()) {
			if (pPlugin) {
				if (!UnloadPlugin(pPlugin)) return false;
			} else {
				if (!UnloadPlugin(name)) return false;
			}
		}
		if (pPlugin) {
			plugins[name] = std::make_pair((HMODULE)INVALID_HANDLE_VALUE, pPlugin);
			pPlugin->OnLoad();
			return true;
		}
		typedef IPlugin* (__cdecl* PFNCREATEPLUGIN)(ISSM*);
		HMODULE hLib = LoadLibrary(name);
		if (hLib != INVALID_HANDLE_VALUE) {
			PFNCREATEPLUGIN pfnCreatePlugin = (PFNCREATEPLUGIN)GetProcAddress(hLib, "CreatePlugin");
			IPlugin* plugin = pfnCreatePlugin(this);
			if (!plugin) {
				FreeLibrary(hLib);
				return false;
			}
			plugins[name] = std::make_pair((void*)hLib, plugin);
			plugin->OnLoad();
			return true;
		}
		return false;
	}

	bool SSM::UnloadPlugin(const char* name) {
		auto it = plugins.find(name);
		if (it == plugins.end()) return false;
		it->second.second->OnUnload();
		HMODULE handle = (HMODULE)it->second.first;
		plugins.erase(it);
		if (handle != INVALID_HANDLE_VALUE) {
			bool res = FreeLibrary(handle);
			return res;
		}
		return true;
	}

	bool SSM::UnloadPlugin(IPlugin* plugin) {
		for (auto& it : plugins) {
			if (it.second.second == plugin) {
				return UnloadPlugin(it.first.c_str());
			}
		}
		return false;
	}

}