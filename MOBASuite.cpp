#include "framework.h"
#include "MOBASuite.h"
#define WIN32_LEAN_AND_MEAN
#include <filesystem>
#include <ShlObj_core.h>
#include <tlhelp32.h>
#include <shellapi.h>
#include <VersionHelpers.h>
constexpr auto MAX_LOADSTRING = 100;

HINSTANCE hInst;
WCHAR szTitle[MAX_LOADSTRING] = L"MOBASuite";
WCHAR szWindowClass[MAX_LOADSTRING];

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);

std::wstring apimswin[] = {
	L"api-ms-win-core-console-l1-1-0.dll",
	L"api-ms-win-core-datetime-l1-1-0.dll",
	L"api-ms-win-core-debug-l1-1-0.dll",
	L"api-ms-win-core-errorhandling-l1-1-0.dll",
	L"api-ms-win-core-file-l1-1-0.dll",
	L"api-ms-win-core-file-l1-2-0.dll",
	L"api-ms-win-core-file-l2-1-0.dll",
	L"api-ms-win-core-handle-l1-1-0.dll",
	L"api-ms-win-core-heap-l1-1-0.dll",
	L"api-ms-win-core-interlocked-l1-1-0.dll",
	L"api-ms-win-core-libraryloader-l1-1-0.dll",
	L"api-ms-win-core-localization-l1-2-0.dll",
	L"api-ms-win-core-memory-l1-1-0.dll",
	L"api-ms-win-core-namedpipe-l1-1-0.dll",
	L"api-ms-win-core-processenvironment-l1-1-0.dll",
	L"api-ms-win-core-processthreads-l1-1-0.dll",
	L"api-ms-win-core-processthreads-l1-1-1.dll",
	L"api-ms-win-core-profile-l1-1-0.dll",
	L"api-ms-win-core-rtlsupport-l1-1-0.dll",
	L"api-ms-win-core-string-l1-1-0.dll",
	L"api-ms-win-core-synch-l1-1-0.dll",
	L"api-ms-win-core-synch-l1-2-0.dll",
	L"api-ms-win-core-sysinfo-l1-1-0.dll",
	L"api-ms-win-core-timezone-l1-1-0.dll",
	L"api-ms-win-core-util-l1-1-0.dll",
	L"api-ms-win-crt-conio-l1-1-0.dll",
	L"api-ms-win-crt-convert-l1-1-0.dll",
	L"api-ms-win-crt-environment-l1-1-0.dll",
	L"api-ms-win-crt-filesystem-l1-1-0.dll",
	L"api-ms-win-crt-heap-l1-1-0.dll",
	L"api-ms-win-crt-locale-l1-1-0.dll",
	L"api-ms-win-crt-math-l1-1-0.dll",
	L"api-ms-win-crt-multibyte-l1-1-0.dll",
	L"api-ms-win-crt-private-l1-1-0.dll",
	L"api-ms-win-crt-process-l1-1-0.dll",
	L"api-ms-win-crt-runtime-l1-1-0.dll",
	L"api-ms-win-crt-stdio-l1-1-0.dll",
	L"api-ms-win-crt-string-l1-1-0.dll",
	L"api-ms-win-crt-time-l1-1-0.dll",
	L"api-ms-win-crt-utility-l1-1-0.dll"
};

wchar_t n[83][MAX_PATH + 1];
SHELLEXECUTEINFOW sei;
int combobox;

const wchar_t* box[18] = {
	L"League of Legends", L"SMITE", L"DOTA2", L"Minecraft Java",
	L"Black Desert", L"Paladins", L"World of Tanks", L"World of Warships", L"Lineage 2",
	L"Elder Scrolls Online", L"GameLoop", L"DirectX9 Unblocked", L"VCRedist AIO", L"Creative ALchemy", L"FinalBurn Neo", L"MAME", L"TrackMania Next", L"HBMAME"
};

void UnblockFile(std::wstring file)
{
	DeleteFileW(file.append(L":Zone.Identifier").c_str());
}

std::wstring PathJoin(const int j, const std::wstring& add)
{
	const std::filesystem::path p = n[j];
	return (p / add).c_str();
}

void PathAppend(const int j, const std::wstring& add)
{
	std::filesystem::path p = n[j];
	const std::filesystem::path f = p / add;
	wcsncpy_s(n[j], f.c_str(), _TRUNCATE);
}

void PathCombine(const int j, const int k, const std::wstring& add)
{
	std::filesystem::path p = n[k];
	const std::filesystem::path f = p / add;
	wcsncpy_s(n[j], f.c_str(), _TRUNCATE);
}

void overwritefile(const int i, const int k, const std::wstring& add)
{
	copy_file(n[i], PathJoin(k, add), std::filesystem::copy_options::overwrite_existing);
}

