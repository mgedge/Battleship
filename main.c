/**********************CREDITS*************************
 * Title:		Battleship Game
 * Authors:		Matthew Gedge
								Robert Witzlib
 * 
 * Description: Graphical game similar to Battleship.
 * Players alternate to shoot onto a grid until all enemy
 * ships are destroyed.	
 ***********************NOTES*************************	   
 * Grid data is set by one of several integers:
 * 0 - indicates a water tile
 * 1 - indicates a ship tile
 * 		#ship tiles are no longer visible after setting up the board
 * 2 - indicates a miss tile
 * 3 - indicates a hit tile
 *******************GLOBAL VARIABLES******************/	
#include "lab_buttons.h"
#include <stdio.h>
#include <stdlib.h>

#define MOVE_PIXELS   	1							// Number of Pixels to move each time.
#define LCD_SIZE_X			240						// X Size of LCD screen in pixels
#define LCD_SIZE_Y			320						// Y Size of LCD screen in pixels
#define LCD_MAX_DRAW_X	LCD_SIZE_X-1	// 0 to LCD_MAX_DRAW_X is = LCD_SIZE_X
#define LCD_MAX_DRAW_Y	LCD_SIZE_Y-1	// 0 to LCD_MAX_DRAW_Y is = LCD_SIZE_Y
#define LCD_HALF_SIZE_X	LCD_SIZE_X/2	// X center of screen in pixels
#define LCD_HALF_SIZE_Y	LCD_SIZE_Y/2	// Y center of screen in pixels
#define	LCD_INIT_X			LCD_HALF_SIZE_X - BUCKY_WIDTH_PXL/2		// Start image in the center of the screen X
#define	LCD_INIT_Y			LCD_HALF_SIZE_Y - BUCKY_HEIGHT_PXL/2	// Start image in the center of the screen 
	
	#if !defined (LCD_COLOR_WHITE)
  #define LCD_COLOR_WHITE			0xFFFF
  #define LCD_COLOR_BLACK			0x0000
  #define LCD_COLOR_RED			  0xF800
  #define LCD_COLOR_GREEN			0x07E0
  #define LCD_COLOR_GREEN2		0xB723
  #define LCD_COLOR_BLUE			0x001F
  #define LCD_COLOR_BLUE2			0x051D
  #define LCD_COLOR_YELLOW		0xFFE0
  #define LCD_COLOR_ORANGE		0xFBE4
  #define LCD_COLOR_CYAN			0x07FF
  #define LCD_COLOR_MAGENTA		0xA254
  #define LCD_COLOR_GRAY			0x7BEF 
  #define LCD_COLOR_BROWN			0xBBCA
#endif

	//Defines the number of ships each player gets
	int player1Ships = 7;
	int player2Ships = 7;
	
	//Defines the player numbers
	int player1 = 1;
	int player2 = 2;
	
	//Game settings
	int gameSpeed = 3; 
	int boardSize = 7;
	uint8_t 	direction;
	int randSeed = -32767;
	
	//Declare the player data grids
	int player1Grid[7][7];
	int player2Grid[7][7];
//*****************************************************************************//
void initializeBoard(int player1Grid[7][7], int player2Grid[7][7])
	// Sets up the 2 player boards so all values are 0 (seen as water)
{		
	for(int y = 0; y < 7; y++)
	{
		for(int x = 0; x < 7; x++)
		{
			player1Grid [x][y] = 0;
			player2Grid [x][y] = 0;
		}
	}
}

