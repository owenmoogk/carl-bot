#include "EV3Servo-lib-UW.c"

const int BOARD_SIZE = 8;

// 2d array with the board location
// bk, wk, k, q, b, r, p, n for knight
string board[BOARD_SIZE][BOARD_SIZE];

// HUGE ASSUMPTION:
// on startup, the y-axis (pulley) MUST be at the same point (probably zero)
// otherwise we have to change the init function, I don't think we'll have a touch sensor there to 'zero'

const int TOUCH = S4;
const int COLOR = S3;
const int XZEROTOUCH = S2;
const int RED = colorRed;

const int XMOTOR = motorD;
const int YMOTOR = motorA;
const int XMOTORPOWER = 20;
const int YMOTORPOWER = 100;
const int YCELLCLICKS = 1180;

const int CLAWACTUATIONMOTOR = motorB;
const int CLAWMOTOR = S1;
const int CLAWCLOSE = 10;
const int CLAWOPEN = 70;
const int CLAWWAITTIME = 500;
const int CLAWLOWERCLICKS = 280;
const int CLAWLOWERCLICKSTALL = 185;
const int SV_GRIPPER = 4;

// where the taken peices go
const int ENDX = 7;
const int ENDY = 0;

// sensor configuration
void configureSensors()
{
	SensorType[TOUCH] = sensorEV3_Touch;
	wait1Msec(50);
	SensorType[COLOR] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[COLOR] = modeEV3Color_Color;
	wait1Msec(50);
	nMotorEncoder[YMOTOR] = 0;
}


void getCellInput(int &currentLetter, int &currentNumber, bool current)
{
	// ascii
	currentLetter = 65;
	currentNumber = 1;
	while (true)
	{

		// will have to test if this ascii casting works
		if (current)
			displayBigTextLine(2, "Current: %c%d", currentLetter, currentNumber);
		else
			displayBigTextLine(4, "Final: %c%d", currentLetter, currentNumber);

		while (!getButtonPress(buttonAny))
		{ }

		if (getButtonPress(buttonEnter))
		{
			while(getButtonPress(buttonEnter))
			{ }
			return;
		}

		if (getButtonPress(buttonLeft))
			currentLetter -= 1;
		if (getButtonPress(buttonRight))
			currentLetter += 1;
		if (getButtonPress(buttonUp))
			currentNumber += 1;
		if (getButtonPress(buttonDown))
			currentNumber -= 1;
		while (getButtonPress(buttonAny))
		{ }

		// wrapping around the board
		// ie, if you start at 1 and hit back, it will go to position 8
		if (currentLetter < 65)
			currentLetter = 72;
		if (currentLetter > 72)
			currentLetter = 65;
		if (currentNumber < 1)
			currentNumber = 8;
		if (currentNumber > 8)
			currentNumber = 1;
	}
}

// getting the user input
bool getInput(int &currentLetter, int &currentNumber, int &moveToLetter, int &moveToNumber)
{
	displayBigTextLine(1,"Continue?");
	bool exit = false;
	bool doContinue = true;
	while(!exit)
	{
		if (getButtonPress(buttonEnter))
			exit = true;
		else if (getButtonPress(buttonRight) || getButtonPress(buttonLeft))
		{
			doContinue = !doContinue;
			while(getButtonPress(buttonRight) || getButtonPress(buttonLeft))
			{ }
		}
		if (doContinue)
			displayBigTextLine(3,"Yes");
		else
			displayBigTextLine(3,"No");
	}
	while(getButtonPress(buttonEnter))
	{ }
	eraseDisplay();
	if (!doContinue)
		return false;

	// ascii
	currentLetter = 65;
	currentNumber = 1;
	moveToLetter = 65;
	moveToNumber = 1;

	getCellInput(currentLetter, currentNumber, true);
	getCellInput(moveToLetter, moveToNumber, false);

	currentLetter -= 65;
	moveToLetter -= 65;
	currentNumber -= 1;
	moveToNumber -= 1;

	currentLetter = 7-currentLetter;
	moveToLetter = 7-moveToLetter;

	wait1Msec(500);
	eraseDisplay();
	return true;
}

