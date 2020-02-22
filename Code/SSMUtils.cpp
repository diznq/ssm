#include "StdAfx.h"
#include "SSMUtils.h"
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

#define CRYPT_KEY {10,32,95,44, 210,235,00,73, 77,68,42,03, 01,254,100,200}

namespace ssm {
	namespace utils {
		const char* GetIP(const char* hostname) {
			in_addr iaddr;
			hostent* host;
			host = gethostbyname(hostname);
			if (!host)
				return hostname;

			iaddr.s_addr = *(unsigned long*)host->h_addr;
			return inet_ntoa(iaddr);
		}
		void GetGameFolder(char* cwd) {
			GetModuleFileNameA(0, cwd, MAX_PATH);
			std::vector<int> pos;
			for (int i = 0, j = strlen(cwd); i < j; i++) {
				if (cwd[i] == '\\')
					pos.push_back(i);
			}
			if (pos.size() >= 2)
				cwd[pos[pos.size() - 2]] = 0;
		}
		int FileDecrypt(const char* name, char** out) {
			char cwd[MAX_PATH], path[2 * MAX_PATH];
			GetGameFolder(cwd);
			sprintf(path, "%s\\Mods\\sfwcl\\%s", cwd, name);
			FILE* f = fopen(path, "rb");
			int ret = 0;
			if (f) {

				fseek(f, 0, SEEK_END);
				long len = ftell(f);
				fseek(f, 0, SEEK_SET);
				unsigned char* mem = new unsigned char[len * 2 + 64];
				memset(mem, 0, len * 2 + 64);
				fread(mem, 1, len, f);
				fclose(f);
				if (mem[0] == 0) {
					AES_ctx ctx; memset(&ctx, 0, sizeof(ctx));
					uint8_t key[] = CRYPT_KEY;
					AES_init_ctx_iv(&ctx, key, (uint8_t*)mem + 1);
					AES_CBC_decrypt_buffer(&ctx, mem + 17, len - 17);
					memcpy(mem, mem + 17, len - 17);
					*out = (char*)mem;
				}
				else *out = (char*)mem;
				ret = strlen(*out);
			}
			else *out = 0;

			return ret;
		}
		void FileEncrypt(const char* name, const char* out) {
			char cwd[MAX_PATH], path[2 * MAX_PATH], outpath[2 * MAX_PATH];
			GetGameFolder(cwd);
			sprintf(path, "%s\\Mods\\sfwcl\\%s", cwd, name);
			sprintf(outpath, "%s\\Mods\\sfwcl\\%s", cwd, out);
			//MessageBoxA(0, path, outpath, 0);
			FILE* f = fopen(path, "rb");
			if (f) {
				fseek(f, 0, SEEK_END);
				long len = ftell(f);
				fseek(f, 0, SEEK_SET);
				unsigned char* mem = new unsigned char[len * 2];
				int remaining = 16 - (len) % 16;
				memset(mem, remaining, len * 2);

				fread(mem, 1, len, f);
				mem[len] = 0;
				fclose(f);
				unsigned char iv[16];
				for (int i = 0; i < 16; i++) {
					iv[i] = rand() & 255;
				}
				if (mem[0] != 0) {
					AES_ctx ctx; memset(&ctx, 0, sizeof(ctx));
					uint8_t key[] = CRYPT_KEY;
					AES_init_ctx_iv(&ctx, key, iv);
					AES_CBC_encrypt_buffer(&ctx, mem, len + remaining);
					f = fopen(outpath, "wb");
					fputc(0, f);
					fwrite(iv, 1, 16, f);
					fwrite(mem, 1, len + remaining, f);
					fclose(f);
				}
			}
		}
	}
}