#pragma once

//
//	Generell feilh�ndterer av exceptions o.l. i programmet.
//
void GetError(std::wstring lpszFunction)
{
	int err = GetLastError();
	std::wstring lpDisplayBuf;
	wchar_t* lpMsgBuf;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		err,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPWSTR)&lpMsgBuf,
		0,
		NULL);

	lpDisplayBuf.append(L"(" + std::to_wstring(err) + L") ");
	lpDisplayBuf.append(lpMsgBuf);

	MessageBox(
		NULL,
		(LPCWSTR)lpszFunction.c_str(),
		(LPCWSTR)lpDisplayBuf.c_str(),
		MB_OK | MB_ICONINFORMATION
	);
}