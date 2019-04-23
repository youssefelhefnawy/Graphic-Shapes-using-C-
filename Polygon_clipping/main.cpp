#if defined(UNICODE) && !defined(_UNICODE)
    #define _UNICODE
#elif defined(_UNICODE) && !defined(UNICODE)
    #define UNICODE
#endif

#include <tchar.h>
#include <windows.h>
#include<bits/stdc++.h>
using namespace std;
class Point
{
public:
    int x;
    int y;
    Point(int frst,int scnd)
    {
        x=frst;
        y=scnd;
    }
};
//Functions
void Draw_before_clip(HDC hdc,COLORREF c,vector<Point>Result);
int x_intersect(int x1, int y1, int x2, int y2,
                int x3, int y3, int x4, int y4) ;
int y_intersect(int x1, int y1, int x2, int y2,
                int x3, int y3, int x4, int y4);
void Polyg_Clip(HDC hdc,COLORREF c,vector<Point> points);
void Set_window(int x1,int yone,int x2,int y2,HDC hdc,COLORREF c);
void DDA(int X0, int Y0, int X1, int Y1,HDC hdc,COLORREF c);

int top,bottom,Left,Right;
/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
/*  Make the class name into a global variable  */
TCHAR szClassName[ ] = _T("CodeBlocksWindowsApp");

int WINAPI WinMain (HINSTANCE hThisInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszArgument,
                     int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           _T("Code::Blocks Template Windows App"),       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           544,                 /* The programs width */
           375,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}


/*  This function is called by the Windows function DispatchMessage()  */
int x1,x2,yone,y2;
int clickcnt=0;
vector<Point> points;
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    HDC hdc;
    hdc =GetDC(hwnd);
    COLORREF c;
    c=RGB(255,0,255);
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        case WM_LBUTTONDOWN:
        {
            yone=HIWORD(lParam);
            x1=LOWORD(lParam);
                break;
        }
        case WM_LBUTTONUP:
        {
                y2=HIWORD(lParam);
                x2=LOWORD(lParam);
                Set_window(x1,yone,x2,y2,hdc,c);
                break;
        }
        case WM_RBUTTONDOWN :
        {
            clickcnt++;
                int y=HIWORD(lParam);
                int x=LOWORD(lParam);
                if(clickcnt<4)
                {
                    points.push_back(Point(x,y));
                }
                else
                {
                    points.push_back(Point(x,y));
                    COLORREF g;
                    g=RGB(0,0,0);
                  Polyg_Clip(hdc,g,points);
              //      Draw_before_clip(hdc,c,points);
                    clickcnt=0;
                    points.clear();
                }
                break;
        }

        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}
