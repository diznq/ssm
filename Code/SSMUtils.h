#pragma once
#include "SSMCrypto.h"

namespace ssm {
	namespace utils {
		static int GetGameVersion() {
			return 6156;
		}
		const char* GetIP(const char* hostName);
		void GetGameFolder(char* gameFolder);
		int FileDecrypt(const char* name, char** out);
		void FileEncrypt(const char* name, const char* out);
	}
}