void terminate_process(const std::wstring& process_name)
{
	HANDLE snap = CreateToolhelp32Snapshot(2, 0);
	PROCESSENTRY32 dwProcessId;
	ZeroMemory(&dwProcessId, sizeof(dwProcessId));
	dwProcessId.dwSize = sizeof(dwProcessId);
	if (Process32FirstW(snap, &dwProcessId))
	{
		while (Process32NextW(snap, &dwProcessId))
		{
			if (dwProcessId.szExeFile == process_name)
			{
				snap = OpenProcess(SYNCHRONIZE | PROCESS_TERMINATE, 1,
					dwProcessId.th32ProcessID);
				TerminateProcess(snap, 0);
				break;
			}
		}
	}
	CloseHandle(snap);
}

void download(const std::wstring& url, int j)
{
	URLDownloadToFileW(nullptr, std::wstring(L"https://mobasweet.org/" + url).c_str(), n[j], 0, nullptr);
	UnblockFile(n[j]);
}

void bulk_apimswin(const wchar_t* url)
{
	for (int i = 0; i < 40; i++)
	{
		PathCombine(i + 1, 0, apimswin[i]);
		download(&std::wstring(url + std::wstring(apimswin[i]))[0], i + 1);
	}
}

bool x64()
{
	if (IsWindows10OrGreater())
	{
		USHORT ProcessMachine = 0;
		USHORT NativeMachine = 0;
		if (!IsWow64Process2(GetCurrentProcess(), &ProcessMachine, &NativeMachine)) {
			return FALSE;
		}
	}
	else if (IsWindowsXPOrGreater())
	{
		BOOL f64 = FALSE;
		return IsWow64Process(GetCurrentProcess(), &f64) && f64;
	}
}

void cfg(const std::wstring& key)
{
	*n[0] = '\0';
	*n[82] = '\0';
	PathAppend(82, std::filesystem::current_path());
	PathAppend(82, L"ms.ini");
	GetPrivateProfileString(L"Path", key.c_str(), nullptr, n[0], 261, n[82]);
	if (std::wstring(n[0]).empty())
	{
		BROWSEINFOW i{};
		i.ulFlags = BIF_USENEWUI | BIF_UAHINT;
		if (key == L"al")
		{
			i.lpszTitle = L"<drive>:\\Program Files(x86)\\Warcraft III\\_retail_\\x86_64, <drive>:\\Program Files (x86)\\StarCraft\\x86_64, <drive>:\\Program Files (x86)\\World of Warcraft\\_retail_";
		}
		if (key == L"track")
		{
			i.lpszTitle = L"<drive>:\\Epic Games\\TrackmaniaNext";
		}
		if (key == L"ll")
		{
			i.lpszTitle = L"<drive>:\\Riot Games\\";
		}
		if (key == L"pl" || key == L"dota2" || key == L"sm")
		{
			i.lpszTitle = L"<drive>:\\<Program Files>\\Steam";
		}
		if (key == L"gl")
		{
			i.lpszTitle = L"<drive>:\\<Program Files>\\TxGameAssistant\\AppMarket";
		}
		if (key == L"eso")
		{
			i.lpszTitle = L"<drive>:\\TheElderScrollsOnline\\The Elder Scrolls Online\\game\\client";
		}
		if (key == L"l2")
		{
			i.lpszTitle = L"<drive>:\\4game";
		}
		if (key == L"wt")
		{
			i.lpszTitle = L"<drive>:\\Games\\World_of_Tanks_<region>\\win64";
		}
		if (key == L"ww")
		{
			i.lpszTitle = L"<drive>:\\Games\\World_of_Warships\\bin\\7708495\\win64";
		}
		if (key == L"bd")
		{
			i.lpszTitle = L"<drive>:\\<Program Files>\\Black Desert Online";
		}
		if (key == L"dota2")
		{
			PathAppend(0, L"steamapps\\common\\dota 2 beta\\game\\bin\\win64");
		}
		if (key == L"pl")
		{
			PathAppend(0, L"steamapps\\common\\Paladins\\Binaries");
		}
		if (key == L"sm")
		{
			PathAppend(0, L"steamapps\\common\\SMITE\\Binaries");
		}
		if (key == L"l2")
		{
			PathAppend(0, L"Lineage II");
		}
		const auto dl = SHBrowseForFolderW(&i);
		if (dl == nullptr)
		{
			exit(0);
		}
		SHGetPathFromIDListW(dl, n[0]);
		WritePrivateProfileString(L"Path", key.c_str(), n[0], n[82]);
	}
}

