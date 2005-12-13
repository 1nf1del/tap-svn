/************************************************************
				Part of the UK TAP Project
		This module handles display of  a Progress Bar

Name	: EventInfo.c
Author	: Darkmatter and IanP
Version	: 0.0
For		: Topfield TF5x00 series PVRs
Licence	:
Descr.	:
Usage	:
History	: v0.1 Kidhazy 18-08-05	Inception Date

	Last change:  USE   18 Aug 105    5:15 pm
**************************************************************/


//------------------------------ DrawLine --------------------------------------
//
// Draw a single vertical line of one colour.
//
void DrawLine(word scrrgn, int X, int Y, int lineheight, int lineColor)
{
    int tempY;
    for (tempY=0; tempY<lineheight;  tempY++)
    {
        TAP_Osd_PutPixel( scrrgn, X, Y+tempY, lineColor);
    }
}

//------------------------------ DrawLineColour --------------------------------------
//
// Determines the colour of the line to be drawn, based on style of bar and percentage.
//
int DrawLineColour(int percent)
{
    int red;    int green;    int blue;
    int LineDrawcolour;
    
    // Determine the appropriate RGB value for the line.
    if (percent >= 50)        red = 31;
    else                      red = percent * 31 /50;

    if (percent <= 50)        green = 31;
    else                      green = 31 - ( (percent -50) * 31 /50 );

    blue = 0;

    LineDrawcolour = RGB( red, green, blue);
    return LineDrawcolour;
}

//------------------------------ DisplayProgressBar --------------------------------------
//
// Displays a graphical progress bar.
//
// scrrgn   - region of screen bar is to be copied to.
// amount   - the amount to be graphed.
// max_amount - the maximum amount that can be graphed.
// x_pos      - x coord of the graph
// y_pos      - y coord of the graph
// linewidth  - width of the graph
// lineheight - height of the graph
// freeSpaceColour - colour of the background of the graph
// BoxLineWidth    - width  of the line surrounding  the graph
// BoxLineColour   - colour of the line surrounding the graph
// style           - style of bar to draw.
//                   0 = colour gradient fill
//                   1 = solid colour partial fill
//                   2 = solid colour full bar
//                  >2 = colour partial fill 
//
void DisplayProgressBar(word scrrgn, dword amount, dword max_amount, int x_pos, int y_pos, int linewidth, int lineheight, int freeSpaceColour, int BoxLineWidth, int BoxLineColour, int style)
{
    int amount_percent;  int line_percent;  int percent;
    int wf;
    int LineDrawcolour;
    int tempX;
    int GradientFill = 0; int SolidColourBar = 1; int FullSolidBar = 2;  

    if (max_amount <= 0) max_amount=1;           // Check we're not out of bounds. 
    if (amount > max_amount) amount=max_amount;  // Check we're not out of bounds.
    if (amount < 0)   amount=0;                  // Check we're not out of bounds.

    amount_percent = amount * 100 / max_amount;
    
    if (style != FullSolidBar)
    {    // Make the graph a percentage of the line width.
         wf = linewidth * amount / max_amount;      // calculate relative position for red/green transition
    }
    else
    {   //Style #3 fills up the entire line with 1 colour.
        wf = linewidth;
    }
    wf = wf - 2 * BoxLineWidth;  // Don't draw on the rectangle.
    // draw full graph using free space colour
    TAP_Osd_FillBox(scrrgn, x_pos, y_pos, linewidth, lineheight, freeSpaceColour);

    // Loop through to draw the lines that make up the graph.
    for (tempX=0; tempX<wf;  tempX++)
    {
        line_percent = (tempX * 100) / linewidth;    // Determine what percentage of graph are we up to.
        switch (style == GradientFill)               // Determine which percentage variable to use to determine colour.
        {
           case TRUE : percent = line_percent;
                              break;
           default : percent = amount_percent;
        }
        if (style <= FullSolidBar) LineDrawcolour = DrawLineColour( percent );    // Determine colour of line
        else LineDrawcolour = style;                                  // Use the style as the colour.
        DrawLine(scrrgn, x_pos+tempX+BoxLineWidth, y_pos, lineheight, LineDrawcolour);         // Draw one single line
    }
    // Draw Box around Bar
    TAP_Osd_DrawRectangle(scrrgn, x_pos, y_pos , linewidth, lineheight, BoxLineWidth, BoxLineColour);

}

