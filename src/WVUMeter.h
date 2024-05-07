#ifndef WVU_METER_H
#define WVU_METER_H

class WVUMeter
{

public:
	WVUMeter();
	~WVUMeter();
	int Create(HWND hwndParent, HINSTANCE inst, int x, int y, int w, int h,
		HBITMAP hbBg, HBITMAP hbFg, int dash, int space);

	int SetRange(DWORD max);
	int SetPos(DWORD pos);
	int SetPeekFalloff(DWORD step, DWORD time_ms);

private:
	HBITMAP _hbBg;
	HBITMAP _hbFg;

	DWORD _max;
	DWORD _pos;
	DWORD _oldpos;
	// int _resolution;
	DWORD _space;
	DWORD _dash;
	DWORD _peek;
	DWORD _oldpeek;
	HWND Handle;
	WNDPROC _oldproc;
	DWORD _oldtime;
	DWORD _peek_fall_step;
	DWORD _peek_fall_time;

	static LRESULT CALLBACK _WindowSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void Draw(WVUMeter* control, HDC hdc);

protected:
};

#endif
