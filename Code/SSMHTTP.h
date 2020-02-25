#pragma once
#include <string>
#include <tuple>
namespace ssm {
	namespace http {
		typedef std::tuple<bool, std::string> HTTPResponse;
		HTTPResponse Request(const std::string& method, const std::string& hostName, const std::string& url, const std::string& body, int port = 80);
	}
}