//Matthew Wong A00925594

/*********************************************************************
Comp3770 Assignment#3 (Max 10 marks).

PUT YOUR FULL NAME AND SET AS YOUR WINDOW TITLE! SetWindowText()

When you attempt to click on a radio button the button "runs away" by 20 pixels.
By clicking on the main window using the right mouse button the buttons
will no longer move. Clicking again on the window will allow the buttons to move
again when the mouse is on them.
You will need to get the child window locations using GetWindowRect() but this is
in screen coordinates. You will then need to convert them to positions within the
main window. You will need to use ScreenToClient() for this. However the first
coordinates will need to be adjusted using the parent position (thus you need to
give the ScreenToClient() proc the handle to the parent) but the second coordinate
needs to be adjusted using the client's handle. Once converted you can then use
the function SetWindowPos(). This MUST be performed via windows subclassing!

When you select a menu item, it is checked and the corresponding radio button is
selected. Conversely if you select the a radio button the corresponding menu item
is checked. Background of the window is changed accordingly. DO NOT USE AUTO RADIO
BUTTONS (yes you need to do it the "hard way". Place the buttons neatly as shown in
example.

To get toggling to work - no globals are used but instead a WM_USER msg is sent to the radio
buttons when the user clicks on the main window (thus you need to capture WM_RBUTTONDOWN in 
the main window). Note: SetWindowLong() used on the buttons is unreliable and SHOULD NOT BE
USED! [radio buttons need to keep track of their state and do so using the WndExtra]

Marking Guidelines:

Functionality						Max
radio buttons and menus				1
radio buttons checked/unchecked		1
Menu checked/unchecked				1
Menu updates radio buttons			1
radio bnt updates menu				1
Background colour changes			1
Windows Subclassing	- btns move		2
Toggle works via user msg			2

Total 10


Function calls used:

GetMenu(hwnd);
SetWindowText()
CallWindowProc()
SetWindowLongPtr()//to subclass window
SendMessage()
CreateWindow()
GetParent()
GetWindowRect()
ScreenToClient()
SetWindowPos()
//use this command to change the background colour
SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG), CreateSolidBrush(//colour))
InvalidateRect()

Flags:
BM_SETCHECK
MF_CHECKED
MF_UNCHECKED
WS_VISIBLE 
WS_CHILD 

*********************************************************************/
#include <Windows.h>;
#include "resource.h";
LRESULT CALLBACK HelloWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK childWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