void drawNumbers(int x, int y)
	//BUGGED: Draws characters on the x axis of the grids
{
	x += 10;

	int index = 0;
	char* numbers[] = {"1","2","3","4","5","6","7"};
	
	for(int b = 6; b >= 0; b--)
	{
		char* character = numbers[b];
		ece210_lcd_print_string(character, x, y, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
		
		x += 12;
		index++;
	}
}

void drawLetters(int x, int y)
	//BUGGED: Draws numbers on the y axis of the grids
{
	x += 14;
	y += 11;
	
	int index = 0;
	char* letters[] = {"A","B","C","D","E","F","G"};
	
	for(int a = 0; a <=6; a++)
	{
		char* character = letters[a];
		ece210_lcd_print_string(character, x, y, LCD_COLOR_BLACK, LCD_COLOR_BLUE);
		
		x += 12;
		index++;
	}
}

void drawGrid(int x, int y)
	//Creates the grid and colors the boards
{
	//Initialize variables
	int sX = x;
	int sY = y;
	
	//Colorize Boards
	ece210_lcd_draw_rectangle(x, 96, y, 96, LCD_COLOR_BLUE2);
	
	//Colorize Headers
	ece210_lcd_draw_rectangle(x, 12, y, 96, LCD_COLOR_BLUE);
	ece210_lcd_draw_rectangle(x, 96, y, 12, LCD_COLOR_GREEN2);
	
	//Draw Grid
	for(int a = 0; a < 9; a++) 
	{
		ece210_lcd_draw_line(x, y, x + 96, y, LCD_COLOR_BLACK);
		y += 12;
	}
	y=sY;
	
	for(int b = 0; b < 9; b++)
	{
		ece210_lcd_draw_line(x, y, x, y + 96, LCD_COLOR_BLACK);
		x += 12;
	}
	x=sX;

//	drawNumbers(sX, sY);
//	drawLetters(sX, sY);
}

void drawShip(int x, int y, int row, int column)
	//Fills the specified box with a white square
{
	// Coordinates to draw a ship
	int selectedBoxTopLeftX = x + (row * 12);
	int selectedBoxTopLeftY = y + (column * 12);
	
	// Draw a white box to represent a hit
	ece210_lcd_draw_rectangle(selectedBoxTopLeftX, 12, selectedBoxTopLeftY, 12, LCD_COLOR_WHITE);
}

void drawMiss(int x, int y, int row, int column)
	//Fills the specified box with a black square
{
	// Coordinates to draw a miss
	int selectedBoxTopLeftX = x + (row * 12);
	int selectedBoxTopLeftY = y + (column * 12);
	
	// Draw a black box at the coordinates
	ece210_lcd_draw_rectangle(selectedBoxTopLeftX, 13, selectedBoxTopLeftY, 13, LCD_COLOR_BLACK);
}	

void drawHit(int x, int y, int row, int column)
	//Draws two lines at the specified box
{
	// Coordinates to draw a hit
	int selectedBoxTopLeftX = x + (row * 12);
	int selectedBoxTopLeftY = y + (column * 12);
	
	// Draw two diagonal lines to form X to represent a hit
	ece210_lcd_draw_line(selectedBoxTopLeftX, selectedBoxTopLeftY, selectedBoxTopLeftX + 12 , selectedBoxTopLeftY + 12, LCD_COLOR_RED);
	ece210_lcd_draw_line(selectedBoxTopLeftX, selectedBoxTopLeftY +12, selectedBoxTopLeftX + 12, selectedBoxTopLeftY, LCD_COLOR_RED);
}
	
void drawCursorBox(int x, int y, int row, int column)
	//Redraws the black grids then draws four white lines around the box 
{
	//Initialize variables
	int sX = x;
	int sY = y;
	
	// Draw Black Grid (so that as you move red box the previous red box is set back to black)
	for(int a = 0; a < 9; a++) 
	{
		ece210_lcd_draw_line(x, y, x+96, y, LCD_COLOR_BLACK);
		y += 12;
	}
	y=sY;
	
	for(int b = 0; b < 9; b++)
	{
		ece210_lcd_draw_line(x, y, x, y+96, LCD_COLOR_BLACK);
		x += 12;
	}
	x=sX;
	
	// Draw selected box
	int selectedBoxTopLeftX = x + (row * 12);
	int selectedBoxTopLeftY = y + (column * 12);
	
	// Top
	ece210_lcd_draw_line(selectedBoxTopLeftX, selectedBoxTopLeftY, selectedBoxTopLeftX + 12, selectedBoxTopLeftY, LCD_COLOR_WHITE);

	// Right
	ece210_lcd_draw_line(selectedBoxTopLeftX + 12, selectedBoxTopLeftY + 12, selectedBoxTopLeftX + 12, selectedBoxTopLeftY, LCD_COLOR_WHITE);

	// Bottom
	ece210_lcd_draw_line(selectedBoxTopLeftX, selectedBoxTopLeftY + 12, selectedBoxTopLeftX + 12, selectedBoxTopLeftY + 12, LCD_COLOR_WHITE);

	// left
	ece210_lcd_draw_line(selectedBoxTopLeftX, selectedBoxTopLeftY , selectedBoxTopLeftX, selectedBoxTopLeftY+12, LCD_COLOR_WHITE);
}

void drawShipNumber(int player)
	//Writes the number of ships remaining for the determined player
{
	//Declare variables
	int numberShips;
	int xStart;
	int yStart;
	
	//Determine where the player's number is drawn
	if(player == 1){
		xStart = 70;
		yStart = 30;
	}
	if(player == 2){
		xStart = 70;
		yStart = LCD_HALF_SIZE_Y + 30;
	}
	
	//Determine who's number is drawn
	if(player == 1){
		numberShips = player1Ships;
	}
	if(player == 2){
		numberShips = player2Ships;
	}
	
	//Draw the number
	switch(numberShips)
	{
		case 0:
			ece210_lcd_print_string("0", xStart, yStart, LCD_COLOR_RED, LCD_COLOR_WHITE);
			break;
		case 1:
			ece210_lcd_print_string("1", xStart, yStart, LCD_COLOR_RED, LCD_COLOR_WHITE);
			break;
		case 2:
			ece210_lcd_print_string("2", xStart, yStart, LCD_COLOR_RED, LCD_COLOR_WHITE);
			break;
		case 3:
			ece210_lcd_print_string("3", xStart, yStart, LCD_COLOR_RED, LCD_COLOR_WHITE);
			break;
		case 4:
			ece210_lcd_print_string("4", xStart, yStart, LCD_COLOR_RED, LCD_COLOR_WHITE);
			break;
		case 5:
			ece210_lcd_print_string("5", xStart, yStart, LCD_COLOR_RED, LCD_COLOR_WHITE);
			break;
		case 6:
			ece210_lcd_print_string("6", xStart, yStart, LCD_COLOR_RED, LCD_COLOR_WHITE);
			break;
		case 7:
			ece210_lcd_print_string("7", xStart, yStart, LCD_COLOR_RED, LCD_COLOR_WHITE);
			break;
	}
}	

void drawPlayers()
	//Draws player's title and ships remaining
{
	//Player 1
	ece210_lcd_print_string(" PLAYER 1", 210, 10, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
	ece210_lcd_print_string(" Ships Remaining: ", 210, 30, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
	drawShipNumber(1);
	
	//Player 2
	ece210_lcd_print_string(" PLAYER 2", 210, LCD_HALF_SIZE_Y + 10, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
	ece210_lcd_print_string(" Ships Remaining: ", 210, LCD_HALF_SIZE_Y + 30, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
	drawShipNumber(2);
}

void drawBoard()
	//Draws the player's info, the grids, and a half line
{	
	//Base
	ece210_lcd_draw_line(0, LCD_HALF_SIZE_Y, 240, LCD_HALF_SIZE_Y, LCD_COLOR_BLACK);
	
	//Player 1
	ece210_lcd_print_string("PLAYER 1", 210, 10, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
	ece210_lcd_print_string("Ships Remaining: ", 210, 30, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
	drawShipNumber(1);
	drawGrid(72, 50);
	
	//Player 2
	ece210_lcd_print_string("PLAYER 2", 210, LCD_HALF_SIZE_Y + 10, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
	ece210_lcd_print_string("Ships Remaining: ", 210, LCD_HALF_SIZE_Y + 30, LCD_COLOR_BLACK, LCD_COLOR_WHITE);
	drawShipNumber(2);
	drawGrid(72, LCD_HALF_SIZE_Y + 50);
}
void eraseBoard()
	//Draws a white rectangle over the lcd screem
{
	ece210_lcd_draw_rectangle(0, 240, 0, 320, LCD_COLOR_WHITE);
}

void clearBoard()
	//Draws a black rectangle over the lcd screen
{
	ece210_lcd_draw_rectangle(0, 240, 0, 320, LCD_COLOR_BLACK);
}

void paint()
	//Erases the board, then draws it from scratch
{
	eraseBoard();
	drawBoard();
}

void highlightPlayer(int player)
	//Draw a solid red rectangle around the determined player's title
{
	//Declare variables
	int xStart;
	int yStart;
		
	//Determine who is highlighted
	if(player == 1){
		xStart = 25;
		yStart = 7;
		ece210_lcd_draw_rectangle(xStart, 89, yStart, 23, LCD_COLOR_RED);
		drawPlayers();
	}
	if(player == 2){
		xStart = 25;
		yStart = LCD_HALF_SIZE_Y + 7;
		ece210_lcd_draw_rectangle(xStart, 89, yStart, 23, LCD_COLOR_RED);
		drawPlayers();
	}
}

void removeHighlightPlayer(int player)
	//Removes the solid red rectangle around the determined player's title
{
	//Declare variables
	int xStart;
	int yStart;
		
	//Determine which player's highlight is removed
	if(player == 1){
		xStart = 25;
		yStart = 7;
		ece210_lcd_draw_rectangle(xStart, 89, yStart, 23, LCD_COLOR_WHITE);
		drawPlayers();
	}
	if(player == 2){
		xStart = 25;
		yStart = LCD_HALF_SIZE_Y + 7;
		ece210_lcd_draw_rectangle(xStart, 89, yStart, 23, LCD_COLOR_WHITE);
		drawPlayers();
	}
}	

void skipLines(int lines)
	//Skips determined lines in console
{
	for(int a = 0; a < lines; a++)
		ece210_lcd_add_msg("", TERMINAL_ALIGN_LEFT, LCD_COLOR_BLACK);
}
	
void playerBoardSetup(int playerNumber, int isPlayerAI, int playerGrid[7][7])
	//Allows the player to construct their board
{
		//Determine if player is an AI
		if(isPlayerAI == 1)
		{
			int xCoordinate = 0;
			int yCoordinate = 0;
			
			for(int remainingShips = 0; remainingShips < 8; remainingShips++)
			{	
				//Generate coordinates until the coordinates are empty,
				//the valid coordinates are turned into a ship tile
				do
				{
					xCoordinate = (int)(rand()%7)+1;
					yCoordinate = (int)(rand()%7)+1;
				}
				while(playerGrid[yCoordinate][xCoordinate] == 1);				

				playerGrid[yCoordinate][xCoordinate] = 1;
			}
		}
		//If the player is not an AI
		else
		{
			//Draw the board and highlight the player
			paint();
			highlightPlayer(playerNumber);
			
			//Declare placement variables
			int xStart;
			int yStart;
			int remainingShips = 7;
			int x_pos = 4;
			int y_pos = 4;
			
			//Determine who's board is being manipulated
			if(playerNumber == 1){
				xStart = 72;
				yStart = 50;
				player1Ships = remainingShips;
				drawCursorBox(xStart, yStart, x_pos, y_pos);
			}
			if(playerNumber == 2){
				xStart = 72;
				yStart = LCD_HALF_SIZE_Y + 50;
				player2Ships = remainingShips;
				drawCursorBox(xStart, yStart, x_pos, y_pos);
			}
			
			//Allow the player to place ships until all are placed
			while(remainingShips != 0)
			{
				direction = ece210_ps2_read_position();
				drawShipNumber(playerNumber);
					
				switch (direction)
				{
					case PS2_RIGHT:
					{
						if(x_pos < 7)
						{
							x_pos++;
							drawCursorBox(xStart, yStart, x_pos, y_pos);					
						}
						break;
					}
					case PS2_LEFT:
					{
						if(x_pos > 1)
						{
							x_pos--;
							drawCursorBox(xStart, yStart, x_pos, y_pos);																
						}
						break;
					}
					case PS2_DOWN:
					{
						if(y_pos < 7)
						{
							y_pos++;
							drawCursorBox(xStart, yStart, x_pos, y_pos);								
						}
						break;
					}
					case PS2_UP:
					{
						if(y_pos > 1)
						{
							y_pos--;
							drawCursorBox(xStart, yStart, x_pos, y_pos);																
						}
						break;
					}
					case PS2_CENTER:
							break;
					default:
							break;
				}
					
				ece210_wait_mSec(100);
				
				if(AlertButtons)
				{
					AlertButtons = false;
					
					//Confirm the placement of the ship
					if(btn_down_pressed())
					{
						if(playerGrid[x_pos][y_pos] != 1)
						{
							playerGrid[x_pos][y_pos] = 1;
							drawShip(xStart, yStart, x_pos, y_pos);
							remainingShips--;
							if(playerNumber == 1)
								player1Ships = remainingShips;
							if(playerNumber == 2)
								player2Ships = remainingShips;
						}
					}
				}
			}
			player1Ships = 7;
			player2Ships = 7;
		}
		removeHighlightPlayer(playerNumber);
}
void shoot(int playerNumber, int isPlayerAI, int player, int targetBoard[7][7])
	//Allows the player to select grid coordinate to shoot at
{
	//Declare variables
	int xCoordinate;
	int yCoordinate;
	int x_pos= 4;
	int y_pos= 4;
	int xStart;
	int yStart;
	bool selected = false;
	
	//Determine who is manipulating the board
	if(playerNumber == 1){
		xStart = 72;
		yStart = 50;
		drawCursorBox(xStart, yStart, 4, 4);
	}
	if(playerNumber == 2){
		xStart = 72;
		yStart = LCD_HALF_SIZE_Y + 50;
		drawCursorBox(xStart, yStart, 4, 4);
	}
		
	//If player is AI, shoot coordinates are randomly assigned
	if(isPlayerAI == 1)
	{
		do 
		{
		//Selects coordinates if valid
			xCoordinate = (int)(rand()%7)+1;
			yCoordinate = (int)(rand()%7)+1;	
		}
		while(targetBoard[xCoordinate][yCoordinate] == 2 || targetBoard[xCoordinate][yCoordinate] == 3);
		
		while(selected == false)
		{
			ece210_wait_mSec(200);
			
			//AI moves laterally until generated x value matches current x value
			while(xCoordinate != x_pos)
			{
				ece210_wait_mSec(150);
				if(xCoordinate > x_pos)
				{
					x_pos++;
					drawCursorBox(xStart, yStart, x_pos, y_pos);
				}
				else if(xCoordinate < x_pos)
				{
					x_pos--;
					drawCursorBox(xStart, yStart, x_pos, y_pos);
				}
			}
			//AI moves vertically until generated y value matches current y value
			while(yCoordinate != y_pos)
			{
				ece210_wait_mSec(150);
				if(yCoordinate > y_pos)
				{
					y_pos++;
					drawCursorBox(xStart, yStart, x_pos, y_pos);
				}
				else if(yCoordinate < y_pos)
				{
					y_pos--;
					drawCursorBox(xStart, yStart, x_pos, y_pos);
				}
			}
			
			ece210_wait_mSec(200);
			//Confirm the selection and end the loop
			if((xCoordinate == x_pos) && (yCoordinate == y_pos))
					selected = true;
		}
				
		//Adjusts board and game values if there's a hit
		if(targetBoard[xCoordinate][yCoordinate] == 1) 
		{
			drawHit(xStart, yStart, x_pos, y_pos);
			targetBoard[xCoordinate][yCoordinate] = 3;
			if(player == 1)
				player1Ships--;
			if(player == 2)
				player2Ships--;
		}
		else 
		{
			drawMiss(xStart, yStart, x_pos, y_pos);
			targetBoard[xCoordinate][yCoordinate] = 2;
		}
	}
	//Player manually selects a grid coordinate if valid
	else
	{	
		//Allow the player to move until they choose a grid value
		while(selected == false){
			direction = ece210_ps2_read_position();
					
			switch (direction)
			{
				case PS2_RIGHT:
				{
					if(x_pos < 7)
					{
						x_pos++;
						drawCursorBox(xStart, yStart, x_pos, y_pos);					
					}
					break;
				}
				case PS2_LEFT:
				{
					if(x_pos > 1)
					{
						x_pos--;
						drawCursorBox(xStart, yStart, x_pos, y_pos);																
					}
					break;
				}
				case PS2_DOWN:
				{
					if(y_pos < 7)
					{
						y_pos++;
						drawCursorBox(xStart, yStart, x_pos, y_pos);								
					}
					break;
				}
				case PS2_UP:
				{
					if(y_pos > 1)					
					{
						y_pos--;
						drawCursorBox(xStart, yStart, x_pos, y_pos);																
					}
					break;
				}
				case PS2_CENTER:				
					break;
				default:
					break;
			}
					
			ece210_wait_mSec(100);
			
			//Allow the player to press a button to confirm the shot
			if(AlertButtons){
				AlertButtons = false;
				
				if(btn_down_pressed())
				{
					if(targetBoard[x_pos][y_pos] == 1)
					{
						targetBoard[x_pos][y_pos] = 3;
						if(playerNumber == 1){
							player1Ships--;
						}
						if(playerNumber == 2){
							player2Ships--;
						}
						drawHit(xStart, yStart, x_pos, y_pos);
						selected = true;
					}
					if(targetBoard[x_pos][y_pos] == 0)
					{
						targetBoard[x_pos][y_pos] = 2;
						drawMiss(xStart, yStart, x_pos, y_pos);
						selected = true;
					}
				}
			}	
		}
	}
}

void determinePlayer(int player)
	//Displays information in-between game stages
{
	clearBoard();
	ece210_wait_mSec(500);

	if(player == 1)
	{
		ece210_lcd_print_string("Player 1", 150, LCD_HALF_SIZE_Y, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
		if(player2Ships == 0)
		{
			ece210_lcd_print_string("Wins!", 140, LCD_HALF_SIZE_Y + 20, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
		}
		else
			ece210_lcd_print_string("will begin", 158, LCD_HALF_SIZE_Y + 20, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
	}
	if(player == 2)
	{
		ece210_lcd_print_string("Player 2", 150, LCD_HALF_SIZE_Y, LCD_COLOR_WHITE, LCD_COLOR_BLACK);	
		if(player1Ships == 0)
		{
			ece210_lcd_print_string("Wins!", 140, LCD_HALF_SIZE_Y + 20, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
		}
		else
			ece210_lcd_print_string("will begin", 158, LCD_HALF_SIZE_Y + 20, LCD_COLOR_WHITE, LCD_COLOR_BLACK);
	}

	ece210_wait_mSec(3000);
}

void game(int isPlayer1AI, int isPlayer2AI, int player1Board[7][7], int player2Board[7][7])
	//Game loop: repeats player order until all ships are destroyed
{
	//Game variables
	int winner = 0;
	int round = 1;
	int firstPlayer = rand()%2+1;
	bool gameOver = false;
	
	//Show who will begin
	determinePlayer(firstPlayer);

	paint();

	do
	//Repeats game loop until win conditions are met
	{	
		drawShipNumber(1);
		drawShipNumber(2);
		
		//Play in predetermined order
		if(firstPlayer == 1) 
		{
			highlightPlayer(1);
			shoot(2, isPlayer1AI, player2, player2Board);
			removeHighlightPlayer(1);
			
			highlightPlayer(2);
			shoot(1, isPlayer2AI, player1, player1Board);
			removeHighlightPlayer(2);
		}
		if(firstPlayer == 2) 
		{
			highlightPlayer(2);
			shoot(1, isPlayer2AI, player1, player1Board);
			removeHighlightPlayer(2);

			highlightPlayer(1);
			shoot(2, isPlayer1AI, player2, player2Board);
			removeHighlightPlayer(1);
		}
			
		//Checks the number of remaining ships and assigns the winner
		if(player1Ships == 0){
			winner = 2;
			determinePlayer(2);
			gameOver = true;
		}
		if(player2Ships == 0){
			winner = 1;
			determinePlayer(1);
			gameOver = true;
		}
		
		//Evalue end game conditions
		if(winner == 1 || winner == 2) 
		{
			gameOver = true;
		}		
		else if(round >= 49) {
			gameOver = true;
			clearBoard();

			ece210_lcd_add_msg("No more rounds! No winner",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
			ece210_lcd_add_msg("\n\n\n\n\n",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLACK);
			ece210_lcd_add_msg("\n\n\n\n\n",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLACK);
			ece210_lcd_add_msg("\n\n\n\n\n",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLACK);
			ece210_lcd_add_msg("\n\n\n\n\n",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLACK);
			ece210_lcd_add_msg("\n\n\n\n\n",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLACK);
			ece210_lcd_add_msg("\n\n\n\n\n",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLACK);		
			ece210_lcd_add_msg("\n\n\n\n\n",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLACK);		
		}
		else
			gameOver = false;
			
		round++;		
	}
	while(gameOver == false);
	
	ece210_wait_mSec(5000);
}

void pressToContinue()
	//Creates a 'press to continue' wait check
{
	ece210_lcd_add_msg("Press any button to continue...", TERMINAL_ALIGN_LEFT, LCD_COLOR_YELLOW);

	bool selected = false;
	
	while(selected == false)
	{
		ece210_wait_mSec(100);
		
		direction = ece210_ps2_read_position();
		
		switch(direction)
		{
			case PS2_UP:
			{
				selected = true;
				break;
			}
			case PS2_DOWN:
			{
				selected = true;
				break;
			}
			case PS2_LEFT:
			{
				selected = true;
				break;
			}
			case PS2_RIGHT:
			{
				selected = true;
				break;
			}			
			case PS2_CENTER:
				break;
			default:
				break;
		}
		
		if(AlertButtons)
		{
			AlertButtons = false;
			if(btn_up_pressed())
				selected = true;			
			if(btn_down_pressed())
				selected = true;
			if(btn_left_pressed())			
				selected = true;
			if(btn_right_pressed())
				selected = true;
		}
	}
}

void howToPlay()
	//Goes through brief text tutorial on game rules and how to play
{	
	ece210_lcd_add_msg("The object of the game is to eliminate all of your opponents ships.", TERMINAL_ALIGN_LEFT, LCD_COLOR_WHITE);
	ece210_lcd_add_msg("Each player will alternate shooting on a grid until one player has no ships remaining.", TERMINAL_ALIGN_LEFT, LCD_COLOR_WHITE);
	pressToContinue();
	ece210_lcd_add_msg("You will begin, by selecting where to place your ships. You have 7 ships total. Place them wisely.", TERMINAL_ALIGN_LEFT, LCD_COLOR_WHITE);
	ece210_lcd_add_msg("After each player places their ships, the game will begin.", TERMINAL_ALIGN_LEFT, LCD_COLOR_WHITE);
	pressToContinue();
	ece210_lcd_add_msg("The player whose turn it is, will be highlighted by a red square.", TERMINAL_ALIGN_LEFT, LCD_COLOR_WHITE);
	ece210_lcd_add_msg("You will select a point using the joystick. Use the bottom button to confirm your shot.", TERMINAL_ALIGN_LEFT, LCD_COLOR_WHITE);
	pressToContinue();
	ece210_lcd_add_msg("There will be a counter beneath your name, indicating the number of your enemies ships.", TERMINAL_ALIGN_LEFT, LCD_COLOR_WHITE);
	ece210_lcd_add_msg("When one player has no ships remaining, the game is over", TERMINAL_ALIGN_LEFT, LCD_COLOR_WHITE);
	pressToContinue();
		
	//Revert back to menu screen
	for(int a = 0; a < 10; a++)
	{
		ece210_lcd_add_msg("",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLACK);
	}

	ece210_lcd_add_msg("Main Menu", TERMINAL_ALIGN_CENTER, LCD_COLOR_WHITE);
	ece210_lcd_add_msg("--------------------", TERMINAL_ALIGN_CENTER, LCD_COLOR_WHITE);
	ece210_wait_mSec(150);
	ece210_lcd_add_msg("UP: 1-Player", TERMINAL_ALIGN_LEFT, LCD_COLOR_BLUE);
	ece210_wait_mSec(150);
	ece210_lcd_add_msg("LEFT: 2-Players", TERMINAL_ALIGN_LEFT, LCD_COLOR_RED);
	ece210_wait_mSec(150);
	ece210_lcd_add_msg("RIGHT: AI only", TERMINAL_ALIGN_LEFT, LCD_COLOR_YELLOW);
	ece210_wait_mSec(150);
	ece210_lcd_add_msg("DOWN: How to play", TERMINAL_ALIGN_LEFT, LCD_COLOR_GREEN);
}

void mainMenu(int isPlayer1AI, int isPlayer2AI)
	//Main menu: Allows player to initialize game settings and begin the game
{	
		ece210_lcd_add_msg("BUCKYSHIP", TERMINAL_ALIGN_CENTER, LCD_COLOR_BLUE);

		skipLines(5);
	
		//Wait for player to be ready
		pressToContinue();
	
		skipLines(10);
	
		//Print information
		ece210_lcd_add_msg("Main Menu", TERMINAL_ALIGN_CENTER, LCD_COLOR_WHITE);
		ece210_lcd_add_msg("--------------------", TERMINAL_ALIGN_CENTER, LCD_COLOR_WHITE);
		ece210_lcd_add_msg("UP:    1-Player", TERMINAL_ALIGN_LEFT, LCD_COLOR_BLUE);
		ece210_lcd_add_msg("LEFT:  2-Players", TERMINAL_ALIGN_LEFT, LCD_COLOR_RED);
		ece210_lcd_add_msg("RIGHT: AI only", TERMINAL_ALIGN_LEFT, LCD_COLOR_YELLOW);
		ece210_lcd_add_msg("DOWN:  How to play", TERMINAL_ALIGN_LEFT, LCD_COLOR_GREEN);
		ece210_lcd_add_msg("Use the joystick to select", TERMINAL_ALIGN_LEFT, LCD_COLOR_CYAN);
		ece210_lcd_add_msg("--------------------", TERMINAL_ALIGN_CENTER, LCD_COLOR_WHITE);
	
		bool selection = false;
	
		while(selection == false)
		{
			direction = ece210_ps2_read_position();
			
			
			
			//Generate random seed while waiting for input
			randSeed += 1;
			if(randSeed == 32768)
				randSeed = -32768;

			switch (direction)
			{
				case PS2_RIGHT:
				{
						isPlayer1AI = 1;
						isPlayer2AI = 1;
						ece210_lcd_add_msg("AI vs AI",TERMINAL_ALIGN_CENTER,LCD_COLOR_YELLOW);					
						selection = true;
						break;
				}
				case PS2_LEFT:
				{
					isPlayer1AI = 0;
					isPlayer2AI = 0;
					ece210_lcd_add_msg("Player1 vs Player2",TERMINAL_ALIGN_CENTER,LCD_COLOR_RED);
					selection = true;
					break;
				}
				case PS2_DOWN:
				{
					ece210_wait_mSec(100);
					howToPlay();
					ece210_wait_mSec(100);
					break;
				}
				case PS2_UP:
				{
					isPlayer1AI = 0;
					isPlayer2AI = 1;
					ece210_lcd_add_msg("Player1 vs AI",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
					selection = true;				
					break;
				}
				case PS2_CENTER:				
					break;
				default:
					break;
			}
			
			
			if(AlertButtons)
			{
				AlertButtons = false;
				
				if(btn_up_pressed())
				{
					isPlayer1AI = 0;
					isPlayer2AI = 1;
					ece210_lcd_add_msg("Player1 vs AI",TERMINAL_ALIGN_CENTER,LCD_COLOR_BLUE);
					selection = true;					
				}
				if(btn_down_pressed())
				{
					ece210_wait_mSec(100);
					howToPlay();
					ece210_wait_mSec(100);
				}
				if(btn_left_pressed())
				{
					isPlayer1AI = 0;
					isPlayer2AI = 0;
					ece210_lcd_add_msg("Player1 vs Player2",TERMINAL_ALIGN_CENTER,LCD_COLOR_RED);
					selection = true;
				}				
				if(btn_right_pressed())
				{
						isPlayer1AI = 1;
						isPlayer2AI = 1;
						ece210_lcd_add_msg("AI vs AI",TERMINAL_ALIGN_CENTER,LCD_COLOR_YELLOW);					
						selection = true;
				}			
			}
		}
		
		//Wait for player to be ready
		pressToContinue();

		//Set the seed
		srand(randSeed);
		ece210_wait_mSec(3000);
	
		//Reset ships if another game is being played
		player1Ships = 7;
		player2Ships = 7;
		
		//Define the player board data
		initializeBoard(player1Grid, player2Grid);
		
		//Create the player boards
		playerBoardSetup(1, isPlayer1AI, player1Grid);
		playerBoardSetup(2, isPlayer2AI, player2Grid);
		
		//Initiate the game
		game(isPlayer1AI, isPlayer2AI, player1Grid, player2Grid);	
}

int main(void)
	// MAIN PROGRAM METHOD
{ 	
	//Declares player AI
	int isPlayer1AI = 0;
	int isPlayer2AI = 0;
	
	//Declare end game determinant
	bool endGame = false;
	
	ece210_initialize_board();
	
	//Repeat game until player ends it
	while(endGame == false)
	{
		//Start the menu
		mainMenu(isPlayer1AI, isPlayer2AI);
		
		bool decided = false;
		
		//After a game, offer to start again
		ece210_lcd_add_msg("Would you like to play again? LEFT: YES; RIGHT: NO", TERMINAL_ALIGN_LEFT, LCD_COLOR_RED);
	
		while(decided == false)
		{
			direction = ece210_ps2_read_position();
					
			switch (direction)
			{
				case PS2_RIGHT: // Joystick pointing left.
				{
					ece210_lcd_add_msg("Thanks for playing!", TERMINAL_ALIGN_LEFT, LCD_COLOR_BLUE2);
					endGame = true;
					decided = true;
					skipLines(20);
					break;
				}
				case PS2_LEFT:
				{
					ece210_lcd_add_msg("Another game!", TERMINAL_ALIGN_LEFT, LCD_COLOR_BLUE2);
					endGame = false;
					decided = true;
					break;
				}
				case PS2_DOWN:
				{
					break;
				}
				case PS2_UP:
				{
					break;
				}
				case PS2_CENTER:					
					break;
				default:
					break;			
			}
		}	
	}
	
//	 TESTING
//		paint();
//	playerBoardSetup(1, false, player1Grid);
//	playerBoardSetup(2, false, player2Grid);
//	game(isPlayer1AI, isPlayer2AI, player1Grid, player2Grid);
}
