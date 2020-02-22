#pragma once
#include <platform.h>
#include <platform_impl.h>
#include <IGameFramework.h>
#include <IGameRulesSystem.h>
#include <IWeapon.h>
#include <functional>
#include <atomic>

struct ITask;

struct ISSM {

	enum Callback {
		eCB_OnUnload,
		eCB_OnUpdate,
		eCB_OnChatMessage,
		eCB_OnPlayerRename,
		eCB_OnPlayerConnect,
		eCB_OnPlayerDisconnect,
		eCB_OnCheatDetected,
		eCB_OnShoot,
		eCB_Length
	};
	
	struct IParams {

	};

	struct CallbackDesc {
		int handle = 0;
		int priority = 0;
		Callback type;
		std::function<bool(IParams*)> function;

		bool operator<(const CallbackDesc& desc) const {
			return priority < desc.priority;
		}

		bool operator==(const CallbackDesc& desc) const {
			return handle == desc.handle;
		}
	};

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
		EntityId player;
		std::string oldName;
		std::string newName;
	};

	struct OnPlayerConnectParams : public IParams {
		EntityId player;
	};
	
	struct OnPlayerDisconnectParams : public IParams {
		EntityId player;
	};
	
	struct OnShootParams : public IParams {
		IWeapon* weapon;
		IFireMode* fireMode;
		IActor* shooter;
	};
	
	struct OnCheatDetectedParams : public IParams {
		EntityId player;
		std::string cheat;
	};

	virtual void OnUpdate(OnUpdateParams* params) = 0;
	virtual bool OnChatMessage(OnChatMessageParams* params) = 0;
	virtual bool OnPlayerRename(OnPlayerRenameParams* params) = 0;
	virtual void OnPlayerConnect(OnPlayerConnectParams* params) = 0;
	virtual void OnPlayerDisconnect(OnPlayerDisconnectParams* params) = 0;
	virtual void OnCheatDetected(OnCheatDetectedParams* params) = 0;
	virtual void OnShoot(OnShootParams* params) = 0;
	virtual CallbackDesc AddCallback(Callback event, const std::function<bool(IParams*)>& callback, int priority=0) = 0;
	virtual void UpdateCallback(const CallbackDesc& desc) = 0;
	virtual void RemoveCallback(const CallbackDesc& desc) = 0;
	virtual void AddTask(ITask* executable) = 0;
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
		cb(result);
	}
};