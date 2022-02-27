#include "vex.h"

using namespace vex;
#include "bot-functions.h"

using namespace vex;

// A global instance of competition
competition Competition;
int MenuItemHighlighted = 1;
int MenuItemSelected = 0;

int PreLoadBallMessage = 0;  // If 1, tells PrepLimShootTask to preload ball
int BallIntakeCounter = 0;
int resetLaunchStatusMessage = 0;

// //---------------------------------------------------------------------
enum Sides{
  RED, BLUE
};

enum FirstGoal{
  LEFT, RIGHT
};

enum MenuItems{
  ITEM_NOT_USED,
  NO_ACTION,
  AUTO_SKILLS_1,
  GOAL_LEFT_CYCLE_ADV,
  GOAL_LEFT_123_CYCLE1,
  GOAL_LEFT_123_CYCLE12,
  GOAL_LEFT_1,
  GOAL_RIGHT_123_CYCLE12,
  GOAL_RIGHT_1,
   GOAL_MID_1,
  MENU_LENGTH,
};

char MenuList[MENU_LENGTH][20] = {
  "",                   //0 ITEM_NOT_USED
  "No Action",           //7 NO_ACTION
  "Auto Skills 1",
  "Goal L Advanced",
  "Goal L 123 Cycle1",
  "Goal L 123 Cycle12",
  "Goal L 1",
  "Goal R 123 Cycle12",
  "Goal R 1",
  "Goal M 1",
};



// //Filter function for driving ---------
int filter (int value) {
  if (abs(value) > 16) {
    return (value);
  } else {
    return (0);
  }
}


void MenuDown(void){
  MenuItemHighlighted++;
  if(MenuItemHighlighted > (MENU_LENGTH - 1)){
    MenuItemHighlighted = 1;
  }
}

void MenuSelect(void){
  MenuItemSelected = 1;
}



/*---------------------------------------------------------------------------*/
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*  You may want to perform some actions before the competition starts.      */
/*  Do them in the following function.  You must return from this function   */
/*  or the autonomous and usercontrol tasks will not be started.  This       */
/*  function is only called once after the V5 has been powered on and        */
/*  not every time that the robot is disabled.                               */
/*---------------------------------------------------------------------------*/

void pre_auton(void) {

  int limitLoop = 0;

  // Initializing Robot Configuration. DO NOT REMOVE!
  vexcodeInit();

  //menu controls
  Brain.Screen.setFont(fontType::mono20);

  mainControl.ButtonDown.pressed(MenuDown);
  mainControl.ButtonRight.pressed(MenuSelect);
  while (!MenuItemSelected){
      mainControl.Screen.clearScreen();
      mainControl.Screen.setCursor(1, 1);
      mainControl.Screen.print("%s", MenuList[MenuItemHighlighted]);

      mainControl.Screen.setCursor(2, 1);
      mainControl.Screen.print("Hi");
      vex::task::sleep(100);
  }

  mainControl.Screen.setCursor(2, 1);
  mainControl.Screen.print("%s selected", MenuList[MenuItemHighlighted]);

  // drive buttons

#if 1
  secondCtrl.ButtonR1.pressed( intakeToggleDriver );
  secondCtrl.ButtonR2.pressed( outtake );
  secondCtrl.ButtonL1.pressed( rollerUpDrv );
  secondCtrl.ButtonX.pressed(outtakeAndRollerDown);
  secondCtrl.ButtonL2.pressed( rollerDownDrv );
  secondCtrl.ButtonB.pressed(outtakeSlow);
  secondCtrl.ButtonA.pressed(intakeStartSlowDrive);
  mainControl.ButtonL1.pressed(  shoot );
  mainControl.ButtonR1.pressed(shootDriveSlow);
  mainControl.ButtonB.pressed(  shootBack );
  secondCtrl.ButtonUp.pressed(flipOpen);
#else
  mainControl.ButtonR1.pressed( intakeToggleDriver );
  mainControl.ButtonR2.pressed( outtake );
  mainControl.ButtonL1.pressed( rollerUpDrv );
  mainControl.ButtonL2.pressed( rollerDownDrv );
  mainControl.ButtonX.pressed(  shoot );
  mainControl.ButtonB.pressed(  shootBack );
  mainControl.ButtonUp.pressed(flipOpen);
  mainControl.ButtonA.pressed(outtakeAndRollerDown);
#endif
  // waits for the Inertial Sensor to calibrate
  InertialSensor.calibrate();
  while (InertialSensor.isCalibrating() && (limitLoop < 25)) {
    limitLoop++;
    wait(100, msec);
  }
  if (limitLoop == 25) {
    mainControl.Screen.newLine();
    mainControl.Screen.print("ERROR Gyro calibrate");
  }
  InertialSensor.resetRotation();
  InertialSensor.resetHeading();
  InertialSensor.setHeading(315, deg);


}

