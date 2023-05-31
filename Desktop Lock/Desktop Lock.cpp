#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <ctime>
#include <fstream>

using namespace std;

HHOOK keyboardHook;
HHOOK mouseHook;
bool locked = true;
string code = "", helpCode = "YOURCODEHERE", inputCode = "", helpInputCode = ""; // you can change your code anytime
int index = 0, helpIndex = 0;
double minutes = 0;
clock_t start;
double duration;

bool pressed[256];

char getLetter(int asciiChar) {
	if (pressed[160] || pressed[161]) {
		if (asciiChar == 49)
			return '!';
		if (asciiChar == 50)
			return '@';
		if (asciiChar == 51)
			return '#';
		if (asciiChar == 52)
			return '$';
		if (asciiChar == 53)
			return '%';
		if (asciiChar == 54)
			return '^';
		if (asciiChar == 55)
			return '&';
		if (asciiChar == 56)
			return '*';
		if (asciiChar == 57)
			return '(';
		if (asciiChar == 48)
			return ')';
		if (asciiChar == 189)
			return '_';
		if (asciiChar == 187)
			return '+';
		if (asciiChar == 219)
			return '{';
		if (asciiChar == 221)
			return '}';
		if (asciiChar == 222)
			return '\"';
	}
	else {
		if (asciiChar == 219)
			return '[';
		if (asciiChar == 221)
			return ']';
		if (asciiChar == 222)
			return '\'';
	}
	char character = (char)asciiChar;
	if (asciiChar == 160 || asciiChar == 161 || asciiChar == 162 || asciiChar == 91 || asciiChar == 165 || asciiChar == 163)
		return NULL;
	return character;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
	PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;

	double finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	if (duration / 60 > minutes)
		locked = true;

	if (key->vkCode == 164 && nCode == HC_ACTION) {
		start = clock();
		return locked ? -1 : 0;
	}
	if ((wParam == WM_KEYFIRST || wParam == WM_KEYLAST || wParam == WM_KEYDOWN || wParam == WM_KEYUP) && nCode == HC_ACTION) {
		start = clock();

		if (wParam == WM_KEYDOWN)
			pressed[key->vkCode] = true;

		if (wParam == WM_KEYUP)
			pressed[key->vkCode] = false;

		if (key->vkCode == 13) // enter
			if (code == inputCode || helpCode == helpInputCode) {
				locked = false;
				inputCode = "";
				index = 0;
				helpIndex = 0;
				return -1;
			}

		// add letters to the inputCode
		if (wParam == WM_KEYUP && getLetter(key->vkCode) != NULL) {
			char letter = getLetter(key->vkCode);
			if (code.size() == index) {
				index = 0;
				inputCode = "";
			}
			if (code.at(index) == letter) {
				index++;
				inputCode += letter;
			}
			else {
				inputCode = "";
				index = 0;
			}


			if (helpCode.size() == helpIndex) {
				helpIndex = 0;
				helpInputCode = "";
			}
			if (helpCode.at(helpIndex) == letter) {
				helpIndex++;
				helpInputCode += letter;
			}
			else {
				helpInputCode = "";
				helpIndex = 0;
			}
		}

		return locked ? -1 : 0;
	}
	return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
	PKBDLLHOOKSTRUCT key = (PKBDLLHOOKSTRUCT)lParam;
	double finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	if (duration / 60 > minutes)
		locked = true;
	if ((wParam == WM_LBUTTONUP || wParam == WM_LBUTTONDOWN
		|| wParam == WM_RBUTTONUP || wParam == WM_RBUTTONDOWN
		|| wParam == WM_XBUTTONUP || wParam == WM_XBUTTONDOWN
		|| wParam == WM_RBUTTONUP || wParam == WM_RBUTTONDOWN
		|| wParam == WM_NCXBUTTONUP || wParam == WM_NCXBUTTONDOWN
		|| wParam == WM_NCLBUTTONUP || wParam == WM_NCLBUTTONDOWN
		|| wParam == WM_NCMBUTTONUP || wParam == WM_NCMBUTTONDOWN
		|| wParam == WM_NCRBUTTONUP || wParam == WM_NCRBUTTONDOWN

		// double click
		|| wParam == WM_LBUTTONDBLCLK
		|| wParam == WM_XBUTTONDBLCLK
		|| wParam == WM_RBUTTONDBLCLK
		|| wParam == WM_NCXBUTTONDBLCLK
		|| wParam == WM_NCLBUTTONDBLCLK
		|| wParam == WM_NCMBUTTONDBLCLK
		|| wParam == WM_NCRBUTTONDBLCLK



		// mouse move
		|| wParam == WM_MOUSEMOVE
		) && nCode == HC_ACTION) {
		start = clock();
		return locked ? -1 : 0;
	}
	return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}


int main() {
	HWND hWnd = GetConsoleWindow();
	ShowWindow(hWnd, SW_HIDE);
	ifstream file;
	file.open("Data.txt");
	if (file.is_open()) {
		int line = 0;
		while (!file.eof()) {

			if (line == 0)
				file >> minutes;
			else if (line == 1) {
				file >> code;
				for (auto& c : code) c = toupper(c);
			}
			line++;

		}
	}
	file.close();



	start = clock();
	keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, NULL);
	mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseProc, NULL, NULL);

	MSG msg{ 0 };

	while (GetMessage(&msg, NULL, 0, 0) != 0) {
	}
	UnhookWindowsHookEx(keyboardHook);
	UnhookWindowsHookEx(mouseHook);
	return 0;
}