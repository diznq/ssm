#include "StdAfx.h"
#include "SSMLua.h"
#include "SSM.h"
#include <IScriptSystem.h>

namespace ssm {
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
					pSS->PushFuncParam(event->msg);
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


	}

	void SSMLua::OnUnload() {
		for (auto& desc : bindings) {
			SSM::GetInstance()->RemoveCallback(desc);
		}
	}
}