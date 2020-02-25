#pragma once
#include <IGameFramework.h>
#include <IActorSystem.h>
#include <IGameRulesSystem.h>
#include <IWeapon.h>
#include <functional>
#include <atomic>

namespace ssm {

	struct ITask;
	struct IParams;
	struct IPlugin;
	struct CallbackDesc;

	constexpr const char* eCB_OnUpdate = "OnUpdate";
	constexpr const char* eCB_OnChatMessage = "OnChatMessage";
	constexpr const char* eCB_OnPlayerRename = "OnPlayerRename";
	constexpr const char* eCB_OnPlayerConnect = "OnPlayerConnect";
	constexpr const char* eCB_OnPlayerDisconnect = "OnPlayerDisconnect";
	constexpr const char* eCB_OnCheatDetected = "OnCheatDetected";
	constexpr const char* eCB_OnShoot = "OnShoot";

	struct IParams {

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
		IActor* player;
		std::string oldName;
		std::string newName;
	};

	struct OnPlayerConnectParams : public IParams {
		IActor* player;
		INetChannel* channel;
		int channelId;
		bool isReset;
	};

	struct OnPlayerDisconnectParams : public IParams {
		IActor* player;
		INetChannel* channel;
		int channelId;
		EDisconnectionCause cause;
		std::string desc;
		bool keepClient;
	};

	struct OnShootParams : public IParams {
		IWeapon* weapon;
		IFireMode* fireMode;
		IActor* shooter;
		Vec3 pos;
		Vec3 dir;
		Vec3 vel;
		EntityId ammoId;
		EntityId weaponId;
	};

	struct OnCheatDetectedParams : public IParams {
		IActor* player;
		std::string cheat;
	};

	struct IPlugin {
		virtual ~IPlugin() = default;
		virtual void OnLoad() = 0;
		virtual void OnUnload() = 0;
	};

	struct ISSM {
		virtual ~ISSM() = default;
		virtual void Init(IGame* pGame) = 0;
		virtual void OnUpdate(OnUpdateParams* params) = 0;
		virtual bool OnChatMessage(OnChatMessageParams* params) = 0;
		virtual bool OnPlayerRename(OnPlayerRenameParams* params) = 0;
		virtual void OnPlayerConnect(OnPlayerConnectParams* params) = 0;
		virtual void OnPlayerDisconnect(OnPlayerDisconnectParams* params) = 0;
		virtual void OnCheatDetected(OnCheatDetectedParams* params) = 0;
		virtual void OnShoot(OnShootParams* params) = 0;
		virtual bool ProcessEvent(const char* event, IParams* params) = 0;
		virtual CallbackDesc AddCallback(const char* event, const std::function<bool(IParams*)>& callback, int priority = 0) = 0;
		virtual std::vector<CallbackDesc>& GetCallbacks(const char* event) = 0;
		virtual void UpdateCallback(const CallbackDesc& desc) = 0;
		virtual void RemoveCallback(const CallbackDesc& desc) = 0;
		virtual void AddTask(ITask* executable) = 0;
		virtual void ExecuteOnMainThread(std::function<void()> fn) = 0;
		virtual void ExecuteAsync(std::function<void()> fn) = 0;
		virtual bool LoadPlugin(const char* name, IPlugin* plugin = 0) = 0;
		virtual bool UnloadPlugin(const char* name) = 0;
		virtual bool UnloadPlugin(IPlugin* plugin) = 0;
		virtual IGameFramework* GetGameFramework() = 0;
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
			if (cb)
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
}