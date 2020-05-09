#include "StdAfx.h"

namespace ssm {
	namespace cvars {
		void SvMaxPlayers(ICVar* cvar) {
			int value = cvar->GetIVal();
			if (value < 2) {
				value = 2;
				cvar->Set(value);
				return;
			}
		}
	}
}