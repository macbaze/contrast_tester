#include <Windows.h>
#include <CommCtrl.h>
#include <tchar.h>
#include <math.h>
#include <time.h>

#define LEFT_SIDE 140				//
#define TOP_SIDE 5					//
#define WND_WIDTH LEFT_SIDE+140		// для вёрстки
#define WND_HEIGHT 180				//
#define HORIZ_BOARDER 24			//
#define VERT_BOARDER 6				//

HDC hdc, memDC;
HBITMAP memBM;
HANDLE hOld1;
PAINTSTRUCT ps;
HFONT hFont;
HWND startWnd, picWnd;
HWND re_ch, gr_ch, bl_ch, re_ed, gr_ed, bl_ed, nt_ed, ampl_ed;

TCHAR re_l[3], gr_l[3], bl_l[3], nt_l[3], ampl_l[3];

byte  r_c, g_c, b_c, ret_c, k_r, k_g, k_b, max_c, min_c, ampl;
int xRes, yRes, line_Res, x, y, n_t;
double sum_c, delta;

LRESULT CALLBACK startProc(HWND, UINT, WPARAM, LPARAM);	
LRESULT CALLBACK pictProc(HWND, UINT, WPARAM, LPARAM);	

byte color_calc(byte c1, double diff)
{
	sum_c = (double)c1 + diff;
	ret_c = (byte)sum_c;
	if ((sum_c - (double)ret_c + 0.1*(rand() % 10)) > 1.0)
		ret_c++;
	return ret_c;
}

void Output_image(HWND window)
{
	hdc = BeginPaint(window, &ps);
	memDC = CreateCompatibleDC(hdc);
    memBM = CreateCompatibleBitmap(hdc, xRes, line_Res);
    hOld1 = SelectObject(memDC, memBM);

	for(y = 0; y < line_Res; y++)
	{
		for(x = 0; x < xRes; x++ )
		{
      		delta = sin((6.28318 * n_t * x) / xRes) * ampl;
			SetPixelV(memDC, x, y, RGB(color_calc(r_c, delta) * k_r, color_calc(g_c, delta) * k_g, color_calc(b_c, delta) * k_b));
		}
	}
	for (y = 0; y <= yRes; y += line_Res)
	{
		BitBlt(hdc, 0, y, xRes, line_Res, memDC, 0, 0, SRCCOPY);
	}

	SelectObject(memDC, hOld1);
	DeleteObject(memBM);
	DeleteDC(memDC);
	EndPaint(window, &ps);
}

int WINAPI _tWinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPTSTR lpCmdLine,int nCmdShow)
{
	WNDCLASS start_wnd;
	start_wnd.style          = CS_HREDRAW | CS_PARENTDC;
	start_wnd.lpfnWndProc    = startProc;
	start_wnd.cbClsExtra     = 0;
	start_wnd.cbWndExtra     = 0;
	start_wnd.hInstance      = hInstance;
	start_wnd.hIcon          = NULL;
	start_wnd.hCursor        = LoadCursor(NULL, IDC_ARROW);
	start_wnd.hbrBackground  = (HBRUSH)(COLOR_WINDOW);
	start_wnd.lpszMenuName   = NULL;
	start_wnd.lpszClassName  = TEXT("startWndcl");
	
	WNDCLASS pic_wnd;
	pic_wnd.style          = CS_PARENTDC;
	pic_wnd.lpfnWndProc    = pictProc;
	pic_wnd.cbClsExtra     = 0;
	pic_wnd.cbWndExtra     = 0;
	pic_wnd.hInstance      = hInstance;
	pic_wnd.hIcon          = NULL;
	pic_wnd.hCursor        = NULL;
	pic_wnd.hbrBackground  = (HBRUSH)(COLOR_WINDOW);
	pic_wnd.lpszMenuName   = NULL;
	pic_wnd.lpszClassName  = TEXT("pictWndcl");

	if (!RegisterClass(&start_wnd) || !RegisterClass(&pic_wnd))
	{
		MessageBox(NULL,TEXT("Ошибка при регистрации оконного класса."),TEXT("Ошибка!"),NULL);
		return 1;
	}
	hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	startWnd = CreateWindow(TEXT("startWndcl"),TEXT("Параметры теста"),WS_SYSMENU,300,400,WND_WIDTH,WND_HEIGHT,NULL,NULL,hInstance,NULL);
	picWnd = CreateWindow(TEXT("pictWndcl"),NULL,WS_POPUP,1,1,100,100,NULL,NULL,hInstance,NULL);
	if (!startWnd || !picWnd)
	{
		MessageBox(NULL,TEXT("Ошибка при создании окон."), TEXT("Ошибка!"), NULL);
		return 1;
	}
	ShowWindow(startWnd, SW_SHOW);
	xRes = GetDeviceCaps(GetDC(startWnd), HORZRES);
	yRes = GetDeviceCaps(GetDC(startWnd), VERTRES);
	line_Res = 10;
	SetWindowPos(picWnd, HWND_TOPMOST, 0, 0, xRes, yRes, NULL);
	SetFocus(startWnd);
	srand((unsigned int)time(NULL));

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int) msg.wParam;
}

