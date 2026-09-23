#include <windows.h>
#include "GetRawInputBufferHook.h"
#include <cassert>
#include "RawInput.h"
#include <imgui.h>

namespace Proto
{

	size_t packetreadingcounter = 0;
	bool calledonce = false;
	DWORD threadIDhooked = 0;
	bool beencalled = false;
	bool beencalledcopy = false;

	DWORD Hook_GetQueueStatus(UINT flags) //initialization call
	{
		DWORD result = GetQueueStatus(flags);

		if (flags & QS_RAWINPUT) {
			//EnterCriticalSection(&RawInput::criticalSection);
			if (RawInput::InputQ) {
				result |= MAKELONG(QS_RAWINPUT, QS_RAWINPUT);
				packetreadingcounter = RawInput::BufferCounter;
			}
			else {
				result &= ~MAKELONG(0, QS_RAWINPUT);
			}

			if (calledonce == false)
			{
				threadIDhooked = GetCurrentThreadId();
				calledonce = true;
			}
			//	LeaveCriticalSection(&RawInput::criticalSection);
		}

		return result;
	}

	DWORD Hook_MsgWaitForMultipleObjects( //second call
		DWORD        nCount,
		const HANDLE* pHandles,
		BOOL         fWaitAll,
		DWORD        dwMilliseconds,
		DWORD        dwWakeMask
	)
	{
		bool isRawInputAvailable = false;
		if (GetCurrentThreadId() == threadIDhooked) //destructive hook. only allow on input thread
		{
			if (dwWakeMask & QS_RAWINPUT)
			{
				while (!isRawInputAvailable)
				{
					//	EnterCriticalSection(&RawInput::criticalSection);
					if (RawInput::InputQ) 
					{
						isRawInputAvailable = true;
						//	LeaveCriticalSection(&RawInput::criticalSection);
						break;
					}
					//	LeaveCriticalSection(&RawInput::criticalSection);
					Sleep(1);
				}
				return WAIT_OBJECT_0 + nCount;  // Set result to indicate raw input is available
			}
		}
		//else //thread not same. may be imgui thread
		//{
		return MsgWaitForMultipleObjects(nCount, pHandles, fWaitAll, dwMilliseconds, dwWakeMask);
		//}
	}

	UINT Hook_GetRawInputBuffer( //last call
		PRAWINPUT pData,
		PUINT     pcbSize,
		UINT      cbSizeHeader
	) {
		if (pData != NULL && *pcbSize >= sizeof(RAWINPUT)) {
			RAWINPUT* ptr = (RAWINPUT*)pData;
			//	EnterCriticalSection(&RawInput::criticalSection);
			if (!beencalled)
			{
				beencalled = true;
				beencalledcopy = true;
			}
			if (RawInput::BufferCounter == packetreadingcounter)
			{

				RawInput::InputQ = false;

			}
			else //check for skipped packets
			{
				if (packetreadingcounter < 1024)
					packetreadingcounter++;
				else
					packetreadingcounter = 0;
			}
			//TODO: If input lags behind, pass multiple in array to catch up.
			*ptr = RawInput::inputBuffer[packetreadingcounter]; //lag behind
		//	LeaveCriticalSection(&RawInput::criticalSection);
			
			return 1;
		}

		*pcbSize = sizeof(RAWINPUT);
		return 0;  // Buffer too small
	}
	void GetRawInputBufferHook::ShowGuiStatus()
	{	
		if (calledonce) 
			ImGui::TextUnformatted("MsgWaitForMultipleObjects hooked. ");
		else ImGui::TextUnformatted("GetQueueStatus not called yet. ");

		if (beencalledcopy)
			ImGui::TextUnformatted("GetRawInputBuffer hooked and called atleast once. ");
		else ImGui::TextUnformatted("GetRawInputBuffer not called yet. ");
	}
	void GetRawInputBufferHook::InstallImpl()
	{
		hookInfobuf = std::get<1>(InstallNamedHook(L"user32", "GetRawInputBuffer", Hook_GetRawInputBuffer));
		hookInfoq = std::get<1>(InstallNamedHook(L"user32", "GetQueueStatus", Hook_GetQueueStatus));
		hookInfoW = std::get<1>(InstallNamedHook(L"user32", "MsgWaitForMultipleObjects", Hook_MsgWaitForMultipleObjects));

	}

	void GetRawInputBufferHook::UninstallImpl()
	{
		UninstallHook(&hookInfobuf);
		UninstallHook(&hookInfoq);
		UninstallHook(&hookInfoW);
	}

}