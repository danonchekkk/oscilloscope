#include <windows.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <vector>
#include <algorithm>

const wchar_t windowClass[] = L"win32app";
const wchar_t windowTitle[] = L"Лабораторная работа 3";
HPEN pen, oldPen;
HBRUSH brush, oldBrush;
RECT r;
bool flag = false;
int N = 0; //число графиков(считывается первой строкой из файла)
int x, y;
int LastX, LastY;
bool flagTRE = true;
std::vector <double> vecX = {0};//координаты точек по X и Y (считываются из файла далее)
std::vector <double> vecY = {0};

std::vector<std::string> PlotName;

void Line(HDC hdc, int x1, int y1, int x2, int y2) //адекватная функция рисования линий
{
    MoveToEx(hdc, x1, y1, NULL); //сделать текущими координаты x1, y1
    LineTo(hdc, x2, y2);
}

void DrowGrafic(HDC hdc, RECT r, HWND hWnd) {


    int width = ((r.right - 10) - (r.left + 10));
    int height = ((r.bottom - 20) - (r.top + 20));

    if (flag)
    {

        double max_x_elem = *max_element(vecX.begin(), vecX.end());
        double min_x_elem = *min_element(vecX.begin(), vecX.end());

        double max_y_elem = *max_element(vecY.begin(), vecY.end());
        double min_y_elem = *min_element(vecY.begin(), vecY.end());



        double kx = width / (max_x_elem - min_x_elem);
        double ky = height / (max_y_elem - min_y_elem);

        double start_x = 10;
        double start_y = 20;

        if (min_y_elem < 0) { start_y = (20 + abs(min_y_elem) * ky); }


        brush = CreateSolidBrush(RGB(255, 255, 255));
        oldBrush = (HBRUSH)SelectObject(hdc, brush);

        pen = CreatePen(PS_USERSTYLE, 2, RGB(255, 0, 0));
        oldPen = (HPEN)SelectObject(hdc, pen);
        for (int j = 0; j < N; j++)
        {
            
            
            for (int i = j; i < vecX.size() - N; i += N)
            {
                double centerX = start_x + vecX[i] * kx;
                double centerY = r.bottom - (start_y + vecY[i] * ky);

               
                if (i + 2 * N < vecX.size())
                {
                    Line(hdc, start_x + vecX[i] * kx, r.bottom - (start_y + vecY[i] * ky), start_x + vecX[i + N] * kx, r.bottom - (start_y + vecY[i + N] * ky));  
                }         

                if (i % 15 == 0)
                {
                   

                    POINT triangle[3];
                    triangle[0].x = centerX + 2;
                    triangle[0].y = centerY + 5;
                    triangle[1].x = centerX;
                    triangle[1].y = centerY;
                    triangle[2].x = centerX - 2;
                    triangle[2].y = centerY + 5;
                    Polygon(hdc, triangle, 3);
                }

               if (i + N >= vecX.size() - N)
                {
                   POINT triangle[3];
                   triangle[0].x = centerX + 2;
                   triangle[0].y = centerY + 5;
                   triangle[1].x = centerX;
                   triangle[1].y = centerY;
                   triangle[2].x = centerX - 2;
                   triangle[2].y = centerY + 5;
                   Polygon(hdc, triangle, 3);
                }

            }

                

        }
            SelectObject(hdc, oldPen);
            DeleteObject(pen);
            SelectObject(hdc, oldBrush);
            DeleteObject(brush);

            //вывод имя графика
            for (int i = 0; i < PlotName.size(); i++)
            {
                TextOutA(hdc, r.left + 40 + i * 40, r.bottom - 15, PlotName[i].c_str(), std::strlen(PlotName[i].c_str()));

            }
    }
}

void drawGrid(HDC hdc)
{
    pen = CreatePen(PS_USERSTYLE, 2, RGB(150, 150, 150));
    oldPen = (HPEN)SelectObject(hdc, pen);
    brush = CreateSolidBrush(RGB(150, 150, 150));
    oldBrush = (HBRUSH)SelectObject(hdc, brush);

    Rectangle(hdc, r.left+10, r.top+10, r.right-10, r.bottom-20);

    pen = CreatePen(PS_USERSTYLE, 1, RGB(0, 0, 0));
    oldPen = (HPEN)SelectObject(hdc, pen);
    for (int i = 0; i < 2000; i++) //подшаманить чтобы не рисовало за сеткой
    {
        if (i % 20 == 0)
        {
            Line(hdc, 0, i, 2000, i);
            Line(hdc, i, 0, i, 2000);
        }
    }

    TextOut(hdc, 0, 0, L"Нажмите 'Home', чтобы выбрать файл", 34);

    SelectObject(hdc, oldPen);
    DeleteObject(pen);
    SelectObject(hdc, oldBrush);
    DeleteObject(brush);

   
}

long __stdcall WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    PAINTSTRUCT ps;
    HDC hdc;

    GetClientRect(hWnd, &r);

    std::ofstream file;
    switch (message)
    {
    case WM_DESTROY:
     PostQuitMessage(0);
        break;

    case WM_LBUTTONDOWN:
       
        break;

    case WM_KEYDOWN://открывает проводник и читает данные из выбранного файла
        if (wParam == VK_HOME)
        {
          
            OPENFILENAME ofn;

            wchar_t szFileName[MAX_PATH] = L"";
            ZeroMemory(&ofn, sizeof(ofn));
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = hWnd;
            ofn.lpstrFilter = L"Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
            ofn.lpstrFile = szFileName;
            ofn.nMaxFile = MAX_PATH;
            ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
            ofn.lpstrDefExt = L"txt";

            if (GetOpenFileName(&ofn))
                MessageBox(hWnd, ofn.lpstrFile, L"имя файла", MB_OK);

            vecX.clear();
            vecY.clear();
            PlotName.clear();

            std::ifstream file;
            file.open(ofn.lpstrFile);

            file >> N;

            std::string str;
          

            for (int i = 0; i < N; i++) 
            {
                file >> str;
                PlotName.push_back(str);
            }


            while (!file.eof()) {
                double x, y;

                file >> x;
                for (int i = 0; i < N; i++) 
                {
                    vecX.push_back(x);
                }

                for (int i = 0; i < N; i++) 
                {
                    file >> y;
                    vecY.push_back(y);
                }
            }

            flag = true;




            file.close();


        }
        InvalidateRect(hWnd, nullptr, true);

        
        break;
    

    case WM_PAINT:
        hdc = BeginPaint(hWnd, &ps);
       
        drawGrid(hdc);
        DrowGrafic(hdc, r, hWnd);

        EndPaint(hWnd, &ps);
        break;
    default:
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 3);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = windowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, L"Can’t register window class!", L"Win32 API Test", NULL);
        return 1;
    }

    HWND hWnd = CreateWindow(windowClass, windowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
        MessageBox(NULL, L"Can’t create window!", L"Win32 API Test", NULL);
        return 1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;

    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}
