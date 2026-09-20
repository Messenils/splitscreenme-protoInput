#pragma once
#include "Hook.h"
#include "InstallHooks.h"

namespace Proto
{
	class GetRawInputBufferHook final : public Hook
	{
	private:
		HookInfo hookInfo{};
		HookInfo hookInfobuf{}; //hookInfoq
		HookInfo hookInfoq{}; //hookInfoq
		HookInfo hookInfoW{}; //hookInfoq
	public:

		const char* GetHookName() const override { return "Get Raw Input Buffer"; }
		const char* GetHookDescription() const override
		{
			return
				"This hook forwards the raw input received by Proto Input to the game. "
				"This is required for some few games that uses raw input, WM_INPUT messages may not be necessary, as this can hook into window events.";
		}
		bool HasGuiStatus() const override { return true; }
		void ShowGuiStatus() override;
		void InstallImpl() override;
		void UninstallImpl() override;
	};
}