int x_intersect(int x1, int y1, int x2, int y2,
                int x3, int y3, int x4, int y4)
{
    int num = (x1*y2 - y1*x2) * (x3-x4) -
              (x1-x2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    return num/den;
}

// Returns y-value of point of intersectipn of
// two lines
int y_intersect(int x1, int y1, int x2, int y2,
                int x3, int y3, int x4, int y4)
{
    int num = (x1*y2 - y1*x2) * (y3-y4) -
              (y1-y2) * (x3*y4 - y3*x4);
    int den = (x1-x2) * (y3-y4) - (y1-y2) * (x3-x4);
    return num/den;
}

void Polyg_Clip(HDC hdc,COLORREF c,vector<Point> points)
{
vector<Point>aftertop;
vector<Point>afterright;
vector<Point>afterbottom;
vector<Point>Result;
////Top Side Clipping
for(int i=0;i<points.size();i++)
{   int k=i+1;
    if(i==points.size()-1)
    {
        k=0;
    }
    //1st case if both Vertices in the Edge are outside
    if(points[i].y<top &&points[k].y<top)
    {
        //Don't do anything
    }
//    First vertex outside and the 2nd  is inside
    else if(points[i].y<top &&points[k].y>top)
    {
        int  x=x_intersect(points[i].x,points[i].y,points[k].x,points[k].y,Left,top,Right,top);
        int  y=y_intersect(points[i].x,points[i].y,points[k].x,points[k].y,Left,top,Right,top);
        Point p(x,y);
        aftertop.push_back(p);
        aftertop.push_back(Point(points[k].x,points[k].y));
    }
    else if(points[i].y>top &&points[k].y>top)
    {
        aftertop.push_back(Point(points[k].x,points[k].y));
    }
    else
    {
          int x=x_intersect(points[i].x,points[i].y,points[k].x,points[k].y,Left,top,Right,top);
        int  y=y_intersect(points[i].x,points[i].y,points[k].x,points[k].y,Left,top,Right,top);
        Point p(x,y);
        aftertop.push_back(p);
    }

}
////Right Side Clipping
for(int i=0;i<aftertop.size();i++)
{
      int k=i+1;
    if(i==aftertop.size()-1)
    {
        k=0;
    }
    //1st case if both Vertices in the Edge are outside
    if(aftertop[i].x>Right && aftertop[k].x>Right)
    {
        //Don't do anything
    }
    //First vertex outside and the 2nd  is inside
    else if(aftertop[i].x>Right&&aftertop[k].x<=Right)
    {
        int  x=x_intersect(aftertop[i].x,aftertop[i].y,aftertop[k].x,aftertop[k].y,Right,top,Right,bottom);
        int  y=y_intersect(aftertop[i].x,aftertop[i].y,aftertop[k].x,aftertop[k].y,Right,top,Right,bottom);
        Point p(x,y);
        afterright.push_back(p);
        afterright.push_back(Point(aftertop[k].x,aftertop[k].y));
    }
    ///First vertex inside 2nd outside
    else if(aftertop[i].x<Right &&aftertop[k].x>Right)
    {
           int x=x_intersect(aftertop[i].x,aftertop[i].y,aftertop[k].x,aftertop[k].y,Right,top,Right,bottom);
        int  y=y_intersect(aftertop[i].x,aftertop[i].y,aftertop[k].x,aftertop[k].y,Right,top,Right,bottom);
        Point p(x,y);
        afterright.push_back(p);
    }
    ///Both inside
    else
    {
          afterright.push_back(Point(aftertop[k].x,aftertop[k].y));


    }
}
////
//Bottom Side Clipping

for(int i=0;i<afterright.size();i++)
{
    int k=i+1;
    if(i==afterright.size()-1)
    {
        k=0;
    }
    //1st case if both Vertices in the Edge are outside
    if(afterright[i].y>bottom && afterright[k].y>bottom)
    {
        //Don't do anything
    }
    //First vertex outside and the 2nd  is inside
    else if(afterright[i].y>bottom&&afterright[k].y<=bottom)
    {
        int  x=x_intersect(afterright[i].x,afterright[i].y,afterright[k].x,afterright[k].y,Left,bottom,Right,bottom);
        int  y=y_intersect(afterright[i].x,afterright[i].y,afterright[k].x,afterright[k].y,Left,bottom,Right,bottom);
        Point p(x,y);
        afterbottom.push_back(p);
        afterbottom.push_back(Point(afterright[k].x,afterright[k].y));
    }
    ///First vertex inside 2nd outside
    else if(afterright[i].y<bottom &&afterright[k].y>bottom)
    {
          int x=x_intersect(afterright[i].x,afterright[i].y,afterright[k].x,afterright[k].y,Left,bottom,Right,bottom);
        int  y=y_intersect(afterright[i].x,afterright[i].y,afterright[k].x,afterright[k].y,Left,bottom,Right,bottom);
        Point p(x,y);
        afterbottom.push_back(p);
    }
    ///Both inside
    else
    {

        afterbottom.push_back(Point(afterright[k].x,afterright[k].y));
    }
}
//Left Side Clipping
for(int i=0;i<afterbottom.size();i++)
{
    int k=i+1;
    if(i==afterbottom.size()-1)
    {
        k=0;
    }
    //1st case if both Vertices in the Edge are outside
    if(afterbottom[i].x<Left && afterbottom[k].x<Left)
    {
        //Don't do anything
    }
    //First vertex outside and the 2nd  is inside
    else if(afterbottom[i].x<Left&&afterbottom[k].y>=Left)
    {
        int  x=x_intersect(afterbottom[i].x,afterbottom[i].y,afterbottom[k].x,afterbottom[k].y,Left,bottom,Left,top);
        int  y=y_intersect(afterbottom[i].x,afterbottom[i].y,afterbottom[k].x,afterbottom[k].y,Left,bottom,Left,top);
        Point p(x,y);
        Result.push_back(p);
        Result.push_back(Point(afterbottom[k].x,afterbottom[k].y));
    }
    ///First vertex inside 2nd outside
    else if(afterbottom[i].x>Left &&afterbottom[k].x<Left)
    {
            int  x=x_intersect(afterbottom[i].x,afterbottom[i].y,afterbottom[k].x,afterbottom[k].y,Left,bottom,Left,top);
        int  y=y_intersect(afterbottom[i].x,afterbottom[i].y,afterbottom[k].x,afterbottom[k].y,Left,bottom,Left,top);
        Point p(x,y);
        Result.push_back(p);
    }
    ///Both inside
    else
    {
               Result.push_back(Point(afterbottom[k].x,afterbottom[k].y));

    }
}

//Draw
if(Result.size()!=0)
{
    Draw_before_clip(hdc,c,Result);
}
}
void Draw_before_clip(HDC hdc,COLORREF c,vector<Point>Result)
{

    for(int i=0;i<Result.size()-1;i++)
{
    DDA(Result[i].x,Result[i].y,Result[i+1].x,Result[i+1].y,hdc,c);
}
    DDA(Result[0].x,Result[0].y,Result[Result.size()-1].x,Result[Result.size()-1].y,hdc,c);
   // DDA(Result[Result.size()-1].x,Result[Result.size()-1].y,Result[0].x,Result[0].y,hdc,c);
}
void  Set_window(int x1,int yone,int x2,int y2,HDC hdc,COLORREF c)
{
DDA(x1,yone,x2,yone,hdc,c);
DDA(x1,yone,x1,y2,hdc,c);
DDA(x1,y2,x2,y2,hdc,c);
DDA(x2,yone,x2,y2,hdc,c);
 if(yone>y2)
 {
     bottom=yone;
     top=y2;
 }
 else
 {
     top=yone;
     bottom=y2;
 }
 if(x1>x2)
 {
     Right=x1;
     Left=x2;
 }
 else
 {
     Right=x2;
     Left=x1;
 }
}
void DDA(int X0, int Y0, int X1, int Y1,HDC hdc,COLORREF c)
{
    // calculate dx & dy
    int dx = X1 - X0;
    int dy = Y1 - Y0;

    // calculate steps required for generating pixels
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    // calculate increment in x & y for each steps
    float Xinc = dx / (float) steps;
    float Yinc = dy / (float) steps;

    // Put pixel for each step
    float X = X0;
    float Y = Y0;
    for (int i = 0; i <= steps; i++)
    {
        SetPixel(hdc,X,Y,c);  // put pixel at (X,Y)
        X += Xinc;           // increment in x at each step
        Y += Yinc;           // increment in y at each step
                             // generation step by step
    }
}
