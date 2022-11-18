#include "PC_FileIO.c"
#include "EV3Servo-lib-UW.c"

const int BOARD_SIZE = 8;
const int BOARD_DIMENSION = 5.5; //cm

// 2d array with the board location
// bk, wk, k, q, b, r, p, n for knight
string board[BOARD_SIZE][BOARD_SIZE];

// HUGE ASSUMPTION:
// on startup, the y-axis (pulley) MUST be at the same point (probably zero)
// otherwise we have to change the init function, I don't think we'll have a touch sensor there to 'zero'

const int TOUCH = S1;
const int COLOR = S2;
const int XZEROTOUCH = S3;
const int RED = colorRed;

const int XMOTOR = motorA;
const int YMOTOR = motorB;
const int XMOTORPOWER = 50;
const int YMOTORPOWER = 50;

const int CLAWACTUATIONMOTOR = motorC;
const int CLAWMOTOR = S1;
const int CLAWCLOSE = 10;
const int CLAWOPEN = 70;
const int CLAWWAITTIME = 100;
const int CLAWLOWERCLICKS = 360;
const int SV_GRIPPER = 4;

// where the taken peices go
const int ENDX = 9;
const int ENDY = 9;

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
void getInput(int &currentLetter, int &currentNumber, int &moveToLetter, int &moveToNumber)
{
	// ascii
	currentLetter = 65;
	currentNumber = 1;
	moveToLetter = 65;
	moveToNumber = 1;

	getCellInput(currentLetter, currentNumber, true);
	getCellInput(currentLetter, currentNumber, false);

	wait1Msec(500);
	eraseDisplay();
}

// zeroing function
void zero()
{
	motor[XMOTOR] = -10;
	while (!SensorValue[XZEROTOUCH])
	{	}
	motor[XMOTOR] = 0;

	// assuming (-) is 'backwards' towards the 'zero point'
	motor[YMOTOR] = -10;
	while (nMotorEncoder[YMOTOR] > 0)
	{ }
	motor[YMOTOR] = 0;
}

// move to cell
void moveToCell(int currX, int currY, int x, int y)
{
	int travelX = x - currX;
	int travelY = y - currY;
	int directionX = -1;
	int directionY = 1;

	if (travelX < 0)
		directionX *= -1;

	if (travelY < 0)
		directionY *= -1;

	for (int count = 0; count < travelX; count++)
	{
		motor[XMOTOR] = XMOTORPOWER * directionX;
		while(SensorValue(COLOR) != RED)
		{ }
	}

	for (int count = 0; count < travelY; count++)
	{
		motor[YMOTOR] = YMOTORPOWER * directionY;
		while(SensorValue(COLOR) != RED)
		{ }
	}
}

//Function for when there is a piece that needs to be taken, etc..

// file input

// file output

// replay the game

// picking up the peice when the claw is in place
void pickUpPiece()
{
	nMotorEncoder[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWACTUATIONMOTOR] = 10;
	while(abs(nMotorEncoder[CLAWACTUATIONMOTOR]) < CLAWLOWERCLICKS)
	{ }
	motor[CLAWACTUATIONMOTOR] = 0;
	setGripperPosition(CLAWMOTOR,SV_GRIPPER,CLAWCLOSE);
	wait1Msec(CLAWWAITTIME);
	nMotorEncoder[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWACTUATIONMOTOR] = -10;
	while(abs(nMotorEncoder[CLAWACTUATIONMOTOR]) < CLAWLOWERCLICKS)
	{ }
	motor[CLAWACTUATIONMOTOR] = 0;
}

// putting down the peice when the claw is in place
void putDownPiece()
{
	nMotorEncoder[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWACTUATIONMOTOR] = 10;
	while(abs(nMotorEncoder[CLAWACTUATIONMOTOR]) < CLAWLOWERCLICKS)
	{ }
	motor[CLAWACTUATIONMOTOR] = 0;
	setGripperPosition(CLAWMOTOR, SV_GRIPPER, CLAWOPEN);
	wait1Msec(CLAWWAITTIME);
	nMotorEncoder[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWACTUATIONMOTOR] = -10;
	while(abs(nMotorEncoder[CLAWACTUATIONMOTOR]) < CLAWLOWERCLICKS)
	{ }
	motor[CLAWACTUATIONMOTOR] = 0;
}