// zeroing function
void zero()
{
	motor[XMOTOR] = -30;
	while (!SensorValue[XZEROTOUCH])
	{	}
	motor[XMOTOR] = 0;

	// assuming (+) is 'backwards' towards the 'zero point'
	motor[YMOTOR] = 100;
	while (nMotorEncoder[YMOTOR] < 0)
	{ }
	motor[YMOTOR] = 0;

	setGripperPosition(CLAWMOTOR,SV_GRIPPER,CLAWOPEN);
}

// move to cell
void moveToCell(int currX, int currY, int x, int y)
{
	int travelX = currX - x;
	int travelY = currY - y;
	int directionX = -1;
	int directionY = 1;

	if (travelX < 0)
	{
		directionX *= -1;
	}

	if (travelY < 0)
		directionY *= -1;

	if (travelX != 0)
	{
		for (int count = 0; count < abs(travelX) + 1; count++)
		{
			motor[XMOTOR] = XMOTORPOWER * directionX;
			wait1Msec(50);
			while(SensorValue(COLOR) != RED)
			{ }
			if (count != abs(travelX))
			{
				while(SensorValue(COLOR) == RED)
				{ }
			}
		}
		// this is tuned for color sensor positioning
		wait1Msec(150);
		motor[XMOTOR] = 0;
	}

	motor[YMOTOR] = -100;
	wait1Msec(110);

	motor[YMOTOR] = YMOTORPOWER * directionY;
	if (directionY == 1)
	{
		while(abs(nMotorEncoder(YMOTOR)) > abs(YCELLCLICKS * y) && nMotorEncoder(YMOTOR) < 0)
		{ }
	}
	if (directionY == -1)
	{
		while(abs(nMotorEncoder(YMOTOR)) < abs(YCELLCLICKS * y) && nMotorEncoder(YMOTOR) < 0)
		{ }
	}

	motor[YMOTOR] = 0;
}

//Function for when there is a piece that needs to be taken, etc..

// file input

// file output

// replay the game

// picking up the peice when the claw is in place
void pickUpPiece(int x2, int y2)
{
	int lowerDistance = CLAWLOWERCLICKS;
	if (stringFind(board[y2][x2], "K") != -1 || stringFind(board[y2][x2], "Q") != -1)
		lowerDistance = CLAWLOWERCLICKSTALL;
	setGripperPosition(CLAWMOTOR,SV_GRIPPER,CLAWOPEN);
	nMotorEncoder[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWACTUATIONMOTOR] = -10;
	while(abs(nMotorEncoder[CLAWACTUATIONMOTOR]) < lowerDistance)
	{ }
	motor[CLAWACTUATIONMOTOR] = 0;
	setGripperPosition(CLAWMOTOR,SV_GRIPPER,CLAWCLOSE);
	wait1Msec(CLAWWAITTIME);
	nMotorEncoder[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWACTUATIONMOTOR] = 10;
	while(abs(nMotorEncoder[CLAWACTUATIONMOTOR]) < lowerDistance)
	{ }
	motor[CLAWACTUATIONMOTOR] = 0;
}

// putting down the peice when the claw is in place
void putDownPiece(int x2, int y2)
{
	int lowerDistance = CLAWLOWERCLICKS;
	if (stringFind(board[y2][x2], "K") != -1 || stringFind(board[y2][x2], "Q") != -1)
		lowerDistance = CLAWLOWERCLICKSTALL;
	nMotorEncoder[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWACTUATIONMOTOR] = -10;
	while(abs(nMotorEncoder[CLAWACTUATIONMOTOR]) < lowerDistance-10)
	{ }
	motor[CLAWACTUATIONMOTOR] = 0;
	wait1Msec(CLAWWAITTIME);
	setGripperPosition(CLAWMOTOR, SV_GRIPPER, CLAWOPEN);
	wait1Msec(CLAWWAITTIME);
	nMotorEncoder[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWACTUATIONMOTOR] = 10;
	while(abs(nMotorEncoder[CLAWACTUATIONMOTOR]) < lowerDistance-10)
	{ }
	motor[CLAWACTUATIONMOTOR] = 0;
}

