#include "vex.h"
#include "bot-functions.h"

// ---- START VEXCODE CONFIGURED DEVICES ----
// Robot Configuration:
// [Name]               [Type]        [Port(s)]
// BackLeftMotor        motor         20              
// BackRightMotor       motor         2               
// FrontLeftMotor       motor         5               
// FrontRightMotor      motor         16              
// LeftIntake           motor         1               
// RightIntake          motor         3               
// Roller               motor         8               
// Shooter              motor         11              
// Controller1          controller                    
// ---- END VEXCODE CONFIGURED DEVICES ----

// yo what's up this is ashna, i'm making a test change :)

using namespace vex;
competition Competition;

// vex::motor BackLeftMotor = vex::motor(vex::PORT20);
// vex::motor BackRightMotor = vex::motor(vex::PORT2, true);
// vex::motor FrontLeftMotor = vex::motor(vex::PORT5);
// vex::motor FrontRightMotor = vex::motor(vex::PORT16, true);
// vex::motor LeftIntake = vex::motor(vex::PORT1);
// vex::motor RightIntake = vex::motor(vex::PORT3,true);
// vex::motor Roller = vex::motor(vex::PORT8, true);
// vex::motor Shooter = vex::motor(vex::PORT11, true);
// vex::controller mainControl = controller();

// //-------------------------
// //12.56 inches per rotation



void turnLeft(float);
void turnRight(float);


//GLOBAL VAR.S --------------------------------------------------------
//Status Variables
int SpeedSlow = 0;

int MenuItemHighlighted = 1;
int MenuItemSelected = 0;

// //---------------------------------------------------------------------
enum Sides{
  RED, BLUE
};

enum MenuItems{
  ITEM_NOT_USED,
  NO_ACTION,
  MENU_LENGTH
};

char MenuList[MENU_LENGTH][20] = {
  "",                   //0 ITEM_NOT_USED
  "No Action"           //7 NO_ACTION
};



// //Filter function for driving ---------
int filter (int value) {
  if (abs(value) > 16) {
    return (value);
  } else {
    return (0);
  }
}


//Functions: Drive

// Shooter Functions


// void homeRowAuton(){
//   open();
//   intake();
//   turnLeft(30);
//   shoot();
//   goBack(13, 70);
//   turnRight(120);
//   goFwd(36, 70);
//   shoot();
//   goBack(13, 70);
//   turnRight(45);
//   goFwd(6, 10);
//   intake();
//   shoot();
//   goBack(7, 90);
// }

void pre_auton(void) {
  vexcodeInit();
  mainControl.ButtonR1.pressed( intake );
  mainControl.ButtonR2.pressed( outtake );
  mainControl.ButtonL1.pressed( rollerUp );
  mainControl.ButtonL2.pressed( rollerDown );
  mainControl.ButtonX.pressed(shoot);
  mainControl.ButtonB.pressed(shootBack);
}

// int ballDetectTask(void) {
//   int lastLimitSwitchState = 0;
//   while(1) {
//     if (BallDetectSwitch.pressing()) {
//       if (!lastLimitSwitchState) {
//         rollerStop();
//         lastLimitSwitchState = 1;
//       }
//     }
//     else {
//       if (lastLimitSwitchState){
//         lastLimitSwitchState = 0;  
//       }
//     }
//     vex::task::sleep(50);
//   }
// }

void autonomous(void) {
  flipOpen();
  intakeStart();
  goFwd(2.5, 50);
  wait(500, msec);
  intakeStop();
  rollerStart();    
  wait(250, msec);
  turnLeft(48);
  slideFwd(6.25, 40);
  shootStart();        
  wait(1000, msec);
  shootStop();        // scored 1st goal
  goBack(45, 50);
  wait(250, msec);
  turnLeft(82);
  goFwd(27, 75);
  slideFwd(5, 30);
  shootStart();     
  wait(1100, msec); 
  shootStop();        // scored 2nd goal
  goBack(20, 75);
  turnLeft(50);
  intakeStart();  
  goFwd(59, 80);
  turnRight(51);
  slideFwd(24, 50);
  shootStart();
  turnRight(2);   //adjust
  wait(5000, msec);
  shootStop();
}


void usercontrol(void) {
  int fwd, trn;
  int leftPwr, rightPwr;
  while (1) {
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
    // vex::task t(ballDetectTask);

  // Set up callbacks for autonomous and driver control periods.
  Competition.autonomous(autonomous);
  Competition.drivercontrol(usercontrol);

  // Run the pre-autonomous function.
  pre_auton();

  // Prevent main from exiting with an infinite loop.
  while (true) {
    wait(100, msec);
  }
}