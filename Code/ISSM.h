#pragma once
#include <IGameFramework.h>
#include <IActorSystem.h>
#include <IGameRulesSystem.h>
#include <IWeapon.h>
#include <functional>
#include <atomic>

struct ITask;
struct IParams;
struct IPlugin;
struct CallbackDesc;

struct IParams {

};

struct IPlugin {
	virtual ~IPlugin() = default;
	virtual void OnLoad() = 0;
	virtual void OnUnload() = 0;
};

struct ISSM {

	const char* eCB_OnUpdate = "OnUpdate";
	const char* eCB_OnChatMessage = "OnChatMessage";
	const char* eCB_OnPlayerRename = "OnPlayerRename";
	const char* eCB_OnPlayerConnect = "OnPlayerConnect";
	const char* eCB_OnPlayerDisconnect = "OnPlayerDisconnect";
	const char* eCB_OnCheatDetected = "OnCheatDetected";
	const char* eCB_OnShoot = "OnShoot";

	struct OnUpdateParams : public IParams {
		float deltaTime;
	};
	
	struct OnChatMessageParams : public IParams {
		EChatMessageType type;
		EntityId source;
		EntityId target;
		std::string msg;
	};
	
	struct OnPlayerRenameParams : public IParams {
		IActor* player;
		std::string oldName;
		std::string newName;
	};

	struct OnPlayerConnectParams : public IParams {
		IActor* player;
		int channelId;
		bool isReset;
	};
	
	struct OnPlayerDisconnectParams : public IParams {
		IActor* player;
		int channelId;
		EDisconnectionCause cause;
		std::string desc;
		bool keepClient;
	};
	
	struct OnShootParams : public IParams {
		IWeapon* weapon;
		IFireMode* fireMode;
		IActor* shooter;
	};
	
	struct OnCheatDetectedParams : public IParams {
		IActor* player;
		std::string cheat;
	};

	virtual ~ISSM() = default;
	virtual void Init(IGame *pGame) = 0;
	virtual void OnUpdate(OnUpdateParams* params) = 0;
	virtual bool OnChatMessage(OnChatMessageParams* params) = 0;
	virtual bool OnPlayerRename(OnPlayerRenameParams* params) = 0;
	virtual void OnPlayerConnect(OnPlayerConnectParams* params) = 0;
	virtual void OnPlayerDisconnect(OnPlayerDisconnectParams* params) = 0;
	virtual void OnCheatDetected(OnCheatDetectedParams* params) = 0;
	virtual void OnShoot(OnShootParams* params) = 0;
	virtual bool ProcessEvent(const char* event, IParams* params) = 0;
	virtual CallbackDesc AddCallback(const char* event, const std::function<bool(IParams*)>& callback, int priority=0) = 0;
	virtual std::vector<CallbackDesc>& GetCallbacks(const char* event) = 0;
	virtual void UpdateCallback(const CallbackDesc& desc) = 0;
	virtual void RemoveCallback(const CallbackDesc& desc) = 0;
	virtual void AddTask(ITask* executable) = 0;
	virtual void ExecuteOnMainThread(std::function<void()> fn) = 0;
	virtual void ExecuteAsync(std::function<void()> fn) = 0;
	virtual bool LoadPlugin(const char* name, IPlugin* plugin=0);
	virtual bool UnloadPlugin(const char* name);
	virtual bool UnloadPlugin(IPlugin* plugin);
};

struct ITask {
public:
	virtual void exec() = 0;
	virtual void finish() = 0;
	virtual bool isFinished() = 0;
};

template<class T>
class Task : public ITask {
private:
	std::function<T()> fn;
	std::function<void(T)> cb;
	T result;
	std::atomic<bool> finished;
public:
	Task(std::function<T()> asyncTask, std::function<void(T)> onFinished = nullptr) : fn(asyncTask), cb(onFinished) {}
	virtual void exec() {
		finished = false;
		result = fn();
		finished = true;
	}
	virtual bool isFinished() {
		return finished.load();
	}
	virtual void finish() {
		if(cb)
			cb(result);
	}
};

struct CallbackDesc {
	int handle = 0;
	int priority = 0;
	std::string type;
	std::function<bool(IParams*)> function;

	bool operator<(const CallbackDesc& desc) const {
		return priority < desc.priority;
	}

	bool operator==(const CallbackDesc& desc) const {
		return handle == desc.handle;
	}
};