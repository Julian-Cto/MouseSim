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
		void MouseDown() {
			SendInput(1, mouseInputs, sizeof(INPUT));
		}
		void SetMouseClick() {
			mouseClickMode = true;
			mouseDownMode = false;
		}
		void SetMouseDown() {
			mouseClickMode = false;
			mouseDownMode = true;
		}
		bool IsClickModeOn() const{
			return mouseClickMode;
		}
		bool IsDownModeOn() const {
			return mouseDownMode;
		}
	private:
		bool mouseClickMode = true;
		bool mouseDownMode = false;
		DWORD interval = 0;
		INPUT mouseInputs[2] = {};
};