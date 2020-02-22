#pragma once
#include "ISSM.h"
#include "BlockingConcurrentQueue.h"

class SSM : public ISSM {
	std::map<std::string, std::vector<CallbackDesc>> callbacks;
	moodycamel::BlockingConcurrentQueue<ITask*> tasks;
	moodycamel::ConcurrentQueue<ITask*> finishedTasks;
	static void Worker(SSM* pSSM);
	int callbackCounter = 0;
public:
	virtual void OnUpdate(OnUpdateParams* params);
	virtual bool OnChatMessage(OnChatMessageParams* params);
	virtual bool OnPlayerRename(OnPlayerRenameParams* params);
	virtual void OnPlayerConnect(OnPlayerConnectParams* params);
	virtual void OnPlayerDisconnect(OnPlayerDisconnectParams* params);
	virtual void OnCheatDetected(OnCheatDetectedParams* params);
	virtual void OnShoot(OnShootParams* params);
	virtual CallbackDesc AddCallback(const char* event, const std::function<bool(IParams*)>& callback, int priority = 0);
	virtual std::vector<CallbackDesc>& GetCallbacks(const char* event);
	virtual void UpdateCallback(const CallbackDesc& desc);
	virtual void RemoveCallback(const CallbackDesc& desc);
	virtual void AddTask(ITask* executable);
};