WNDPROC SubProc[5];

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, 
					PSTR szCMLine, int iCmdShow){
	static TCHAR szAppName[] = TEXT ("HelloApplication");
	static HWND	hwnd;
	MSG		msg;
	WNDCLASS wndclass;
	HMENU hMenu = LoadMenu(hInstance, MAKEINTRESOURCE(IDR_MENU1));

	wndclass.style		= CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = HelloWndProc;
	wndclass.cbClsExtra	= 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance	= hInstance;
	wndclass.hIcon		= LoadIcon (NULL, IDI_APPLICATION);
	wndclass.hCursor	= LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) CreateSolidBrush(RGB(255,0,0));
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = szAppName;
	if (!RegisterClass (&wndclass)){
		MessageBox (NULL, TEXT ("This program requires Windows 95/98/NT"),
					szAppName, MB_ICONERROR);
		return 0;
	}

	hwnd = CreateWindow(szAppName,		
						TEXT("Quiz 3"), 
						WS_OVERLAPPEDWINDOW,	
						CW_USEDEFAULT,		
						CW_USEDEFAULT,		
						800,		
						600,		
						NULL,				
						hMenu,				
						hInstance,			
						NULL);			
	ShowWindow(hwnd, iCmdShow);
	UpdateWindow(hwnd);
	

	while (GetMessage(&msg, NULL, 0, 0))
	{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK HelloWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {

	static TCHAR* button[] = { TEXT("Red"), TEXT("Blue"), TEXT("Green"), TEXT("Orange"), TEXT("Aqua") };
	static int colour[] = { RGB(255,0,0),RGB(0,0,255),RGB(0,255,0),RGB(255,165,0),RGB(0,255,255) };
	static HWND hwndButton[5];
	static int cxChar, cyChar, iSelection = ID_FILE_RED;
	HDC hdc;
	HMENU hMenu;
	TRACKMOUSEEVENT tme, tme1;
	RECT rect;
	POINT point, mousePos;

	switch (message) {
	case WM_CREATE:
		hMenu = GetMenu(hwnd);

		CheckMenuItem(hMenu, ID_FILE_RED, MF_CHECKED);//initially checked

		SetWindowText(hwnd, TEXT("Matthew Wong Set 3B"));

		cxChar = LOWORD(GetDialogBaseUnits());
		cyChar = HIWORD(GetDialogBaseUnits());
		
		//create radio buttons
		for (int i = 0; i < 5; i++) {
			hwndButton[i] = CreateWindow(TEXT("button"),
				button[i],
				WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
				cxChar, cyChar * (1 + 2 * i) + 100,
				20 * cxChar, 7 * cyChar / 4,
				hwnd, (HMENU)(i + ID_FILE_RED),
				((LPCREATESTRUCT)lParam)->hInstance, NULL);
			
			//save wndprocs of radiobuttons into array
			SubProc[i] = (WNDPROC)
				SetWindowLong(hwndButton[i], GWL_WNDPROC, (LONG)childWndProc);
		}
		//sets first checkbox to be initially checked
		SendMessage(hwndButton[iSelection - ID_FILE_RED], BM_SETCHECK, BST_CHECKED, 1);
		return 0;

	case WM_COMMAND:
		hMenu = GetMenu(hwnd);

		switch (LOWORD(wParam)) {
		case ID_FILE_RED:
		case ID_FILE_BLUE:
		case ID_FILE_GREEN:
		case ID_FILE_ORANGE:
		case ID_FILE_AQUA:
			CheckMenuItem(hMenu, iSelection, MF_UNCHECKED);//uncheck previous menu item
			SendMessage(hwndButton[iSelection - ID_FILE_RED], BM_SETCHECK, BST_UNCHECKED, 0);//uncheck radio button that was checked

			iSelection = LOWORD(wParam);
			CheckMenuItem(hMenu, iSelection, MF_CHECKED);//check menu item
			SendMessage(hwndButton[LOWORD(wParam) - ID_FILE_RED], BM_SETCHECK, BST_CHECKED, 1);//check radio button that is chosen
			SetClassLong(hwnd, GCL_HBRBACKGROUND, (LONG)CreateSolidBrush(colour[LOWORD(wParam) - ID_FILE_RED]));
			InvalidateRect(hwnd, NULL, TRUE);
			return 0;

		case ID_FILE_QUIT:
			PostQuitMessage(0);
			return 0;
		}
		break;

	case WM_RBUTTONUP:
		for (int i = 0; i < 5; i++) {
			SendMessage(hwndButton[i], WM_USER, wParam, lParam);
		}
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

LRESULT CALLBACK childWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	int id = GetWindowLong(hwnd, GWL_ID);
	static BOOLEAN runAway = TRUE;
	switch (message) 
	{
	case WM_MOUSEMOVE:
		if (runAway) {
			POINT pt;
			RECT rc;
			GetWindowRect(hwnd, &rc);
			pt.x = rc.left;
			pt.y = rc.top;

			ScreenToClient(GetParent(hwnd), (LPPOINT)&pt);

			SetWindowPos(hwnd, HWND_TOP, pt.x + 20, pt.y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_SHOWWINDOW);
		}
		return 0;

	case WM_USER:
		runAway = !runAway;
		return 0;
	}
	return CallWindowProc(SubProc[id - ID_FILE_RED],hwnd, message, wParam, lParam);
}