LRESULT CALLBACK startProc(HWND start_proc_wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case 1:
			GetWindowText(re_ed,re_l,4);
			GetWindowText(gr_ed,gr_l,4);
			GetWindowText(bl_ed,bl_l,4);
			GetWindowText(nt_ed,nt_l,4);
			GetWindowText(ampl_ed,ampl_l,4);
			k_r = IsDlgButtonChecked(startWnd,101);
			k_g = IsDlgButtonChecked(startWnd,102);
			k_b = IsDlgButtonChecked(startWnd,103);
			r_c = _ttoi(re_l);
			g_c = _ttoi(gr_l);
			b_c = _ttoi(bl_l);
			n_t = _ttoi(nt_l);
			ampl = _ttoi(ampl_l);
			
			if((k_r)&&(k_g)&&(k_b)){
				max_c = max(r_c,max(g_c,b_c));
				min_c = min(r_c,min(g_c,b_c));}
			if((!k_r)&&(k_g)&&(k_b)){
				max_c = max(g_c,b_c);
				min_c = min(g_c,b_c);}
			if((k_r)&&(!k_g)&&(k_b)){
				max_c = max(r_c,b_c);
				min_c = min(r_c,b_c);}
			if((!k_r)&&(!k_g)&&(k_b)){
				max_c = k_b;
				min_c = k_b;}
			if((k_r)&&(k_g)&&(!k_b)){
				max_c = max(r_c,g_c);
				min_c = min(r_c,g_c);}
			if((!k_r)&&(k_g)&&(!k_b)){
				max_c = g_c;
				min_c = g_c;}
			if((k_r)&&(!k_g)&&(!k_b)){
				max_c = r_c;
				min_c = r_c;}
			if((!k_r)&&(!k_g)&&(!k_b)){
				max_c = 0;
				min_c = 0;}

			if((max_c + ampl <= 255) && (min_c - ampl >= 0))
			{
				ShowWindow(picWnd,SW_SHOW);
				UpdateWindow(picWnd);
				ShowCursor(false);
			}
			else
				MessageBox(NULL,TEXT("Значение амплитуды слишком большое для заданных уровней RGB."),TEXT("Предупреждение"),NULL);
			break;
		}
		break;
	case WM_CREATE:
		re_ch = CreateWindow(TEXT("button"),TEXT("R"), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|BST_CHECKED, LEFT_SIDE, TOP_SIDE, 30, 18, start_proc_wnd,(HMENU)101,NULL,NULL);
		gr_ch = CreateWindow(TEXT("button"),TEXT("G"), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|BST_CHECKED, LEFT_SIDE+45, TOP_SIDE, 30, 18, start_proc_wnd,(HMENU)102,NULL,NULL);
		bl_ch = CreateWindow(TEXT("button"),TEXT("B"), WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX|BST_CHECKED, LEFT_SIDE+90, TOP_SIDE, 30, 18, start_proc_wnd,(HMENU)103,NULL,NULL);

		re_ed = CreateWindow(TEXT("edit"),NULL, WS_CHILD|WS_VISIBLE|ES_CENTER|ES_NUMBER|WS_BORDER, LEFT_SIDE, TOP_SIDE+20, 40, 20, start_proc_wnd,NULL,NULL,NULL);
		CreateUpDownControl(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT|UDS_ARROWKEYS|UDS_ALIGNRIGHT, 0, 0, 0, 0, start_proc_wnd,  201, NULL, re_ed, 255, 0, 150);
		gr_ed = CreateWindow(TEXT("edit"),NULL, WS_CHILD|WS_VISIBLE|ES_CENTER|ES_NUMBER|WS_BORDER, LEFT_SIDE+45, TOP_SIDE+20, 40, 20, start_proc_wnd,NULL,NULL,NULL);
		CreateUpDownControl(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT|UDS_ARROWKEYS|UDS_ALIGNRIGHT, 0, 0, 0, 0, start_proc_wnd,  202, NULL, gr_ed, 255, 0, 150);
		bl_ed = CreateWindow(TEXT("edit"),NULL, WS_CHILD|WS_VISIBLE|ES_CENTER|ES_NUMBER|WS_BORDER, LEFT_SIDE+90, TOP_SIDE+20, 40, 20, start_proc_wnd,NULL,NULL,NULL);
		CreateUpDownControl(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT|UDS_ARROWKEYS|UDS_ALIGNRIGHT, 0, 0, 0, 0, start_proc_wnd,  203, NULL, bl_ed, 255, 0, 150);
		nt_ed = CreateWindow(TEXT("edit"),NULL, WS_CHILD|WS_VISIBLE|ES_CENTER|ES_NUMBER|WS_BORDER, LEFT_SIDE, TOP_SIDE+44, 40, 20, start_proc_wnd,NULL,NULL,NULL);
		CreateUpDownControl(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT|UDS_ARROWKEYS|UDS_ALIGNRIGHT, 0, 0, 0, 0, start_proc_wnd,  204, NULL, nt_ed, 1000, 1, 10);
		ampl_ed = CreateWindow(TEXT("edit"),NULL, WS_CHILD|WS_VISIBLE|ES_CENTER|ES_NUMBER|WS_BORDER,  LEFT_SIDE, TOP_SIDE+68, 40, 20, start_proc_wnd,NULL,NULL,NULL);
		CreateUpDownControl(WS_CHILD|WS_VISIBLE|UDS_SETBUDDYINT|UDS_ARROWKEYS|UDS_ALIGNRIGHT, 0, 0, 0, 0, start_proc_wnd,  205, NULL, ampl_ed, 127, 0, 15);

		SendMessage(CreateWindow(TEXT("button"), TEXT("Показать изображение"), WS_CHILD|WS_VISIBLE, 5, WND_HEIGHT-HORIZ_BOARDER-50,  WND_WIDTH-VERT_BOARDER-10, 45, start_proc_wnd, (HMENU)1,NULL, NULL), WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(CreateWindow(TEXT("static"),TEXT("Каналы цвета"),	WS_CHILD|WS_VISIBLE|SS_RIGHT, 5, TOP_SIDE+1, LEFT_SIDE-10, 20, start_proc_wnd,NULL,NULL,NULL), WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(CreateWindow(TEXT("static"),TEXT("Интенсивность каналов"), WS_CHILD|WS_VISIBLE|SS_RIGHT, 5, TOP_SIDE+22, LEFT_SIDE-10, 20, start_proc_wnd,NULL,NULL,NULL), WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(CreateWindow(TEXT("static"),TEXT("Количество циклов"), WS_CHILD|WS_VISIBLE|SS_RIGHT, 5, TOP_SIDE+46, LEFT_SIDE-10, 20, start_proc_wnd,NULL,NULL,NULL), WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(CreateWindow(TEXT("static"),TEXT("Амплитуда"),	WS_CHILD|WS_VISIBLE|SS_RIGHT, 5, TOP_SIDE+70, LEFT_SIDE-10, 20, start_proc_wnd,NULL,NULL,NULL), WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(re_ch, BM_SETCHECK, (WPARAM)BST_CHECKED, TRUE);
		SendMessage(gr_ch, BM_SETCHECK, (WPARAM)BST_CHECKED, TRUE);
		SendMessage(bl_ch, BM_SETCHECK, (WPARAM)BST_CHECKED, TRUE);
		SendMessage(re_ch, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(gr_ch, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(bl_ch, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(re_ed, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(gr_ed, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(bl_ed, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(nt_ed, WM_SETFONT, (WPARAM)hFont, TRUE);
		SendMessage(ampl_ed, WM_SETFONT, (WPARAM)hFont, TRUE);
		break;
	case WM_CLOSE:
		ShowCursor(true);
		exit(0);
		break;
    default:
        return DefWindowProc(start_proc_wnd, message, wParam, lParam);
        break;
    }
     return 0;
 }

LRESULT CALLBACK pictProc(HWND pic_proc_wnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_KEYDOWN:
		switch(LOWORD(wParam))
		{
		case VK_ESCAPE:
			ShowWindow(pic_proc_wnd, SW_HIDE);
			ShowCursor(true);
			break;
		}
		break;
	case WM_PAINT:
		Output_image(pic_proc_wnd);
		break;
	case WM_DESTROY:
		ShowCursor(true);
		exit(0);
		break;
    default:
        return DefWindowProc(pic_proc_wnd, message, wParam, lParam);
        break;
    }
    return 0;
}
