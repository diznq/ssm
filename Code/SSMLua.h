#pragma once
#include "ISSM.h"

namespace ssm {
	class SSMLua : public IPlugin {
		std::vector<CallbackDesc> bindings;
		void OnLoad() override;
		void OnUnload() override;
	};
}