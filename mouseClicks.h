#include <iostream>
#include <windows.h>

class MouseClick {
	public:
		MouseClick() {
			ZeroMemory(mouseInputs, sizeof(mouseInputs));
			mouseInputs[0].type = INPUT_MOUSE;
			mouseInputs[1].type = INPUT_MOUSE;
			mouseInputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			mouseInputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
			
		}
		void SetLeftClick() {
			mouseInputs[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
			mouseInputs[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
		}
		void SetRightClick() {
			mouseInputs[0].mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
			mouseInputs[1].mi.dwFlags = MOUSEEVENTF_RIGHTUP;
		}
		void Click() {
			SendInput(ARRAYSIZE(mouseInputs), mouseInputs, sizeof(INPUT));
		}
	private:
		INPUT mouseInputs[2] = {};
};