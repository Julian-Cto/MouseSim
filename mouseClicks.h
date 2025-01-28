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
		void SetInterval(DWORD userInterval) {
			userInterval = interval;
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
			Sleep(interval);
		}
		void MouseDown() {
			SendInput(1, mouseInputs, sizeof(INPUT));
			Sleep(1000);
		}
		void MouseOutput() {
			if (mouseClick) {
				Click();
			}
			if (mouseDown) {
				MouseDown();
			}
		}
		void SetMouseClick() {
			mouseClick = true;
			mouseDown = false;
		}
		void SetMouseDown() {
			mouseClick = false;
			mouseDown = true;
		}
	private:
		bool mouseClick = true;
		bool mouseDown = false;
		DWORD interval = 0;
		INPUT mouseInputs[2] = {};
};