// execute move fucntion
bool movePiece(int x1, int y1, int x2, int y2)
{
	// if endpos same as start pos, or color already obtains the destination cell, move is invalid
	if ((x1 == x2 && y1 == y2) || (stringFind(board[x2][y2], "W") == stringFind(board[x1][y1], "W")))
	{
		return false;
	}
	// if the board has another piece here
	if (board[x2][y2] != "")
	{
		moveToCell(0,0,x2,y2);
		pickUpPiece();
		moveToCell(x2,y2,ENDX,ENDY);
		putDownPiece();
		moveToCell(ENDX, ENDY, x1, y1);
	}
	else
	{
		moveToCell(0,0,x1,y1);
	}
	pickUpPiece();
	moveToCell(x1,y1,x2,y2);
	putDownPiece();
	// have to check for legal move here
	board[x2][y2] = board[x1][y1];
	board[x1][y1] = "";
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
void shutDownProcedure()
{

}

// main function
task main()
{
	configureSensors();
	boardInitState();

	// UI

	bool playing = false; // CHANGE THIS LINE
	bool reviewGame = false;

	while(playing)
	{
		displayBigTextLine(1,"Game Mode");
		displayString(3,"Toggle with UP/DOWN");
		if(reviewGame)
			displayBigTextLine(5, "Review Game");
		else
			displayBigTextLine(5, "Play Game");

		if(getButtonPress(buttonUp) || getButtonPress(buttonDown))
		{
			reviewGame = !reviewGame;
			while(getButtonPress(buttonUp) || getButtonPress(buttonDown))
			{ }
		}

		if(getButtonPress(buttonEnter))
			playing = false;
			while(getButtonPress(buttonEnter))
			{}
	}

	eraseDisplay();

	if (reviewGame)
	{
		// Open FIle
		// TFileHandle FileIn;
		// openReadPC (FileIn, "*FileName*");

		// Get File Length
		// int length = getFileLength (FileIn);
		// closeFilePC (FileIn);
		// openReadPC (FileIn, "*FileName*");

		// Make arrays
		// string initialColumn[length];
		// int initialRow[length];
		// string finalColumn[length];
		// int finalRow[length];

		// get file input

		// Close File closeFilePC (FileIn);
	}
	else
	{
		playing = true;
		bool whiteTurn = true;
		while(playing)
		{
			int currentLetter, currentNumber, moveToLetter, moveToNumber;
			getInput(currentLetter, currentNumber, moveToLetter, moveToNumber);
			playing = false;
		}
		// keep track of turn (probably boolean is fine)
		// move to position
		// pick up
		// move to position
		// place
		// move to zero?
		// back to top of while loop

	}
}




// functions for fileio

// function to get input
// returns current letter and number by pbr
/*
int getFileLength (TFileHandle &FileIn)
{
	int counter = 0;
	char input = '';
	int inputNum = 0;
	bool check = true;
	do
	{
		check = readCharPC(FileIn, input);
		bool check1 = readIntPC (FileIn, inputNum);
		bool check2 = readCharPC(FileIn, input);
		bool check3 = readIntPC (FileIn, inputNum);
		counter++;
	}while(check);

	return counter;
}

void getFileInput (TFileHandle &FileIn, string initialColumn[], int initialRow[], string finalColumn[], int finalRow[])
{

	int counter = 0;
	while(readTextPC(FileIn, initialColumn[counter])
				readIntPC (FileIn, initialRow[counter])
				readTextPC(FileIn, finalCoulmn[counter])
				readIntPC (FileIn, finalRow[counter]))
	{
		counter++;
	}
}
*/
