#include "StdAfx.h"
#include "SSMHTTP.h"
#include <WinSock2.h>
#include <wininet.h>
#include <sstream>

#pragma comment(lib, "wininet")
#pragma comment(lib, "wldap32")
#pragma comment(lib, "shell32")
#pragma comment(lib, "ws2_32")

namespace ssm {
	namespace http {
		HTTPResponse Request(const std::string& method, const std::string& hostName, const std::string& page, const std::string& body, int port) {
			char* buffer = new char[16384];
			bool https = (port % 1000) == 443;
				
			const char* headers = "Content-Type: application/x-www-form-urlencoded";
			const char* form = body.c_str();
			LPCSTR accept[] = { "*/*", 0 };

			HINTERNET hSession = InternetOpen("SSMSafeWriting/2.9.0+", INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
			HINTERNET hConnect = InternetConnect(hSession, hostName.c_str(), port, NULL, NULL, INTERNET_SERVICE_HTTP, 0, 1);

			DWORD timeo = 15000;

			InternetSetOption(hConnect, INTERNET_OPTION_RECEIVE_TIMEOUT, &timeo, sizeof timeo);
			InternetSetOption(hConnect, INTERNET_OPTION_SEND_TIMEOUT, &timeo, sizeof timeo);
			InternetSetOption(hConnect, INTERNET_OPTION_CONNECT_TIMEOUT, &timeo, sizeof timeo);

			HINTERNET hRequest = HttpOpenRequest(hConnect, method.c_str(), page.c_str(), NULL, NULL, accept, (https ? INTERNET_FLAG_SECURE : 0) | INTERNET_FLAG_DONT_CACHE, 1);
			BOOL res = HttpSendRequest(hRequest, headers, (DWORD)strlen(headers), (void*)form, (DWORD)strlen(form));
			std::stringstream ss;
			bool error = false;
			if (res) {
				DWORD dwBytes = 0;
				while (InternetReadFile(hRequest, buffer, 16384, &dwBytes))
				{
					if (dwBytes <= 0) break;
					ss << std::string(buffer, buffer + dwBytes);
				}
			} else {
				error = true;
				ss << "\\\\Error: WinInet code: " << GetLastError();
			}

			InternetCloseHandle(hRequest);
			InternetCloseHandle(hConnect);
			InternetCloseHandle(hSession);

			delete[] buffer;
			buffer = 0;

			return std::make_tuple(error, ss.str());
		}
	}
}