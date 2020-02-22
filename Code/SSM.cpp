#include "SSM.h"
#include <algorithm>

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

void SSM::OnUpdate(OnUpdateParams* params){
	ITask* task = 0;
	while (finishedTasks.try_dequeue(task)) {
		if (task) {
			task->finish();
			delete task;
		}
	}
	for (auto& cb : callbacks[Callback::eCB_OnUpdate]) {
		if (!cb.function(params)) break;
	}
}

bool SSM::OnChatMessage(OnChatMessageParams* params){
	for (auto& cb : callbacks[Callback::eCB_OnChatMessage]) {
		if (!cb.function(params)) return false;
	}
	return true;
}

void SSM::OnPlayerConnect(OnPlayerConnectParams* params){
	for (auto& cb : callbacks[Callback::eCB_OnPlayerConnect]) {
		if (!cb.function(params)) break;
	}
}

void SSM::OnPlayerDisconnect(OnPlayerDisconnectParams* params){
	for (auto& cb : callbacks[Callback::eCB_OnPlayerDisconnect]) {
		if (!cb.function(params)) break;
	}
}

bool SSM::OnPlayerRename(OnPlayerRenameParams* params) {
	for (auto& cb : callbacks[Callback::eCB_OnPlayerRename]) {
		if (!cb.function(params)) return false;
	}
	return true;
}

void SSM::OnCheatDetected(OnCheatDetectedParams* params){
	for (auto& cb : callbacks[Callback::eCB_OnCheatDetected]) {
		if (!cb.function(params)) break;
	}
}

void SSM::OnShoot(OnShootParams* params){
	for (auto& cb : callbacks[Callback::eCB_OnShoot]) {
		if (!cb.function(params)) break;
	}
}

ISSM::CallbackDesc SSM::AddCallback(Callback event, const std::function<bool(IParams*)>& callback, int priority){
	ISSM::CallbackDesc desc;
	desc.handle = callbackCounter++;
	desc.function = callback;
	desc.priority = priority;
	desc.type = event;
	callbacks[event].emplace_back(desc);
	return desc;
}

void SSM::UpdateCallback(const CallbackDesc& desc) {
	std::sort(callbacks[desc.type].rbegin(), callbacks[desc.type].rend());
}

void SSM::RemoveCallback(const CallbackDesc& desc){
	bool found = false;
	for (auto it = callbacks[desc.type].begin(); it != callbacks[desc.type].end(); it++) {
		if (it->handle == desc.handle) {
			callbacks[desc.type].erase(it);
			return;
		}
	}
}

void SSM::AddTask(ITask* task) {
	tasks.enqueue(task);
}