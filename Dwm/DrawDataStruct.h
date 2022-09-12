#pragma once
#include "OwnImGui.h"

const int TEXTMAX = 256;
const int DRAWMAX = 0x2500;

struct _Text
{
	ImVec2 Pos;
	float  Size;
	ImVec4 Color;
	char   Text[TEXTMAX];
	bool   Stroke;
};

struct _Rect
{
	ImVec2 Pos;
	float  Width;
	float  Height;
	ImVec4 Color;
	float  Thickness;
};

struct _RectFilled
{
	ImVec2 Pos;
	float  Width;
	float  Height;
	ImVec4 Color;
	float  Rounding;
};

struct _Line
{
	ImVec2 From;
	ImVec2 To;
	ImVec4 Color;
	float  Thickness;
};

struct _Circle
{
	ImVec2 Pos;
	float  Radius;
	ImVec4 Color;
	float  Thickness;
};

enum DRAWTYPE {
	TYPE_TEXT,
	TYPE_RECT,
	TYPE_LINE,
	TYPE_CIRCLE,
	TYPE_RECTFILLED
};

struct DrawInfo
{
	DRAWTYPE Type;
	union
	{
		_Text	Text;
		_Rect	Rect;
		_Line	Line;
		_Circle Circle;
		_RectFilled RectFilled;
	}_From;
};

typedef struct DrawList
{
	bool	ServerMutex = true;
	bool	ClientMutex = false;
	int		DrawCount = 0;
	DrawInfo	DrawDataArray[DRAWMAX]{};
}DRAWLIST, * PDRAWLIST;
