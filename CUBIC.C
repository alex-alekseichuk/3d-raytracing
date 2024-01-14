/***********************************************************************

		      C U B I C   W O R L D

	    Copyright (C)  Alexander Alexeychuk  1998

************************************************************************/


#include<math.h>
#include<conio.h>
#include<stdio.h>
#include<graphics.h>


#define ViewLeft    20
#define ViewTop     50
#define ViewRight   319
#define ViewBottom  249

#define ViewWidth   300
#define ViewHeight  200
#define ViewYCenter 150

#define GFSize       10
#define GFCellSize   10
#define GFCellSize_2 5
#define GFCellSize_d (GFCellSize-.3)

#define gfSpace 0
#define gfWall  1

#define posMax 100

typedef unsigned char BYTE;
typedef float METRIC;
typedef int CELL;
typedef int ANGLE;

METRIC dxbYS, dxeYS, dybYS, dyeYS, dxbXS, dxeXS, dybXS, dyeXS;

CELL GetGFx(METRIC x) { return (CELL)(x/GFCellSize); }
CELL GetGFy(METRIC y) { return (CELL)(y/GFCellSize); }

#define RADIAN(x) ((x)*M_PI/900)

#define INFINITY 1e20

char xcur=5, ycur=5;

METRIC xpos, ypos;
ANGLE apos;