int PrepLimShootTask(void){
  int i = 0;
  int cmdCount = 0;

  while (1) {
    if (PreLoadBallMessage && !lim.pressing()){    // Is there a command to load the ball
      rollerStartSlow();
      cmdCount++;
      i = 0;

      mainControl.Screen.setCursor(1,1);//print
      mainControl.Screen.print("Preload %d ", cmdCount);

      while (!lim.pressing() && (i<400)){
        i++;
        wait(10, msec);
      }
      if (!lim.pressing()){
        mainControl.Screen.print("WRN1:BallNotLoaded");
      }
      wait(100, msec);           // ensure the ball is securely pressing the limit switch

      rollerStop();
      PreLoadBallMessage = 0;   //Ball loaded so reset the command
    }
    vex::task::sleep(25);
  }
}

void resetLaunchStatus(void){
  resetLaunchStatusMessage = 1;
}
int LaunchTrackerTask(void){      // ashna 11/22
  while (1) {
    if (resetLaunchStatusMessage){
      launched = 0;
      resetLaunchStatusMessage = 0;
    }
    if (lim.pressing()){
      lim_pressed = 1;
      launched = 0;
    }
    else if (!lim.pressing()){
      if (lim_pressed==1){
        launched = 1;
      }
      lim_pressed = 0;
    }
    vex::task::sleep(25);
  }
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control your robot during the autonomous phase of   */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

#define BALLINTAKEWAIT 500

/*void AutoSkills_1(void){
  // Skills program
  intakeStart();
  goFwd(9, 50); //pick up ball
  prepForLimShootLoaded();
  wait(25, msec);
  turnLeft(90);
  wait(10, msec);
  alignToHeading(-90);  // face wall
  wait(10, msec);
  intakeStop();
  slideFwd(16, 65);
  wait(25, msec);
  turnLeft(45);   //face goal 1
  alignToHeading(-135);
  wait(25, msec);
  shootStart();      
  slideFwd(13, 50);     // Approach 1st goal
  limShootLoaded();     // Score 1st goal

  wait(100, msec);
  
  shootStop();
  goBack(18, 80);       // Back up from goal
  wait(25, msec);
  turnRight(45);
  wait(25, msec);
  alignToHeading(-90); // Face the wall
  wait(25, msec);
  prepForLimShootLoaded();

  goBack(52.5, 80);       // Back up to 2nd goal
  turnLeft(90);
  alignToHeading(-180); // Face 2nd goal
  wait(25, msec);
  shootStart(); 
  slideFwd(14, 50);
  limShootLoaded();     // Score 2nd goal
  wait(100, msec);
  goBack(13, 80);       // Back up from goal
  shootStop(); 

  intakeStart();
  turnLeft(90);      
  wait(25, msec);
  alignToHeading(-270); // Face (other) wall
  wait(25, msec);

  goFwd(46, 80); //pick up ball
  intakeStop();
  prepForLimShootLoaded();
  turnRight(45);  //face goal 3
  wait(25, msec);

  alignToHeading(-225);
  wait(25, msec);
  shootStart();
  slideFwd(29, 50);
  limShootLoaded(); //score goal 3
  wait(100, msec);

  goBack(24, 80);
  shootStop();
  intakeStart();
  turnLeft(70); //facing ball
  wait(25, msec);
  alignToHeading(-295);
  wait(25, msec);
  slideFwd(19, 50); //pick up ball
  wait(.25, sec);
  goBack(26, 80);
  intakeStop();
  prepForLimShootLoaded();
  turnLeft(40);  //facing goal 4
  wait(25, msec);
  alignToHeading(-335);
  wait(25, msec);

  prepForLimShootLoaded();
  slideFwd(50, 50);
  limShootLoaded();     // Score goal 4
  wait(100, msec);
  goBack(15, 80);       // Back up from goal
  shootStop(); 

  turnLeft(50);
  wait(25, msec);
  alignToHeading(-25);
  wait(25, msec);
  goFwd(30, 80);
  turnRight(50); //facing ball
  wait(25, msec);
  alignToHeading(25);
  wait(25, msec);
  intakeStart();
  slideFwd(25, 50);
  goBack(15, 70);
  intakeStop();
  turnLeft(20);  //facing goal 5
  wait(25, msec);
  alignToHeading(5);
  wait(25, msec);

  prepForLimShootLoaded();
  slideFwd(40, 50);
  limShootLoaded();     // Score goal 5
  wait(100, msec);
  goBack(20, 80);       // Back up from goal
  shootStop(); */





//Test function to try out new subsystems
void AutoSkills_1Test(void){
  Roller.setStopping(brakeType::hold);

  mainControl.Screen.clearScreen();
  mainControl.Screen.setCursor(1,1);
  mainControl.Screen.print("G1 %3.2f", InertialSensor.heading());

  turnLeft(90);

  wait(25, msec);
  mainControl.Screen.print(" G2 %3.2f", InertialSensor.heading());

  wait(5000,msec);
  alignToHeading(270);
  wait(25,msec);

  mainControl.Screen.setCursor(2,1);
  mainControl.Screen.print("G3 %3.2f", InertialSensor.heading());

  wait(5000, msec);

  turnLeft(45);
  
  mainControl.Screen.print(" G4 %3.2f", InertialSensor.heading());

  alignToHeading(225);

  mainControl.Screen.setCursor(3,1);
  mainControl.Screen.print("G5 %3.2f", InertialSensor.heading());
  wait(5000, msec);

}

void AutoSkills_1(void){
  Roller.setStopping(brakeType::hold);
  // Skills program
  intakeStart();
  PreLoadBallMessage = 1;           // Tell Load ball task to move the preload ball to the limit switch         

  mainControl.Screen.clearScreen();
  mainControl.Screen.setCursor(1,1);

  goFwd(10, 60);             //9-10        //pick up ball in front
  wait(25, msec); //25-50
  turnLeft(95);
  wait(25, msec);
  alignToHeading(275);              // face wall
  wait(25, msec);
  //intakeStop();
  goFwd(15, 70); //16-15
  wait(25, msec);
  turnLeft(45);                     //face goal 1
  wait(25, msec);
  alignToHeading(225);
  wait(25, msec);
  mainControl.Screen.setCursor(1,1);
  mainControl.Screen.print("G3 %3.2f", InertialSensor.heading());

  shootStart();      
  slideFwd(15, 50); //12-13,14,15,15.5,15,14.5,14.5,15           // Approach 1st goal
  limShootLoadedPreStart();       // Score 1st goal
  wait(200,msec); //200-50
  rollerDown();
  intakeStop();
  outtake();
  wait(25,msec);
  rollerStop();

  goBack(14, 70);            //17.5-16,13-14,15,14   // Back up from goal
  wait(25,msec);
  intakeStop();
  PreLoadBallMessage=1;
  turnRight(45);
  wait(50, msec);
  alignToHeading(270);            // Face the wall
  wait(50, msec);
  mainControl.Screen.setCursor(2,1);
  mainControl.Screen.print(" G4 %3.2f", InertialSensor.heading());
  intakeStart();

  PreLoadBallMessage = 1;        // Tell Load ball task to move the first picked ball to the limit switch         

  goBack(52, 70);          //53-52,51,49,51,52,53,51,52,53   // Back up to 2nd goal
  wait(25, msec);
  turnLeft(90);
  wait(25, msec);
  alignToHeading(180);          // Face 2nd goal
  wait(25, msec);
  mainControl.Screen.setCursor(3,1);
  mainControl.Screen.print("G5 %3.2f", InertialSensor.heading());

  slideFwd(14, 70); //13,14,15,14
  limShootLoadedPreStart();      //2nd goal done
  wait(75, msec); //125-50
  rollerDown();
  intakeStop();
  outtakeSlow();
  goBack(20, 30);    //11-9-13,12, 10,11,13,18,19,20     // Back up from goal
  wait(75, msec);
  intakeStop();
  rollerStop();

  intakeStartSlow();
  turnLeft(77);    //90   
  wait(25, msec);
  alignToHeading(103);       // Face (other) wall
  wait(25, msec);
  mainControl.Screen.setCursor(1,1);
  mainControl.Screen.print(" G6 %3.2f", InertialSensor.heading());

  goFwd(55, 80);        //47-45-46-47,49,50,51,50,49,50,52,53,51,52,54,55         //pick up ball between goal 2 and 3
  PreLoadBallMessage = 1;           // Tell Load ball task to move the 2nd picked ball to the limit switch    
  wait(50,msec);     
  turnRight(32);  //face goal 3 //45
  wait(25, msec);
  alignToHeading(135);
  wait(25, msec);
 // intakeStop();
  slideFwd(22.5, 50); //25.5-24.5-24,23,22.5
  wait(25, msec);
  limShootLoadedPreStart();           //shoot to 3rd goal
  wait(50, msec);
  rollerDown();
  intakeStop();
  outtakeSlow();

  goBack(17, 60); //22-30,21,18,20,19, 20,18,19,18,17.5,16,15,16,17,18,17
  wait(50,msec);
  rollerStop();
  intakeStop();

  intakeStartSlow();
  turnLeft(135); //facing ball
  wait(25, msec);
  alignToHeading(0); 
  wait(25, msec);
  goFwd(51,80); //pick up ball //47-44,50,53,51,49,50,53,51
  wait(150, msec);
  //goBack(2, 25);
  //wait(25,msec);

  PreLoadBallMessage = 1;
  turnRight(90);  //facing goal4 
  wait(25, msec);
  alignToHeading(90); 
  wait(25, msec);
  intakeStop();

  slideFwd(14, 70); 
  wait(25,msec);
  limShootLoadedPreStart(); //score goal 4
  wait(50,msec);  //100-50

  //Head towards Goal 5
  goBack(11, 70);
  wait(25,msec);
  turnLeft(90);
  wait(25,msec);
  alignToHeading(0);
  wait(25,msec);

  // Go pick up the wall ball for goal 5
  goFwd(33, 80); //34-33,
  wait(25,msec);
  intakeStart();
  turnRight(90);          //face ball 
  wait(25,msec);
  alignToHeading(90);
  wait(25,msec);
  slideFwd(17, 70); //17-19
  wait(200,msec);


  goBack(14, 70); //10,14,16,19,18,16,14
  wait(25,msec);

  PreLoadBallMessage = 1;

  turnLeft(60); //30-40,75,70,60 //face goal
  wait(25,msec);
  alignToHeading(30);
  wait(25,msec);
  intakeStop();

  slideFwd(34,70); //30-32,34
  wait(25,msec);
  limShootLoadedPreStart(); //score goal 5
  wait(50,msec); //150-50

  goBack(15.5,70); //15-15.5
  wait(25,msec);
  turnLeft(110);
  wait(25,msec);
  alignToHeading(270);
  wait(25,msec);

  intakeStartSlow();
  slideFwd(52,70); //53-54,52
  wait(100,msec);         // give time to pick the ball against the wall
  PreLoadBallMessage=1;
  turnRight(90); //face goal 6
  wait(25,msec);
  alignToHeading(0);
  wait(25,msec);
  intakeStop();

  slideFwd(17, 80); //13-15,16,17
  limShootLoadedPreStart();      //score goal 6
  wait(50, msec);  //125-50
  goBack(11.5,70);   // Back up from goal //11-14,12,11,12,11.5
  wait(25, msec);

  intakeStartSlow();
  turnLeft(90);      
  wait(25, msec);
  alignToHeading(270);       // Face left wall
  wait(25, msec);
  
  goFwd(47, 80);  //49-46,42,45,44,46,45,46,47,46,47         //pick up ball
  PreLoadBallMessage = 1;           
  wait(25,msec);     
  turnRight(45);  //face goal 7
  wait(25, msec);
  alignToHeading(315);
  wait(25, msec);
  intakeStop();
  slideFwd(25, 70); //26-25
  wait(25, msec);
  limShootLoadedPreStart();           //score goal 7
  wait(50, msec);  //150-50

  goBack(19, 70);  //17-18,21,19
  wait(25,msec);
  turnRight(198); //201-199,198
  wait(25,msec);
  alignToHeading(155);//face ball //156
  wait(25,msec);

  mainControl.Screen.setCursor(2,1);
  mainControl.Screen.print("G7 %3.2f", InertialSensor.heading());
  
  intakeStartSlow();
  goFwd(62, 80); //60-61,65,62
  wait(50,msec);    // give some time to intake ball on field
  turnLeft(66);
  mainControl.Screen.print(" %3.2f", InertialSensor.heading());
  PreLoadBallMessage=1;
  wait(25,msec);
  alignToHeading(90); //face center goal
  mainControl.Screen.setCursor(3,1);
  mainControl.Screen.print("G8 %3.2f", InertialSensor.heading());

  wait(25,msec);

  slideFwd(21,80); //descore
  wait(25,msec);
  goBack(9,50); //10-9
  wait(25,msec);

  slideFwd(6,50);
  intakeStop();
  wait(25,msec);
  turnLeft(25);

  mainControl.Screen.print(" %3.2f", InertialSensor.heading());

  //wait(50,msec);
  //alignToHeading(70);
  wait(50,msec);
  slideFwd(4,70);
  limShootLoadedPreStart(); //score center
  wait(100,msec);
  outtake();
  goBack(15,50);

  //Go for 8th goal
  turnLeft(115);
  wait(25,msec);
  alignToHeading(300); //face ball
  wait(25,msec);
  intakeStartSlow();
  rollerStartSlow();
  goFwd(35,60); //pick up ball
  wait(25,msec);
//  PreLoadBallMessage=1;
  wait(50,msec);
  turnLeft(32); //28
  wait(25,msec);
  alignToHeading(268); //270
  wait(25,msec);
  slideFwd(14,700); //16-14
  //intakeStop();
  wait(25,msec);
  shootStart();
  //limShootLoadedPreStart();
  wait(100,msec);
  goBack(15,70);

  

  
  /*turnLeft(135); //facing ball
  wait(100, msec);
  alignToHeading(180); 
  wait(100, msec);
  mainControl.Screen.setCursor(2,1);
  mainControl.Screen.print(" G7 %3.2f", InertialSensor.heading());
  intakeStartSlow();
 
  goFwd(52,60); //pick up ball 52,51,52,53,52
  wait(250, msec);
  goBack(25, 25);
  wait(25,msec);

  PreLoadBallMessage = 1;
  turnRight(90);  //facing goal 8
  wait(50, msec);
  alignToHeading(270); 
  wait(100, msec);
  intakeStop();

  slideFwd(14, 50);  //13-14
  wait(25,msec);
  limShootLoadedPreStart(); //score goal 8
  wait(100,msec);

  goBack(15, 50);
  wait(25,msec);
  intakeStartSlow();
  turnRight(173); //175-173
  wait(50,msec);
  alignToHeading(83);
  wait(50,msec);
  slideFwd(46,90); //42-45,46
  wait(50,msec);

  PreLoadBallMessage=1;

  //descore center goal
  wait(100,msec);
  goBack(7,50);
  wait(50,msec);

  turnRight(15);
  wait(50,msec);
  //alignToHeading(98);
  //wait(100,msec);
  goFwd(10,50);  //14-12,10
  limShootLoadedPreStart();
  wait(100,msec);
  goBack(20, 30);*/

  
}

void AutoLCycle1(int LeftRight, int numGoals) {

  int i =0;                     // generic variable used through the function

  intakeStartSlow();

  goFwd(12, 80); //28 to9,9-12,12-14,12
  wait(25, msec);
  turnLeft(90); //132 to 90
  wait(50, msec);
  alignToHeading(270);
  wait(50, msec);
  
  goFwd(19.5, 80); //20-18,17,19,19.5
  turnLeft(45);
  wait(25,msec);
  alignToHeading(225);
  wait(50,msec);

  slideFwd(15, 90); //31.5 to 12, 12-14,14-16, 18,12-13comp-14 15
  wait(25, msec);

  // ** Pick first ball in front of goal
//0129  rollerStartSlow();              // Slow load to avoid pushing 
  slideFwdNonBlock(8,25); //11 to 4 to 8

  prepForLimShootLoaded();
//  rollerStart();                  // get the preload into shooting position i.e. and is pressing the limit switch

  shootStart();

  // ** Line up to the goal and shoot first ball
  goBack(0.5, 30);
  wait(25, msec);
  turnLeft(3);
  wait(25, msec);

  limShootLoadedPreStart();                 // scored 1st
  wait(25, msec);        // 0129 wait(200, msec);                  // Give shooter time to shoot the ball


  // ** Get second ball into shooting position and shoot second ball
  prepForLimShootLoaded();

  limShootLoadedPreStart();                 // scored 2nd
//  wait(25, msec);        // 0129 wait(200, msec);                  // Give shooter time to shoot the ball

intakeStop();
  // ** Get third ball into shooting position and shoot third ball
  prepForLimShootLoaded();

//  wait(100, msec);                      // what is this wait for ? remove if not needed

  // ** Stop intake as 3rd red ball and blue ball should be in robot and 
  // ** Dont want to pick the balls already shot in the goals
  //intakeStop();

  limShootLoadedPreStart();                     // scored 3rd
  wait(25, msec); // 0129 wait(300, msec); 

  // ** Go back from goal and eject the blue ball
  goBack(5, 80);
  outtake();
  rollerStop();
  rollerDown();     // eject the blue ball

  goBack(14, 90);  // 29 to 40 split to 30 + 10 (see 2 lines below)  go back 45to18, 18-20,17,15.5,14
//  vex::task::sleep(25);       // Some time to ensure blue ball is ejected

  
}

void AutoLeft1(void){
  intakeStart();

  goFwd(14, 100); //28 to9,9-12,12-14
  wait(25, msec);
  turnLeft(90); //132 to 90
  wait(50, msec);
  alignToHeading(270);
  wait(50, msec);
  
  goFwd(17, 100); //20-18,17
  turnLeft(45);
  wait(25,msec);
  alignToHeading(225);
  wait(50,msec);

  slideFwd(12, 90); //31.5 to 12, 12-14,14-16, 18
  wait(25, msec);

  // ** Pick first ball in front of goal
//0129  rollerStartSlow();              // Slow load to avoid pushing 
  slideFwdNonBlock(8,25); //11 to 4 to 8

  prepForLimShootLoaded();
//  rollerStart();                  // get the preload into shooting position i.e. and is pressing the limit switch

  shootStart();

  // ** Line up to the goal and shoot first ball
  goBack(0.5, 30);
  wait(25, msec);
  turnLeft(3);
  wait(25, msec);

  limShootLoadedPreStart();                 // scored 1st
  wait(25, msec);        // 0129 wait(200, msec);                  // Give shooter time to shoot the ball


    goBack(15, 90);

  /*intakeStart();
  prepForLimShootLoaded();

  goFwd(12, 50); 
  wait(25, msec);
  turnLeft(90);
  wait(100, msec);
  alignToHeading(270);  // face wall
  wait(100, msec);
  goFwd(19, 65);
  wait(25, msec);
  turnLeft(45);   //face goal 1
  wait(50, msec);
  alignToHeading(225);
  wait(50, msec);

  //shootStart();      
  slideFwd(15, 50);     // Approach 1st goal
  intakeStop();
  wait(50,msec);
  shootStart();
  rollerStart();
  slideFwd(2,50);
  //limShootLoadedPreStart();     // Score 1st goal
  wait(1,sec);
  goBack(17.5, 60);       // Back up from goal*/
}

void AutoRight1(void){
  intakeStart();

  goFwd(17, 100); //28 to9,9-12,12-14
  wait(25, msec);
  turnRight(90); //132 to 90
  wait(50, msec);
  alignToHeading(90);
  wait(50, msec);
  
  goFwd(19, 100); //20-18,17
  turnRight(45);
  wait(25,msec);
  alignToHeading(135);
  wait(500,msec);
  slideFwd(17, 90); //31.5 to 12, 12-14,14-16, 18
  wait(25, msec);

  slideFwdNonBlock(8,25); //11 to 4 to 8

  prepForLimShootLoaded();
//  rollerStart();                  // get the preload into shooting position i.e. and is pressing the limit switch
  intakeStop();
  shootStart();

  // ** Line up to the goal and shoot first ball
  goBack(0.5, 30);
  wait(25, msec);
  turnRight(3);
  wait(25, msec);

  limShootLoadedPreStart();                 // scored 1st
  wait(25, msec);        // 0129 wait(200, msec);                  // Give shooter time to shoot the ball

  goBack(15, 90);  
}
void AutoMid1(void){
  prepForLimShootLoaded();

  goFwd(9, 50); 
  wait(25, msec);
  turnLeft(90);
  wait(100, msec);
  alignToHeading(270);  // face wall
  wait(100, msec);
  goFwd(16, 90);
  wait(25, msec);
  turnLeft(90);   //face goal 1
  wait(50, msec);
  alignToHeading(180);
  wait(50, msec);

  shootStart();      
  slideFwd(12, 50);     // Approach 1st goal
  limShootLoadedPreStart();     // Score 1st goal

  goBack(17.5, 60);       // Back up from goal
}

void AutoGoalLeft123Cycle12(void){
  int i =0;                     // generic variable used through the function
  robotDrive.setHeading(315, deg);
  robotDrive.setTurnVelocity(90, pct);  //0509 70->90
  drivePI(25); //4-8
  flipOpenStop();
//  wait(25,msec);
//  drivePI(15);  //0313 -- 10

  Roller.setStopping(brakeType::hold);
  PreLoadBallMessage=1;

  //Head to goal 1
  robotDrive.turnToHeading(225,deg);
  outtakeSlow();
  drivePI(27); 

  // Head to goal 2
  drivePI(-19);
  intakeStop();
  robotDrive.turnToHeading(270, deg);


  drivePI(-43); // drivePID(48);   //39 41->45 //40to53, 53-50,48,48-47comp,47.5,48.5 //47 //49 // 3/6/21 52-46
  wait(25,msec);

  robotDrive.turnToHeading(180, deg);
  shootStart();

  // ** Approach second goal and pull in the red ball and load it for shooting
  slideFwd(14, 70); //0509 16 -> 14
  slideFwdNonBlock(2, 50);
  wait(25,msec);
  limShootLoadedPreStart();                     // scored 2nd goal
  //slideFwd(2, 70);
  // rollerStartSlow();
//  wait(50, msec);

  // Go to center goal
  stopDriveTrain();
  alignToHeading(180);
  wait(25,msec);
  slideFwd(-41, 80); //5/1 -48-> -45 -> 41
  slideFwdNonBlock(-4, 80);
  wait(150,msec);
  stopDriveTrain();
  alignToHeading(180);
  drivePI(12);
  robotDrive.turnToHeading(119, deg);
  outtakeSlow();
  drivePI(75);
  wait(50,msec);
  //goBack(10,70); //mar26 commented out
  

 
  /*goBack(15, 70);    //11-9-13,12, 10,11,13,15   // Back up from goal
  wait(25, msec);
  
  turnLeft(90);    //90   
  wait(25, msec);
  alignToHeading(90);       // Face (other) wall
  wait(25, msec);


  goFwd(42, 80);        //47-45-46-47,49,50,51,50,49,50,52,53,51,52,54,55,52         //pick up ball between goal 2 and 3           // Tell Load ball task to move the 2nd picked ball to the limit switch    
  wait(25,msec);    
  turnRight(45);  //face goal 3 //45
  wait(25, msec);
  alignToHeading(135);
  wait(25, msec);
  outtake();
 
  slideFwd(23.5, 80); //25.5-24.5-24,23,22.5        //wedge ball into 3
  wait(50, msec);
  goBack(15, 80);*/

}

void AutoGoalLeft123Cycle12Advanced(void){
  int i =0;                     // generic variable used through the function
  robotDrive.setHeading(315, deg);
  robotDrive.setTurnVelocity(80, pct);  //0509 70->90
  wait(250, msec);
  flipOpenStop();
  drivePI(33);
#if 1
//0516  robotDrive.turnToHeading(225, deg);
  turnLeft(90);
  wait(25,msec);
  alignToHeading(225);
  wait(25, msec);
#endif

  outtakeSlow();
  drivePI(38);
  wait(25,msec);
 
  drivePI(-31);
  intakeStop();
#if 1
//0516  robotDrive.turnToHeading(0,deg);
  turnRight(135);
  wait(25,msec);
  alignToHeading(0);
  wait(25, msec);

#endif
  intakeStart();
  drivePI(36);
  wait(100,msec);
  
  drivePI(-22);
#if 1
//0516  robotDrive.turnToHeading(270,deg);
  turnLeft(90);
  wait(25,msec);
  alignToHeading(225);
  wait(25, msec);
#endif
  drivePI(-36);
#if 1
//0516  robotDrive.turnToHeading(180,deg);
  turnLeft(90);
  wait(25,msec);
  alignToHeading(180);
  wait(25, msec);
#endif

  slideFwdNonBlock(-9,60);
  wait(100, msec);

  while (robotDrive.isMoving() && i < 50){
    i++;
    wait (25, msec);
  }

  stopDriveTrain();
  alignToHeading(180);
  PreLoadBallMessage=1;
  drivePI(12);
  intakeStop();
#if 1
//0516  robotDrive.turnToHeading(119, deg);
  turnLeft(61);
  wait(25,msec);
  alignToHeading(119);
  wait(25, msec);
#endif
  outtakeSlow();
  shootStart();
  drivePI(75);
  limShootLoadedPreStart();
  wait(50,msec);
  //goBack(10,70); //mar26 commented out
  

 
  /*goBack(15, 70);    //11-9-13,12, 10,11,13,15   // Back up from goal
  wait(25, msec);
  
  turnLeft(90);    //90   
  wait(25, msec);
  alignToHeading(90);       // Face (other) wall
  wait(25, msec);


  goFwd(42, 80);        //47-45-46-47,49,50,51,50,49,50,52,53,51,52,54,55,52         //pick up ball between goal 2 and 3           // Tell Load ball task to move the 2nd picked ball to the limit switch    
  wait(25,msec);    
  turnRight(45);  //face goal 3 //45
  wait(25, msec);
  alignToHeading(135);
  wait(25, msec);
  outtake();
 
  slideFwd(23.5, 80); //25.5-24.5-24,23,22.5        //wedge ball into 3
  wait(50, msec);
  goBack(15, 80);*/

}


void AutoGoalRight123Cycle12(){
  
  int i =0;                     // generic variable used through the function

  intakeStartSlow();

  goFwd(14, 100); //28 to9,9-12,12-14
  wait(25, msec);
  turnRight(90); //132 to 90
  wait(50, msec);
  alignToHeading(90);
  wait(50, msec);
  
  goFwd(19, 100); //20-18,17
  turnRight(45);
  wait(25,msec);
  alignToHeading(135);
  wait(500,msec);

  slideFwd(17, 90); //31.5 to 12, 12-14,14-16, 18
  wait(25, msec);

  // ** Pick first ball in front of goal
//0129  rollerStartSlow();              // Slow load to avoid pushing 
  slideFwdNonBlock(8,25); //11 to 4 to 8

  prepForLimShootLoaded();
//  rollerStart();                  // get the preload into shooting position i.e. and is pressing the limit switch

  shootStart();

  // ** Line up to the goal and shoot first ball
  goBack(0.5, 30);
  wait(25, msec);
  turnRight(3);
  wait(25, msec);

  limShootLoadedPreStart();                 // scored 1st
  wait(25, msec);        // 0129 wait(200, msec);                  // Give shooter time to shoot the ball


  // ** Get second ball into shooting position and shoot second ball
  prepForLimShootLoaded();

  limShootLoadedPreStart();                 // scored 2nd
//  wait(25, msec);        // 0129 wait(200, msec);                  // Give shooter time to shoot the ball


  // ** Get third ball into shooting position and shoot third ball
  prepForLimShootLoaded();

//  wait(100, msec);                      // what is this wait for ? remove if not needed

  // ** Stop intake as 3rd red ball and blue ball should be in robot and 
  // ** Dont want to pick the balls already shot in the goals
  intakeStop();

  limShootLoadedPreStart();                     // scored 3rd
  wait(25, msec); // 0129 wait(300, msec); 

  // ** Go back from goal and eject the blue ball
  goBack(5, 90);
  outtake();
  rollerStop();
  rollerDown();     // eject the blue ball

  goBack(16, 90);  // 29 to 40 split to 30 + 10 (see 2 lines below)  go back 45to18, 18-20,17
//  vex::task::sleep(25);       // Some time to ensure blue ball is ejected

  // ** Line to the second goal 
  vex::task::sleep(25);
/*#if 1
  turnToRotationPID(-212);    //270 ->-222 -> 215 -> 210
#else*/
  turnRight(135); //65to90,135
  vex::task::sleep(25);
  alignToHeading(270); //-208to180, 180to90
//#endif
  rollerStop();

  vex::task::sleep(25);
  intakeStart();
  goFwd(46, 100); // drivePID(48);   //39 41->45 //40to53, 53-50,48,
  vex::task::sleep(25);

  turnLeft(90);
  wait(25,msec);
  alignToHeading(180);
  wait(25,msec);

  // ** Approach second goal and pull in the red ball and load it for shooting
  slideFwd(15, 80); //8to9,12,15,17, 18
  rollerStartSlow();
  wait(150, msec);
  prepForLimShootLoaded();
  limShootLoadedPreStart();                             // scored 2nd goal
  //wait(200, msec); //test
  intakeStop();

  // ** Go back and eject blue ball
  rollerStop();
  wait(25,msec);
  goBack(20, 90);   //  drivePID(-20);20
  rollerDown();
  outtake();
  wait(25,msec);
  rollerStop();
}
 /* intakeStart();
  rollerStart();    
//VK  swingRight(52, 0.30);  // Using a lower ratio 0f 0.3 vs 0.33
  goFwd(28, 80);
  wait(25, msec);
  turnRight(135);
  wait(25, msec);
  
  alignToHeading(135);
  wait(25, msec);

  goFwd(43, 75);
  vex::task::sleep(25);

  goFwd(1, 30);       
  wait(BALLINTAKEWAIT, msec);
  wait(100, msec);

  limShoot();      // scored 1st
  wait(350, msec);// wait to intake balls and let first ball be in the goal
  // wait(100, msec);// wait to intake balls and let first ball be in the goal
  intakeStop();
  limShoot();     // put the red ball back in
  // intakeStop();

  // goBack(38, 100); // drivePID(-38);
  goBack(10, 90);
  rollerStop();
  rollerDown();     // eject the blue ball
  outtake();
  goBack(27, 90);  // 29 to 40 split to 30 + 10 (see 2 lines below) //30-27
  intakeStop();
  rollerStop();
  vex::task::sleep(50);

  goBack(15, 90);  // 10 to 15
  rollerStart();

  vex::task::sleep(25);

  turnToRotationPID(204);    //270 ->-222 -> 216 -> 213
  vex::task::sleep(25);
  goFwd(47, 80); // drivePID(48);   //39 41->45->47->50
  vex::task::sleep(25);

  intakeStart();
  slideFwd(4, 80);
  wait(BALLINTAKEWAIT, msec);
  shootStart();                             // scored 2nd goal
  wait(350, msec);// wait to intake balls and let first ball be in the goal
  intakeStop();
  wait(900, msec);// wait to intake balls and let first ball be in the goal

  rollerStop();
  goBack(20, 75);   //  drivePID(-20);
  rollerDown();
  outtake();
}*/

void autonomous(void) {

  flipOpenStart();

  switch(MenuItemHighlighted){
      case AUTO_SKILLS_1:
        AutoSkills_1();
        break;
      case GOAL_LEFT_CYCLE_ADV:
        AutoGoalLeft123Cycle12Advanced();
        break;
      case GOAL_LEFT_123_CYCLE1:
        AutoGoalLeft123Cycle12();
        break;
      case GOAL_LEFT_123_CYCLE12:
        AutoGoalLeft123Cycle12();
        break;
      case GOAL_LEFT_1:
          AutoLeft1();
          break;
      case GOAL_RIGHT_123_CYCLE12:
        AutoGoalRight123Cycle12();
        break;
      case GOAL_RIGHT_1:
          AutoRight1();
          break;
        case GOAL_MID_1:
          AutoMid1();
          break;
      case NO_ACTION:                     // no action
          vex::task::sleep(10000); //Sleep the task for a short amount of time to prevent wasted resources.
          break;
    }
    
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control your robot during the user control phase of */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*  You must modify the code to add your own robot specific commands here.   */
/*---------------------------------------------------------------------------*/

void usercontrol(void) {
  // User control code here, inside the loop
  Shooter.setBrake(brakeType::brake);
  int fwd, trn;
  int leftPwr, rightPwr;

  PreLoadBallMessage = 0; //clear up preload message so preload task does nothing
  while (1) {
    //origial code
    fwd = filter(mainControl.Axis3.position()) * DRIVE_WEIGHT;
    trn = filter(mainControl.Axis1.position()) * TURN_WEIGHT;
    leftPwr = (fwd + trn);
    rightPwr = (fwd - trn);
  

    FrontLeftMotor.spin(vex::directionType::fwd, leftPwr, vex::velocityUnits::pct);
    FrontRightMotor.spin(vex::directionType::fwd, rightPwr, vex::velocityUnits::pct);
    BackLeftMotor.spin(vex::directionType::fwd, leftPwr, vex::velocityUnits::pct);
    BackRightMotor.spin(vex::directionType::fwd, rightPwr, vex::velocityUnits::pct);

    vex::task::sleep(20); //Sleep to save resources :)

  }
}
  
//
// Main will set up the competition functions and callbacks.
//
int main() {

  // Start ball detect task
  vex::task t(LaunchTrackerTask);
  vex::task t1(PrepLimShootTask);
    
  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);

  //Competition.drivercontrol(usercontrol);
  Competition.drivercontrol(usercontrol);
  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}
