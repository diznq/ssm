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
	~SSM();
	void Init(IGame* pGame);
	void OnUpdate(OnUpdateParams* params) override;
	bool OnChatMessage(OnChatMessageParams* params) override;
	bool OnPlayerRename(OnPlayerRenameParams* params) override;
	void OnPlayerConnect(OnPlayerConnectParams* params) override;
	void OnPlayerDisconnect(OnPlayerDisconnectParams* params) override;
	void OnCheatDetected(OnCheatDetectedParams* params) override;
	void OnShoot(OnShootParams* params) override;
	bool ProcessEvent(const char* event, IParams* params) override;
	CallbackDesc AddCallback(const char* event, const std::function<bool(IParams*)>& callback, int priority = 0) override;
	std::vector<CallbackDesc>& GetCallbacks(const char* event) override;
	void UpdateCallback(const CallbackDesc& desc) override;
	void RemoveCallback(const CallbackDesc& desc) override;
	void AddTask(ITask* executable) override;
	void ExecuteOnMainThread(std::function<void()> fn) override;
	void ExecuteAsync(std::function<void()> fn) override;
	bool LoadPlugin(const char* name, IPlugin* plugin=0) override;
	bool UnloadPlugin(const char* name) override;
	bool UnloadPlugin(IPlugin* plugin) override;
};