void game(const std::wstring& dir, const std::wstring& key)
{
	for (int i = 0; i < 57; i++)
	{
		*n[i] = '\0';
	}
	cfg(key);
	PathAppend(0, dir);
	PathCombine(41, 0, L"D3DCompiler_42.dll");
	PathCombine(42, 0, L"d3dx9_42.dll");
	PathCombine(43, 0, L"d3dx11_42.dll");
	PathCombine(44, 0, L"msvcp140.dll");
	PathCombine(45, 0, L"ucrtbase.dll");
	PathCombine(46, 0, L"vcruntime140.dll");
	PathCombine(47, 0, L"d3dcompiler_43.dll");
	PathCombine(48, 0, L"d3dcompiler_47.dll");
	PathCombine(49, 0, L"concrt140.dll");
	PathCombine(52, 0, L"vccorlib140.dll");
	PathCombine(53, 0, L"mfc140u.dll");
	PathCombine(54, 0, L"mfcm140u.dll");
	PathCombine(55, 0, L"vcomp140.dll");
	PathCombine(56, 0, L"vccorlib140.dll");
	PathCombine(57, 0, L"d3dcompiler_46.dll");
}

void calchemy(bool restore)
{
	terminate_process(L"StarCraft.exe");
	terminate_process(L"WarCraft III.exe");
	terminate_process(L"Wow.exe");
	for (auto i = 0; i < 3; i++)
	{
		*n[i] = '\0';
	}
	cfg(L"al");
	UnblockFile(PathJoin(0, L"StarCraft.exe"));
	UnblockFile(PathJoin(0, L"WarCraft III.exe"));
	UnblockFile(PathJoin(0, L"Wow.exe"));
	PathCombine(1, 0, L"dsound.ini");
	PathCombine(2, 0, L"dsound.dll");

	if (restore)
	{
		std::filesystem::remove_all(n[1]);
		std::filesystem::remove_all(n[2]);
	}
	else
	{
		download(L"dsound.ini", 1);
		download(L"dsound.dll", 2);
	}

	exit(0);
}

