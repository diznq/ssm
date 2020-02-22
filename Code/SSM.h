#pragma once
#include "ISSM.h"
#include "BlockingConcurrentQueue.h"
#include <Windows.h>
class SSM : public ISSM {
	std::map<std::string, std::vector<CallbackDesc>> callbacks;
	moodycamel::BlockingConcurrentQueue<ITask*> tasks;
	moodycamel::ConcurrentQueue<ITask*> finishedTasks;
	static void Worker(SSM* pSSM);
	int callbackCounter = 0;
	bool initialized = false;
	IGame* game;
	std::map<std::string, std::pair<HMODULE, IPlugin*>> plugins;
public:
	static SSM* GetInstance() {
		static SSM instance;
		return &instance;
	}
	SSM();
	virtual ~SSM();
	virtual void Init(IGame* pGame);
	virtual void OnUpdate(OnUpdateParams* params);
	virtual bool OnChatMessage(OnChatMessageParams* params);
	virtual bool OnPlayerRename(OnPlayerRenameParams* params);
	virtual void OnPlayerConnect(OnPlayerConnectParams* params);
	virtual void OnPlayerDisconnect(OnPlayerDisconnectParams* params);
	virtual void OnCheatDetected(OnCheatDetectedParams* params);
	virtual void OnShoot(OnShootParams* params);
	virtual bool ProcessEvent(const char* event, IParams* params);
	virtual CallbackDesc AddCallback(const char* event, const std::function<bool(IParams*)>& callback, int priority = 0);
	virtual std::vector<CallbackDesc>& GetCallbacks(const char* event);
	virtual void UpdateCallback(const CallbackDesc& desc);
	virtual void RemoveCallback(const CallbackDesc& desc);
	virtual void AddTask(ITask* executable);
	virtual void ExecuteOnMainThread(std::function<void()> fn);
	virtual void ExecuteAsync(std::function<void()> fn);
	virtual bool LoadPlugin(const char* name, IPlugin* plugin=0);
	virtual bool UnloadPlugin(const char* name);
	virtual bool UnloadPlugin(IPlugin* plugin);
};