BYTE GameField[GFSize][GFSize] =
{{1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
 {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
 {1, 1, 1, 1, 0, 1, 1, 0, 1, 1},
 {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
 {1, 0, 1, 0, 0, 1, 0, 1, 0, 1},
 {1, 0, 1, 0, 1, 1, 0, 1, 0, 1},
 {1, 0, 0, 0, 1, 0, 0, 1, 0, 1},
 {1, 0, 1, 0, 1, 0, 1, 1, 0, 1},
 {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
 {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};


#define AngleTableSize   1800
#define AngleTableSize_2 900
float adjoining[AngleTableSize];
float opposite[AngleTableSize];
float op_ad[AngleTableSize];
float ad_op[AngleTableSize];


#define MapTop        50
#define MapLeft       350
#define MapCellSize   20
#define MapCellSize_2 10

struct LightStruct {int x,y;} Light[ViewWidth];

char ViewMode = 1;



void BuildAngleTable()
{
  unsigned int i;
  for(i=0;i<AngleTableSize;i++) {
    adjoining[i] = (cos( ((float)i)*M_PI/(180.0*5.0) ));
    opposite[i] = (sin( ((float)i)*M_PI/(180.0*5.0) ));
    if(adjoining[i]!=0) op_ad[i] = opposite[i]/adjoining[i];
    if(opposite[i]!=0) ad_op[i] = adjoining[i]/opposite[i];

/*    if(adjoining[i]!=0)
      op_ad[i] = (int)(sin(((float)i)*M_PI/(180*5.0)) / cos(((float)i)*M_PI/(180*5.0)));
    if(opposite[i]!=0)
      ad_op[i] = (int)(cos(((float)i)*M_PI/(180*5.0)) / sin(((float)i)*M_PI/(180*5.0)));*/
  } /* for */
} /* BuildAngleTable */


  FILE *ff;

float GetWallPos(ANGLE a, CELL *xc, CELL *yc, BYTE *vertical, METRIC *xm, METRIC *ym)
{
  METRIC dX, dY;

  METRIC XDistance=INFINITY, YDistance=INFINITY;
  METRIC x, y, xXS, yXS, xYS, yYS;
  CELL xCellYS, yCellYS, xCellXS, yCellXS;

  if(opposite[a]!=0) dX=GFCellSize*ad_op[a]; else dX=INFINITY;
  if(adjoining[a]!=0) dY=GFCellSize*op_ad[a]; else dY=INFINITY;


  /* ----------- Y STRIP ------------------------ */
  /* if(fabs(dX)>300) goto XLabel; */
  if(a<=450&&( opposite[a]*dxeYS<adjoining[a]*dyeYS )) goto XLabel;
  if(450<a&&a<=900&&( opposite[a]*dxbYS<-adjoining[a]*dyeYS )) goto XLabel;
  if(900<a&&a<=1350&&( -opposite[a]*dxbYS<-adjoining[a]*dybYS )) goto XLabel;
  if(1350<a&&a<=1800&&( -opposite[a]*dxeYS<adjoining[a]*dybYS )) goto XLabel;

  if(0<a && a<900) {
    x= ((y=(1+GetGFy(ypos))*GFCellSize)-ypos)*ad_op[a];
    x=xpos+x;
    do {
      xCellYS=GetGFx(x); yCellYS=GetGFy(y+GFCellSize_2);
      if(GameField[yCellYS][xCellYS]==gfWall) {
        YDistance = sqrt((x-xpos)*(x-xpos) + (y-ypos)*(y-ypos));
	xYS=x; yYS=y;
	goto XLabel;
      } /* if */
      y+=GFCellSize; x+=dX;
      if(x<=0 || x>=posMax || y>=posMax) goto XLabel;
    }while(1); /* do */
  } /* IF(90-180) */

  if(900<a && a<1800) {
    x=(ypos-(y=(GetGFy(ypos))*GFCellSize)) * ad_op[a];
    x=xpos-x;
    do {
      xCellYS=GetGFx(x); yCellYS=GetGFy(y-GFCellSize_2);
      if(GameField[yCellYS][xCellYS]==gfWall) {
        YDistance = sqrt((x-xpos)*(x-xpos) + (y-ypos)*(y-ypos));
	xYS=x; yYS=y;
	goto XLabel;
      }
      y-=GFCellSize; x-=dX;
      if(x<=0 || x>=posMax || y<=0) goto XLabel;
    }while(1);
  } /* IF(180-270) */

XLabel:
  /* if(fabs(dY)>300) goto EndLabel; */
  if(a<=450&&( opposite[a]*dxeXS>adjoining[a]*dyeXS )) goto EndLabel;
  if(450<a&&a<=900&&( opposite[a]*dxbXS>-adjoining[a]*dyeXS )) goto EndLabel;
  if(900<a&&a<=1350&&( -opposite[a]*dxbXS>-adjoining[a]*dybXS )) goto EndLabel;
  if(1350<a&&a<=1800&&( -opposite[a]*dxeXS>adjoining[a]*dybXS )) goto EndLabel;

  if(a<450 || a>1350) {
    y= ((x=(1+GetGFx(xpos))*GFCellSize)-xpos) *op_ad[a];
    y=ypos+y;
    do {
      yCellXS=GetGFy(y); xCellXS=GetGFx(x+GFCellSize_2);
      if(GameField[yCellXS][xCellXS]==gfWall) {
        XDistance = sqrt((x-xpos)*(x-xpos) + (y-ypos)*(y-ypos));
	xXS=x; yXS=y;
	goto EndLabel;
      }
      x+=GFCellSize; y+=dY;
      if(y<=0 || y>=posMax || x>=posMax) goto EndLabel;
    }while(1);
  } /* IF(270-90) */

  if(450<a && a<1350) {
    y= (xpos-(x=(GetGFx(xpos))*GFCellSize)) *op_ad[a];
    y=ypos-y;
    do {
      yCellXS=GetGFy(y); xCellXS=GetGFx(x-GFCellSize_2);
      if(GameField[yCellXS][xCellXS]==gfWall) {
	XDistance = sqrt((x-xpos)*(x-xpos) + (y-ypos)*(y-ypos));
	xXS=x; yXS=y;
	goto EndLabel;
      }
      x-=GFCellSize; y-=dY;
      if(y<=0 || y>=posMax || x<=0) goto EndLabel;
    }while(1);
  } /* IF(90-270) */

EndLabel:
  if(XDistance==INFINITY && YDistance==INFINITY) {
    setcolor(15);circle(400,200,100);
  }

  if(XDistance < YDistance) {
    double modx, mody;
    (*xc)=xCellXS; (*yc)=yCellXS;
    *vertical= 1;
    if(((modx=fmod(xXS, GFCellSize)) < .3 || modx > GFCellSize_d) &&
       ((mody=fmod(yXS, GFCellSize)) < .3 || mody > GFCellSize_d))
	*vertical=2;
    *xm=xXS; *ym=yXS;
    return XDistance;
  } else {
    double modx, mody;
    (*xc)=xCellYS; (*yc)=yCellYS;
    *vertical= 0;
    if(((modx=fmod(xYS, GFCellSize)) < .3 || modx > GFCellSize_d) &&
       ((mody=fmod(yYS, GFCellSize)) < .3 || mody > GFCellSize_d))
	*vertical=2;
    *xm=xYS; *ym=yYS;
    return YDistance;
  }
} /* GetWallPos */


void ShowView()
{
  METRIC distance, xm, ym;
  CELL xc, yc;
  BYTE vertical;
  ANGLE a, n0, apos_;
  unsigned int i;
  float scale;



  dxbYS = xpos;
  dxeYS = posMax - xpos;
  dybYS = ypos - GFCellSize*floor(ypos/GFCellSize);
  dyeYS = ((METRIC)GFCellSize) - dybYS;

  dxbXS = xpos - GFCellSize*floor(xpos/GFCellSize);
  dxeXS = ((METRIC)GFCellSize) - dxbXS;
  dybXS = ypos;
  dyeXS = posMax - ypos;



  for(i=0,a=apos;i<ViewWidth;i++,a++) {
    if(a==AngleTableSize) a=0;

    distance = GetWallPos(a, &xc, &yc, &vertical, &xm, &ym);
    apos_=apos+150; if(apos_>=360) apos_=apos_-360;
    n0=abs(i-150);
    scale = fabs(1000/distance);
    scale /= fabs(adjoining[n0]);
    if(scale<1) scale=1; if(scale>ViewHeight) scale=ViewHeight;

    moveto(ViewRight-i, ViewTop);
    setcolor(1);
    lineto(ViewRight-i, ViewYCenter-scale/2);
    switch(vertical) {
	case 0 : setcolor(8);break;
	case 1 : setcolor(6);break;
	case 2 : setcolor(0);break;
    }
    lineto(ViewRight-i, ViewYCenter+scale/2);
    setcolor(2);
    lineto(ViewRight-i, ViewBottom+1);

    Light[i].x = MapLeft + xm*MapCellSize/GFCellSize;
    Light[i].y = MapTop + ym*MapCellSize/GFCellSize;
  }

} /* ShowView */




/*************************************************************************
**************************************************************************
**************************************************************************/

void ShowMapCell(char x, char y)
{
  switch(GameField[y][x]) {
    case gfSpace :
      setfillstyle(1,2);
      bar(MapLeft+x*MapCellSize, MapTop+y*MapCellSize,
	  MapLeft+(x+1)*MapCellSize, MapTop+(y+1)*MapCellSize);
      break;
    case gfWall :
      setfillstyle(9,6);
      bar(MapLeft+x*MapCellSize, MapTop+y*MapCellSize,
	  MapLeft+(x+1)*MapCellSize, MapTop+(y+1)*MapCellSize);
      break;

  }/*switch*/
}/* ShowMapCell */

void ShowMapField()
{
 char i,j;
 for(i=0;i<GFSize;i++)
   for(j=0;j<GFSize;j++)
     ShowMapCell(i, j);
 setcolor(0);
 rectangle(MapLeft-2, MapTop-2, MapLeft+GFSize*MapCellSize+2, MapTop+GFSize*MapCellSize+2);
} /* ShowMapField */

void ShowYou(char full)
{
  int x, y, i;
  setwritemode(1);
  if(full) {
    x=MapLeft + xpos*MapCellSize/GFCellSize;
    y=MapTop + ypos*MapCellSize/GFCellSize;
    setcolor(14);
    rectangle(x-2, y-2, x+2, y+2);
    setcolor(9);
    for(i=0;i<300;i+=30) {
	if(i==150) {setcolor(14); setlinestyle(0,0,3);}
	line(x, y, Light[i].x, Light[i].y);
	if(i==150) {setcolor(9); setlinestyle(0,0,1);}
    }
  } else {
    x=MapLeft + GFCellSize*GetGFx(xpos)*MapCellSize/GFCellSize + MapCellSize_2;
    y=MapTop + GFCellSize*(GetGFy(ypos))*MapCellSize/GFCellSize + MapCellSize_2;
    setcolor(12);
    rectangle(x-2, y-2, x+2, y+2);
  }
  setwritemode(0);
} /* ShowYou */




#define step 2
ANGLE p30(ANGLE a)
{
a+=150;
if(a>=AngleTableSize) a-=AngleTableSize;
return a;
}

void ShowCur()
{setwritemode(1);
  setcolor(12);
  rectangle(MapLeft+xcur*MapCellSize, MapTop+ycur*MapCellSize,
	  MapLeft+(xcur+1)*MapCellSize, MapTop+(ycur+1)*MapCellSize);
setwritemode(0);}

void SwitchMode()
{
  if(ViewMode) {
    ShowYou(1);  ViewMode=0;
    ShowYou(0);
    ShowCur();
    gotoxy(20, 20);printf("  Edit Mode  ");
  } else {
    ShowCur();ShowYou(0); ViewMode=1; ShowView(); ShowYou(1);
    gotoxy(20, 20);printf("  View Mode  ");
  }
} /* SwitchMode */



void main()
{
  int gd=0,gm, key;
  int a_a_a;
  METRIC xnew, ynew;

  BuildAngleTable();
/*  _setvideomode(_MRES16COLOR);*/
  initgraph(&gd, &gm, '');
  setfillstyle(1,7);bar(0,0,getmaxx(),getmaxy());

  gotoxy(2,2);printf("     C U B I C   W O R L D        Copyright (C) Alexander Alexeychuk  1998    ");
  setcolor(0);line(0,349,getmaxx(),349);
  gotoxy(45,18);printf(" Esc-Exit   Tab-Switch mode ");
  gotoxy(20, 20);printf("  View Mode  ");
  setcolor(0);rectangle(ViewLeft-2, ViewTop-2, ViewRight+2, ViewBottom+3);

  xpos=15; ypos=15; apos=0;
  ShowMapField();

  ShowView();
  ShowYou(1);
  do {

    gotoxy(3,18); a_a_a=330-apos/5; if(a_a_a<0) a_a_a+=360;
    printf(" Angle= %3d    X= %4.1f    Y= %4.1f ", a_a_a, xpos, ypos);
    key=bioskey(0);
    switch(key) {
      case 19200 : /* left */
	if(ViewMode) {
	  ShowYou(1);
	  apos+=60;
	  if(apos>AngleTableSize) apos-=AngleTableSize;
	  ShowView();
	  ShowYou(1);
	} else { ShowCur(); xcur--; if(xcur==0) xcur=GFSize-2; ShowCur(); }
	break;

      case 19712 : /* right */
        if(ViewMode) {
	  ShowYou(1);
	  if(apos<30) apos+=AngleTableSize;
	  apos-=60;
	  ShowView();
	  ShowYou(1);
	} else { ShowCur(); xcur++; if(xcur==GFSize-1) xcur=1; ShowCur(); }
	break;

      case 18432 : /* up */
        if(ViewMode) {
	  xnew=xpos+(1+step)*adjoining[p30(apos)];
	  ynew=ypos+(1+step)*opposite[p30(apos)];
	  if(GameField[GetGFy(ynew)][GetGFx(xnew)]!=gfWall)
	    {ShowYou(1);
	     xpos=xpos+step*adjoining[p30(apos)];
	     ypos=ypos+step*opposite[p30(apos)];
	     ShowView();
	     ShowYou(1);}
	} else { ShowCur(); ycur--; if(ycur==0) ycur=GFSize-2; ShowCur(); }
	break;

      case 20480 : /* down */
        if(ViewMode) {
	  xnew=xpos-(1+step)*adjoining[p30(apos)];
	  ynew=ypos-(1+step)*opposite[p30(apos)];
	  if(GameField[GetGFy(ynew)][GetGFx(xnew)]!=gfWall)
	    {ShowYou(1);
	     xpos=xpos-step*adjoining[p30(apos)];
	     ypos=ypos-step*opposite[p30(apos)];
	     ShowView();
	     ShowYou(1);}
	} else { ShowCur(); ycur++; if(ycur==GFSize-1) ycur=1; ShowCur(); }
	break;

	case 3849 : /* Tab */
	  SwitchMode();
	  break;

	case 14624 : /* Space */
	  if(!ViewMode && (xcur!=GetGFx(xpos)||ycur!=GetGFy(ypos))) {
	    ShowCur();
	    if(GameField[ycur][xcur]==gfSpace) GameField[ycur][xcur]=gfWall;
	    else GameField[ycur][xcur]=gfSpace;
	    ShowMapCell(xcur, ycur);
	    ShowCur();
	  }
	  break;

    } /* switch*/
  } while(key!=283);
/*  _setvideomode(_DEFAULTMODE);*/
closegraph();
} /* main */