void dota2(bool restore)
{
	terminate_process(L"dota2.exe");
	*n[1] = '\0';
	*n[2] = '\0';
	cfg(L"dota2");
	PathCombine(1, 0, L"D3DCompiler_47.dll");
	PathCombine(2, 0, L"embree3.dll");
	UnblockFile(PathJoin(0, L"dota2.exe"));
	if (restore)
	{
		download(L"r/dota/D3DCompiler_47.dll", 1);
		download(L"r/dota/embree3.dll", 2);
	}
	else
	{
		download(L"6/D3DCompiler_47.dll", 1);
		download(L"6/embree4.dll", 2);
	}
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = L"steam://rungameid/570";
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void l2(bool restore)
{
	for (auto i = 0; i < 8; i++)
	{
		*n[i] = '\0';
	}
	terminate_process(L"L2.exe");
	cfg(L"l2");
	PathAppend(0, L"system");
	UnblockFile(PathJoin(0, L"L2.exe"));
	PathCombine(1, 0, L"d3dcompiler_43.dll");
	PathCombine(2, 0, L"d3dcompiler_47.dll");
	PathCombine(3, 0, L"D3DX9_43.dll");
	PathCombine(4, 0, L"vcomp120.dll");
	PathCombine(5, 0, L"D3DX9_40.dll");
	PathCombine(6, 0, L"vcomp140.dll");
	PathCombine(7, 0, L"vcruntime140.dll");
	
	if (restore)
	{
		download(L"r/l2/d3dcompiler_43.dll", 1);
		download(L"r/l2/d3dcompiler_47.dll", 2);
		download(L"r/l2/D3DX9_40.dll", 5);
		download(L"r/l2/vcomp120.dll", 4);
		download(L"r/l2/vcomp140.dll", 6);
		download(L"r/l2/vcruntime140.dll", 7);
	}
	else
	{
		download(L"D3DCompiler_47.dll", 1);
		download(L"D3DCompiler_47.dll", 2);
		download(L"D3DX9_43.dll", 3);
		download(L"vcomp120.dll", 4);
		download(L"vcomp140.dll", 6);
		download(L"vcruntime140.dll", 7);
	}
	exit(0);
}

void blackdesert(bool restore)
{
	terminate_process(L"BlackDesert32.exe");
	terminate_process(L"BlackDesert64.exe");
	cfg(L"bdo");
	game(L"bin", L"bdo");
	if (restore)
	{
		bulk_apimswin(L"r/bdo/");
		download(L"r/bdo/msvcp140.dll", 44);
		download(L"r/bdo/ucrtbase.dll", 45);
		download(L"r/bdo/vcruntime140.dll", 46);
		download(L"r/bdo/D3DCompiler_43.dll", 47);
		download(L"r/bdo/concrt140.dll", 49);
		download(L"r/bdo/mfc140u.dll", 53);
		download(L"r/bdo/mfcm140u.dll", 54);
		download(L"r/bdo/vcomp140.dll", 55);
		download(L"r/bdo/vccorlib140.dll", 56);
	}
	else
	{
		bulk_apimswin(L"");
		download(L"msvcp140.dll", 44);
		download(L"ucrtbase.dll", 45);
		download(L"vcruntime140.dll", 46);
		download(L"D3DCompiler_47.dll", 47);
		download(L"concrt140.dll", 49);
		download(L"mfc140u.dll", 53);
		download(L"mfcm140u.dll", 54);
		download(L"vcomp140.dll", 55);
		download(L"vccorlib140.dll", 56);
	}
	game(L"bin64", L"bdo");
	if (restore)
	{
		download(L"r/bdo/x64/msvcp140.dll", 44);
		download(L"r/bdo/x64/ucrtbase.dll", 45);
		download(L"r/bdo/x64/vcruntime140.dll", 46);
		download(L"r/bdo/x64/D3DCompiler_43.dll", 47);
		download(L"r/bdo/x64/concrt140.dll", 49);
		download(L"r/bdo/x64/mfc140u.dll", 53);
		download(L"r/bdo/x64/mfcm140u.dll", 54);
		download(L"r/bdo/x64/vcomp140.dll", 55);
		download(L"r/bdo/x64/vccorlib140.dll", 56);
		bulk_apimswin(L"r/bdo/x64/");
	}
	else
	{
		download(L"6/msvcp140.dll", 44);
		download(L"6/ucrtbase.dll", 45);
		download(L"6/vcruntime140.dll", 46);
		download(L"6/D3DCompiler_47.dll", 47);
		download(L"6/concrt140.dll", 49);
		download(L"6/mfc140u.dll", 53);
		download(L"6/mfcm140u.dll", 54);
		download(L"6/vcomp140.dll", 55);
		download(L"6/vccorlib140.dll", 56);
		bulk_apimswin(L"6/");
	}
	if (restore)
	{
		download(L"r/bdo/main/d3dcompiler_43.dll", 47);
		download(L"r/bdo/main/d3dcompiler_46.dll", 57);
	}
	else
	{
		download(L"D3DCompiler_47.dll", 47);
		download(L"D3DCompiler_47.dll", 57);
	}

	game(L"", L"bdo");
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = L"Black Desert Online Launcher.exe";
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void elderscrolls(bool restore)
{
	for (int i = 0; i < 3; i++)
	{
		*n[i] = '\0';
	}
	terminate_process(L"eso64.exe");
	cfg(L"eso");
	PathCombine(1, 0, L"d3dcompiler_47.dll");
	PathCombine(2, 0, L"eso64.exe");
	UnblockFile(n[2]);
	if (restore)
	{
		download(L"r/teso/d3dcompiler_47.dll", 1);
	}
	else
	{
		download(L"6/D3DCompiler_47.dll", 1);
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = n[2];

	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void worldoftanks(bool restore)
{
	for (int i = 0; i < 46; i++)
	{
		*n[i] = '\0';
	}
	cfg(L"wt");
	wchar_t world_of_tanks[261] = L"WorldOfTanks.exe";
	terminate_process(L"WorldOfTanks.exe");
	UnblockFile(PathJoin(0, world_of_tanks));
	PathCombine(41, 0, L"concrt140.dll");
	PathCombine(42, 0, L"msvcp140.dll");
	PathCombine(43, 0, L"tbb.dll");
	PathCombine(44, 0, L"vcruntime140.dll");
	PathCombine(45, 0, L"system");
	PathCombine(46, 45, L"ucrtbase.dll");
	if (restore)
	{
		download(L"r/wg/concrt140.dll", 41);
		download(L"r/wg/msvcp140.dll", 42);
		download(L"r/wg/tbb12.dll", 43);
		download(L"r/wg/vcruntime140.dll", 44);
		download(L"r/wg/ucrtbase.dll", 46);
		PathAppend(0, L"system");
		bulk_apimswin(L"r/wg/");
	}
	else
	{
		download(L"6/concrt140.dll", 41);
		download(L"6/msvcp140.dll", 42);
		download(L"6/tbb12.dll", 43);
		download(L"6/vcruntime140.dll", 44);
		download(L"6/ucrtbase.dll", 46);
		PathAppend(0, L"system");
		bulk_apimswin(L"6/");
	}
	cfg(L"wt");
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = PathJoin(0, world_of_tanks).c_str();
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void worldofwarships(bool restore)
{
	for (int i = 0; i < 46; i++)
	{
		*n[i] = '\0';
	}
	cfg(L"ww");
	wchar_t world_of_warships[261] = L"WorldOfWarships.exe";
	terminate_process(L"WorldOfWarships.exe");
	UnblockFile(PathJoin(0, world_of_warships));
	PathCombine(41, 0, L"msvcp140.dll");
	PathCombine(42, 0, L"ucrtbase.dll");
	PathCombine(43, 0, L"vcruntime140.dll");
	PathCombine(44, 0, L"cef");
	PathCombine(45, 44, L"d3dcompiler_43.dll");
	PathCombine(46, 44, L"d3dcompiler_47.dll");
	if (restore)
	{
		bulk_apimswin(L"r/wg/");
		download(L"r/wg/msvcp140.dll", 41);
		download(L"r/wg/ucrtbase.dll", 42);
		download(L"r/wg/vcruntime140.dll", 43);
		PathAppend(0, L"cef");
		download(L"r/wg/d3dcompiler_43.dll", 45);
		download(L"r/wg/d3dcompiler_47.dll", 46);
	}
	else
	{
		bulk_apimswin(L"6/");
		download(L"msvcp140.dll", 41);
		download(L"ucrtbase.dll", 42);
		download(L"vcruntime140.dll", 43);
		PathAppend(0, L"cef");
		download(L"D3DCompiler_47.dll", 45);
		download(L"D3DCompiler_47.dll", 46);
	}
	cfg(L"ww");
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = PathJoin(0, world_of_warships).c_str();
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void gameloop(bool restore)
{
	terminate_process(L"AppMarket.exe");
	terminate_process(L"QQExternal.exe");
	for (auto i = 0; i < 44; i++)
	{
		*n[i] = '\0';
	}
	cfg(L"gl");
	UnblockFile(PathJoin(0, L"AppMarket.exe"));
	UnblockFile(PathJoin(0, L"QQExternal.exe"));
	PathCombine(40, 0, L"msvcp140.dll");
	PathCombine(41, 0, L"ucrtbase.dll");
	PathCombine(42, 0, L"vcomp140.dll");
	PathCombine(43, 0, L"vcruntime140.dll");
	if (restore)
	{
		bulk_apimswin(L"r/gameloop");
		download(L"r/gameloop/msvcp140.dll", 40);
		download(L"r/gameloop/ucrtbase.dll", 41);
		download(L"r/gameloop/vcomp140.dll", 42);
		download(L"r/gameloop/vcruntime140.dll", 43);
	}
	else
	{
		bulk_apimswin(L"");
		download(L"msvcp140.dll", 40);
		download(L"ucrtbase.dll", 41);
		download(L"vcomp140.dll", 42);
		download(L"vcruntime140.dll", 43);
	}
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = PathJoin(0, L"AppMarket.exe").c_str();
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void leagueoflegends(bool restore)
{
	cfg(L"ll");
	terminate_process(L"League of Legends.exe");
	terminate_process(L"RiotClientServices.exe");
	terminate_process(L"RiotClientUx.exe");
	terminate_process(L"RiotClientUxRender.exe");
	terminate_process(L"LeagueClient.exe");
	terminate_process(L"LeagueClientUX.exe");
	terminate_process(L"LeagueClientUXRender.exe");
	PathCombine(54, 0, L"Riot Client");
	PathAppend(0, L"League of Legends");
	PathCombine(42, 0, L"Game");
	UnblockFile(PathJoin(42, L"League of Legends.exe"));
	UnblockFile(PathJoin(0, L"LeagueClient.exe"));
	UnblockFile(PathJoin(54, L"RiotClientServices.exe"));
	PathAppend(54, L"UX");
	UnblockFile(PathJoin(54, L"LeagueClientUx.exe"));
	UnblockFile(PathJoin(54, L"LeagueClientUxRender.exe"));
	PathCombine(49, 42, L"D3DCompiler_43.dll");
	PathCombine(44, 0, L"concrt140.dll");
	PathCombine(45, 0, L"vcruntime140.dll");
	PathCombine(46, 0, L"ucrtbase.dll");
	PathCombine(47, 0, L"msvcp140.dll");
	PathCombine(48, 42, L"tbb.dll");
	PathCombine(50, 42, L"D3DCompiler_47.dll");
	PathCombine(51, 0, L"D3DCompiler_47.dll");
	PathCombine(52, 0, L"msvcp140_1.dll");
	PathCombine(53, 0, L"msvcp140_2.dll");
	PathCombine(54, 0, L"vcruntime140_1.dll");

	if (restore)
	{
		bulk_apimswin(L"r/lol/");
		download(L"r/lol/D3DCompiler_43.dll", 42);
		download(L"r/lol/concrt140.dll", 44);
		download(L"r/lol/vcruntime140.dll", 45);
		download(L"r/lol/vcruntime140_1.dll", 54);
		download(L"r/lol/ucrtbase.dll", 46);
		download(L"r/lol/msvcp140.dll", 47);
		download(L"r/lol/D3DCompiler_43.dll", 49);
		download(L"r/lol/d3dcompiler_47.dll", 50);
		download(L"r/lol/d3dcompiler_47.dll", 51);
		download(L"r/lol/msvcp140_1.dll", 52);
		download(L"r/lol/msvcp140_2.dll", 53);
		std::filesystem::remove_all(n[48]);
	}
	else
	{

		download(L"concrt140.dll", 44);
		download(L"vcruntime140.dll", 45);
		download(L"vcruntime140_1.dll", 54);
		download(L"ucrtbase.dll", 46);
		download(L"msvcp140.dll", 47);
		download(L"D3DCompiler_47.dll", 50);
		download(L"D3DCompiler_47.dll", 51);
		download(L"msvcp140_1.dll", 52);
		download(L"msvcp140_2.dll", 53);
		if (x64())
		{
			bulk_apimswin(L"6/");
			download(L"6/tbb12.dll", 48);
			download(L"6/D3DCompiler_47.dll", 49);
		}
		else
		{
			bulk_apimswin(L"");
			download(L"tbb12.dll", 48);
			download(L"D3DCompiler_47.dll", 49);
		}
	}

	*n[54] = '\0';
	cfg(L"ll");
	PathCombine(54, 0, L"Riot Client");
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = PathJoin(54, L"RiotClientServices.exe").c_str();
	ShellExecuteExW(&sei);
	exit(0);
}

void smite(bool restore)
{
	terminate_process(L"Smite.exe");
	terminate_process(L"SmiteEAC.exe");
	terminate_process(L"ShippingPC-BattleGame.exe");
	cfg(L"sm");
	PathCombine(6, 0, L"win32");
	PathAppend(0, L"Win64");
	PathCombine(1, 0, L"tbbmalloc.dll");
	PathCombine(2, 0, L"concrt140.dll");
	PathCombine(3, 0, L"vccorlib140.dll");
	PathCombine(4, 0, L"vcruntime140.dll");
	PathCombine(5, 0, L"d3dcompiler_46.dll");
	UnblockFile(PathJoin(0, L"Smite.exe"));
	UnblockFile(PathJoin(0, L"SmiteEAC.exe"));
	UnblockFile(PathJoin(0, L"ShippingPC - BattleGame.exe"));
	if (restore)
	{
		download(L"r/smite/x64/tbbmalloc.dll", 1);
		download(L"r/smite/x64/concrt140.dll", 2);
		download(L"r/smite/x64/vccorlib140.dll", 3);
		download(L"r/smite/x64/vcruntime140.dll", 4);
		download(L"r/smite/x64/d3dcompiler_46.dll", 5);
	}
	else
	{
		download(L"6/tbbmalloc.dll", 1);
		download(L"6/concrt140.dll", 2);
		download(L"6/vccorlib140.dll", 3);
		download(L"6/vcruntime140.dll", 4);
		download(L"6/D3DCompiler_47.dll", 5);
	}
	for (auto i = 7; i < 12; i++)
	{
		*n[i] = '\0';
	}
	PathCombine(7, 6, L"tbbmalloc.dll");
	PathCombine(8, 6, L"concrt140.dll");
	PathCombine(9, 6, L"vccorlib140.dll");
	PathCombine(10, 6, L"vcruntime140.dll");
	PathCombine(11, 6, L"d3dcompiler_46.dll");
	UnblockFile(PathJoin(6, L"Smite.exe"));
	UnblockFile(PathJoin(6, L"SmiteEAC.exe"));
	UnblockFile(PathJoin(6, L"ShippingPC - BattleGame.exe"));
	if (restore)
	{
		download(L"r/smite/tbbmalloc.dll", 7);
		download(L"r/smite/concrt140.dll", 8);
		download(L"r/smite/vccorlib140.dll", 9);
		download(L"r/smite/vcruntime140.dll", 10);
		download(L"r/smite/d3dcompiler_46.dll", 11);
	}
	else
	{
		download(L"tbbmalloc.dll", 7);
		download(L"concrt140.dll", 8);
		download(L"vccorlib140.dll", 9);
		download(L"vcruntime140.dll", 10);
		download(L"D3DCompiler_47.dll", 11);
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = L"steam://rungameid/386360";
	ShellExecuteExW(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);

}

void paladins(bool restore)
{
	terminate_process(L"Paladins.exe");
	terminate_process(L"PaladinsEAC.exe");
	terminate_process(L"ShippingPC - ChaosGame.exe");

	*n[0] = '\0';
	*n[1] = '\0';
	cfg(L"pl");
	PathAppend(0, L"Win64");
	PathCombine(1, 0, L"tbbmalloc.dll");
	UnblockFile(PathJoin(0, L"Paladins.exe"));
	UnblockFile(PathJoin(0, L"PaladinsEAC.exe"));
	UnblockFile(PathJoin(0, L"ShippingPC - ChaosGame.exe"));
	if (restore)
	{
		download(L"r/paladins/x64/tbbmalloc.dll", 1);
	}
	else
	{
		download(L"6/tbbmalloc.dll", 1);
	}
	*n[0] = '\0';
	*n[1] = '\0';
	cfg(L"pl");
	PathAppend(0, L"Win32");
	PathCombine(1, 0, L"tbbmalloc.dll");
	UnblockFile(PathJoin(0, L"Paladins.exe"));
	UnblockFile(PathJoin(0, L"PaladinsEAC.exe"));
	UnblockFile(PathJoin(0, L"ShippingPC - ChaosGame.exe"));
	if (restore)
	{
		download(L"r/paladins/tbbmalloc.dll", 1);
	}
	else
	{
		download(L"tbbmalloc.dll", 1);
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = L"steam://rungameid/444090";
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

void java_minecraft()
{
	*n[82] = '\0';
	PathAppend(82, std::filesystem::current_path());
	if (x64())
	{
		PathAppend(82, L"jdk-17.0.9_windows-x64_bin.exe");
		URLDownloadToFileW(nullptr, L"https://download.oracle.com/java/17/archive/jdk-17.0.9_windows-x64_bin.exe", n[82], 0, nullptr);
		UnblockFile(n[82]);
	}
	else
	{
		PathAppend(82, L"jre-8u391-windows-i586.exe");
		URLDownloadToFileW(nullptr, L"https://javadl.oracle.com/webapps/download/AutoDL?BundleId=249201_b291ca3e0c8548b5a51d5a5f50063037", n[82], 0, nullptr);
		UnblockFile(n[82]);
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = n[82];
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	std::filesystem::remove_all(n[0]);
	if (x64())
	{
		MessageBoxW(nullptr,
			L"Minecraft Launcher > Minecraft: Java Edition > Installations > Latest > Edit > More Options > Java Executable Path > <drive>:\\Program Files\\Java\\jdk-17\\bin\\javaw.exe",
			L"MOBASuite", MB_OK);
	}
	else
	{
		MessageBoxW(nullptr,
			L"Minecraft Launcher > Minecraft: Java Edition > Installations > Latest > Edit > More Options > Java Executable Path > <drive>:\\Program Files\\Java\\jre-8u391\\bin\\javaw.exe",
			L"MOBASuite", MB_OK);
	}

}

void directx9()
{
	*n[82] = '\0';
	PathAppend(82, std::filesystem::current_path());
	PathAppend(82, L"dx9.exe");
	download(L"dx9.exe", 82);
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpParameters = L"/Q";
	sei.lpFile = n[82];
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	std::filesystem::remove_all(n[82]);
}

void winaio(bool restore)
{
	*n[0] = '\0';
	PathAppend(0, std::filesystem::current_path());
	PathAppend(0, L"vcredist-aio.exe");
	download(L"VisualCppRedist_AIO_x86_x64.exe", 0);
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	if (restore)
	{
		sei.lpParameters = L"/aiR";
	}
	else
	{
		sei.lpParameters = L"/y";
	}
	sei.lpFile = n[0];
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	std::filesystem::remove_all(n[0]);
}

void fbneo()
{

	*n[82] = '\0';
	PathAppend(82, std::filesystem::current_path());
	PathAppend(82, L"FBNeo.zip");
	URLDownloadToFileW(nullptr, L"https://github.com/finalburnneo/FBNeo/releases/download/latest/Windows.x32.zip", n[82], 0, nullptr);
	UnblockFile(n[82]);

	if (x64())
	{
		*n[82] = '\0';
		PathAppend(82, std::filesystem::current_path());
		PathAppend(82, L"FBNeo x64.zip");
		URLDownloadToFileW(nullptr, L"https://github.com/finalburnneo/FBNeo/releases/download/latest/Windows.x64.zip", n[82], 0, nullptr);
		UnblockFile(n[82]);
	}
	exit(0);
}

void mame()
{
	*n[82] = '\0';
	PathAppend(82, std::filesystem::current_path());
	PathAppend(82, L"MAME x64 (0.260).exe");
	if (x64())
	{

		URLDownloadToFileW(nullptr, L"https://github.com/mamedev/mame/releases/download/mame0260/mame0260b_64bit.exe", n[82], 0, nullptr);
		UnblockFile(n[82]);
	}
	else
	{
		MessageBoxW(nullptr,
			L"Sorry, MAME is not available for x86",
			L"MOBASuite", MB_OK);
	}
	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = n[82];
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	std::filesystem::remove_all(n[0]);
	exit(0);
}

void hbmame()
{
	*n[82] = '\0';
	PathAppend(82, std::filesystem::current_path());
	PathAppend(82, L"HBMAME.7z");
	URLDownloadToFileW(nullptr, L"https://hbmame.1emulation.com/hbmameui24514.7z", n[82], 0, nullptr);
	UnblockFile(n[82]);
	exit(0);
}

void trackmania(bool restore)
{
	terminate_process(L"Trackmania.exe");
	cfg(L"track");
	UnblockFile(PathJoin(0, L"Trackmania.exe"));
	PathCombine(1, 0, L"d3dcompiler_47.dll");
	if (restore)
	{
		download(L"r/track/d3dcompiler_47.dll", 1);
	}
	else
	{
		download(L"D3DCompiler_47.dll", 1);
	}

	sei = {};
	sei.cbSize = sizeof(SHELLEXECUTEINFOW);
	sei.fMask = 64;
	sei.nShow = 5;
	sei.lpFile = L"com.epicgames.launcher://apps/b04882669b2e495e9f747c8560488c93%3A7c1dfb2a7e444a8b8a7b3166219550f4%3APigeon?action=launch&silent=true";
	ShellExecuteEx(&sei);
	if (sei.hProcess != nullptr)
	{
		WaitForSingleObject(sei.hProcess, INFINITE);
	}
	exit(0);
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_MOBASUITE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MOBASUITE));

	MSG msg;

	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = szWindowClass;

	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;

	HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME, CW_USEDEFAULT,
		CW_USEDEFAULT, 350, 130,
		nullptr, nullptr, hInstance, nullptr);

	CreateWindow(L"BUTTON", L"Patch", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 10, 10, 75,
		75, hWnd, reinterpret_cast<HMENU>(1), hInstance, nullptr);

	CreateWindow(L"BUTTON", L"Restore", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON, 250, 10, 75,
		75, hWnd, reinterpret_cast<HMENU>(2), hInstance, nullptr);

	HWND hWndComboBox = CreateWindow(WC_COMBOBOX, L"",
		CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
		90, 10, 160, 320, hWnd, NULL, hInstance,
		NULL);

	for (auto& i : box)
	{
		SendMessage(hWndComboBox, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(i));
	}

	SendMessage(hWndComboBox, CB_SETCURSEL, 0, 0);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_COMMAND:
	{
		if (HIWORD(wParam) == CBN_SELCHANGE)
		{
			combobox = SendMessage(reinterpret_cast<HWND>(lParam), CB_GETCURSEL, 0, 0);
		}
		if (LOWORD(wParam) == 1)
		{
			switch (combobox)
			{
			case 0:
				leagueoflegends(false);
				break;
			case 1:
				smite(false);
				break;
			case 2:
				dota2(false);
				break;
			case 3:
				java_minecraft();
				break;
			case 4:
				blackdesert(false);
				break;
			case 5:
				paladins(false);
				break;
			case 6:
				worldoftanks(false);
				break;
			case 7:
				worldofwarships(false);
				break;
			case 8:
				l2(false);
				break;
			case 9:
				elderscrolls(false);
				break;
			case 10:
				gameloop(false);
				break;
			case 11:
				directx9();
				break;
			case 12:
				winaio(false);
				break;
			case 13:
				calchemy(false);
				break;
			case 14:
				fbneo();
				break;
			case 15:
				mame();
				break;
			case 16:
				trackmania(false);
				break;
			case 17:
				hbmame();
				break;
			default:;
			}
		}
		else if (LOWORD(wParam) == 2)
		{
			switch (combobox)
			{
			case 0:
				leagueoflegends(true);
				break;
			case 1:
				smite(true);
				break;
			case 2:
				dota2(true);
				break;
			case 3:
				java_minecraft();
				break;
			case 4:
				blackdesert(true);
				break;
			case 5:
				paladins(true);
				break;
			case 6:
				worldoftanks(true);
				break;
			case 7:
				worldofwarships(true);
				break;
			case 8:
				l2(true);
				break;
			case 9:
				elderscrolls(true);
				break;
			case 10:
				gameloop(true);
				break;
			case 11:
				directx9();
				break;
			case 12:
				winaio(true);
				break;
			case 13:
				calchemy(true);
				break;
			case 14:
				fbneo();
				break;
			case 15:
				mame();
				break;
			case 16:
				trackmania(true);
				break;
			case 17:
				hbmame();
				break;
			default:;
			}
		}

		switch (LOWORD(wParam))
		{
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}