#ifndef __INC_DIRECTOSD
#define __INC_DIRECTOSD


class DirectOSD
{
public:
	static void Construct();

	static void SetPixel(int x, int y, word color);
	static void DrawLine(int x1, int y1, int x2, int y2, word color);
	static void DrawRect(int x, int y, int width, int height, int thickness, word color);
	static void FillRect(int x, int y, int width, int height, word color);
	static void FillRect(int x, int y, int width, int height, word color, word increment);
	static void Copy(int srcX, int srcY, int width, int height, int destX, int destY);
	static void FrameRect(int x1, int y1, int width, int height, int borderThickness, word fillColor, word borderColor);
	static void FrameRoundRect(int x, int y, int width, int height, int radius, int borderThickness, word fillColor, word borderColor);
	static void RoundRectTop(int x, int y, int width, int height, int radius, int borderThickness, word fillColor);
	static void FrameRoundRectTop(int x, int y, int width, int height, int radius, int borderThickness, word fillColor, word borderColor);
	static void RoundRectBottom(int x, int y, int width, int height, int radius, int borderThickness, word fillColor);
	static void FrameRoundRectBottom(int x, int y, int width, int height, int radius, int borderThickness, word fillColor, word borderColor);
	static void FrameRoundRectShadow(int x, int y, int width, int height, int radius, int borderThickness, word fillColor, word borderColor, word outerColor);
	static void DrawCorner(int x, int y, int n, word fillColor);
	static void DrawCorner(int x, int y, int n, word fillColor, word borderColor);
	static void DrawCorner(int x, int y, int n, word fillColor, word borderColor, word outerColor);

private:
	enum {MAX_SCREEN_X = 720, MAX_SCREEN_Y = 576};

	static word* m_lineAddr[MAX_SCREEN_Y];
};

inline void DirectOSD::SetPixel(int x, int y, word color)
{
	if (x <= MAX_SCREEN_X && y <= MAX_SCREEN_Y && (x >= 0 && y >= 0))   
	{
  		*(m_lineAddr[y]+x) = color;
	}
}

extern DirectOSD screen;


#endif
