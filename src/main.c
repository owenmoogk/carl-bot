const int BOARD_SIZE = 8;

// 2d array with the board location
// bk, wk, k, q, b, r, p, n for knight
string board[BOARD_SIZE][BOARD_SIZE];

const int TOUCH = S1;
const int COLOR = S2;
const int XZEROTOUCH = S3;

const int XMOTOR = motorA;
const int YMOTOR = motorB;
const int CLAWACTUATIONMOTOR = motorC;
const int CLAWMOTOR = motorD;

const int CLAWLOWERCLICKS = 100;
// time to wait until the claw has fully secured the peice
const int CLAWWAITTIME = 100; // ms

// sensor configuration
void configureSensors()
{
	SensorType[TOUCH] = sensorEV3_Touch;
	SensorType[COLOR] = sensorEV3_Color;
	wait1Msec(50);
	SensorMode[COLOR] = modeEV3Color_Color;
	wait1Msec(50);
	nMotorEncoder[YMOTOR] = 0;
}

// function to get input
// returns current letter and number by pbr
void getInput(int &currentLetter, int &currentNumber)
{
	// ascii
	currentLetter = 65;
	currentNumber = 1;
	while (true)
	{
		while (!getButtonPress(buttonAny))
		{ }

		if (getButtonPress(buttonEnter)) return;

		if (getButtonPress(buttonLeft)) currentLetter -= 1;
		if (getButtonPress(buttonRight)) currentLetter += 1;
		if (getButtonPress(buttonUp)) currentNumber += 1;
		if (getButtonPress(buttonDown)) currentNumber -= 1;
		while (getButtonPress(buttonAny))
		{ }

		// wrapping around the board
		// ie, if you start at 1 and hit back, it will go to position 8
		if (currentLetter < 65) currentLetter = 72;
		if (currentLetter > 72) currentLetter = 65;
		if (currentNumber < 1) currentNumber = 8;
		if (currentNumber > 8) currentNumber = 1;

		// will have to test if this ascii casting works
		displayString(2, "Current Position: %c%i", currentLetter, currentNumber);
	}
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
	// calculate distance to cell
	// move x axis
	// move z axis

// file input

// file output

// replay the game

// picking up the peice when the claw is in place
void pickUpPeice()
{
	nMotorEncoder[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWACTUATIONMOTOR] = 10;
	while(abs(nMotorEncoder[CLAWACTUATIONMOTOR]) < CLAWLOWERCLICKS)
	{ }
	motor[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWMOTOR] = 10;
	wait1Msec(CLAWWAITTIME * 1.5);
	// note, the motor is being left on to make sure the peice is always secured in the claw
	// this can be fine tuned later
	nMotorEncoder[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWACTUATIONMOTOR] = -10;
	while(abs(nMotorEncoder[CLAWACTUATIONMOTOR]) < CLAWLOWERCLICKS)
	{ }
}

// putting down the peice when the claw is in place
void putDownPeice()
{
	nMotorEncoder[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWACTUATIONMOTOR] = 10;
	while(abs(nMotorEncoder[CLAWACTUATIONMOTOR]) < CLAWLOWERCLICKS)
	{ }
	motor[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWMOTOR] = -10;
	wait1Msec(CLAWWAITTIME);
	motor[CLAWMOTOR] = 0;
	nMotorEncoder[CLAWACTUATIONMOTOR] = 0;
	motor[CLAWACTUATIONMOTOR] = -10;
	while(abs(nMotorEncoder[CLAWACTUATIONMOTOR]) < CLAWLOWERCLICKS)
	{ }
}
// execute move fucntion
void movePeice(int x1, int y1, int x2, int y2)
{

}
// moves to cell
// pick up peice
// place peice
// update game board

void boardInitState()
{
  for (int row = 0; row < BOARD_SIZE; row++)
  {
    for (int col = 0; col < BOARD_SIZE; col++)
    {

      string value = "";

      // black or white
      if (row <= 1)
      {
        value = "W";
      }
      else if (row >= 6)
      {
        value = "B";
      }

      if (row == 1 || row == 6)
      {
        value = value + "P";
      }
      else if (row == 7 || row == 0)
      {
        if (col == 0 || col == 7)
        {
          value = value + "R";
        }
        else if (col == 1 || col == 6)
        {
          value = value + "N";
        }
        else if (col == 2 || col == 5)
        {
          value = value + "B";
        }
        else if (col == 3)
        {
          value = value + "Q";
        }
        else if (col == 4)
        {
          value = value + "K";
        }
      }

      board[row][col] = value;
    }
  }
}

task main()
{
	configureSensors();
	boardInitState();

	// while loop
	// keep track of turn (probably boolean is fine)
	// prompt for move
	// move to position
	// pick up
	// move to position
	// place
	// move to zero?
	// back to top of while loop
	// exit when the user says so..? don't think we can check for checkmates soooo.....
}
