#pragma once
#include <atlbase.h>
#include <atlcom.h>
#include <string>
#include <memory>
#import "ComACRServer.tlb" no_namespace named_guids

struct COMInit
{
	bool success;
	COMInit()
		: success(false)
	{
		CoInitializeEx(NULL, COINIT_MULTITHREADED);
	}
	~COMInit()
	{
		if (!this->success)
		{
			CoUninitialize();
		}
	}
};

class ACRConnection
{
public:
	ACRConnection();
	~ACRConnection();
	using ptr = std::unique_ptr<ACRConnection>;
	friend ACRConnection::ptr make_connection(std::string ip);

	long getLong(long param_number);
private:
	CComQIPtr<IChannel> m_channel;
};