void capturePiece(int x2, int y2)
{
		moveToCell(0,0,x2,y2);
		pickUpPiece(x2,y2);
		moveToCell(x2,y2,ENDX,ENDY);
		motor[XMOTOR] = 30;
		wait1Msec(900);
		motor[XMOTOR] = 0;
		putDownPiece(x2,y2);
		return;
}

// execute move fucntion
bool movePiece(int x1, int y1, int x2, int y2)
{
	// if endpos same as start pos, or color already obtains the destination cell, move is invalid
//|| (stringFind(board[y2][x2], "W") == stringFind(board[y1][x1], "W"))
// if your black and the other is nothing then oop, not a valid move
// whoohoo figured it out
	if ((x1 == x2 && y1 == y2) )
	{
		displayBigTextLine(1, "Invalid Move");
		displayBigTextLine(3, "Try again");
		wait1Msec(3000);
		return false;
	}
	// if the board has another piece here

	if (board[y2][x2] != "")
	{
		capturePiece(x2,y2);
		zero();
	}
	//displayBigTextLine(1, "X1: %d", x1);
	//displayBigTextLine(3, "Y1: %d", y1);
	//displayBigTextLine(5, "X2: %d", x2);
	//displayBigTextLine(7, "Y2: %d", y2);
	moveToCell(0,0,x1,y1);
	pickUpPiece(x1,y1);
	moveToCell(x1,y1,x2,y2);
	putDownPiece(x1,y1);

	board[y2][x2] = board[y1][x1];
	board[y1][x1] = "";
	zero();

	return true;
}

void boardInitState()
{
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {

      string value = "";

      // black or white
      if (row <= 1)
        value = "W";
      else if (row >= 6)
        value = "B";

      if (row == 1 || row == 6)
        value = value + "P";

      else if (row == 7 || row == 0)
        if (col == 0 || col == 7)
          value = value + "R";
        else if (col == 1 || col == 6)
          value = value + "N";
        else if (col == 2 || col == 5)
          value = value + "B";
        else if (col == 3)
          value = value + "Q";
        else if (col == 4)
          value = value + "K";

      board[row][col] = value;
    }
  }
}

// when the user wants to shut down
void shutDownProcedure(bool whiteLoses, int endCode)
{
	if (whiteLoses)
		displayBigTextLine(2,"Black Wins!");
	else
		displayBigTextLine(2,"White Wins!");

	if (endCode == 0)
		displayBigTextLine(4,"By Resignation");
	if (endCode == 1)
		displayBigTextLine(4, "On Time");
	zero();
	wait1Msec(5000);
}

// main function
task main()
{
	configureSensors();
	boardInitState();

	int whiteTime = 300;
	int blackTime = 300;

	bool whiteTurn = true;
	bool playing = true;
	while(playing)
	{
		clearTimer(T1);
		int x1,y1,x2,y2;
		if (whiteTurn)
			displayBigTextLine(7,"Time Left:%d:%d", whiteTime / 60, whiteTime % 60);
		else
			displayBigTextLine(7,"Time Left:%d:%d", blackTime / 60, blackTime % 60);

		playing = getInput(x1,y1,x2,y2);
		if (whiteTurn)
			whiteTime -= time1[T1] /1000;
		else
			blackTime -= time1[T1] /1000;

		if (whiteTime == 0 || blackTime == 0)
		{
			playing = false;
			shutDownProcedure(whiteTurn, 1);
		}

		if (!playing)
			shutDownProcedure(whiteTurn, 0);

		if (playing)
		{
			zero();
			movePiece(x1,y1,x2,y2);
			zero();
			whiteTurn = !whiteTurn;
		}
	}

}
