// yo this is ashna, i'm just trying to figure out how this github thing works

#include "vex.h"
using namespace vex;
competition Competition;

vex::motor BackLeftMotor = vex::motor(vex::PORT20);
vex::motor BackRightMotor = vex::motor(vex::PORT2, true);
vex::motor FrontLeftMotor = vex::motor(vex::PORT5);
vex::motor FrontRightMotor = vex::motor(vex::PORT16, true);
vex::motor LeftIntake = vex::motor(vex::PORT1);
vex::motor RightIntake = vex::motor(vex::PORT3,true);
vex::motor Roller = vex::motor(vex::PORT8, true);
vex::motor Shooter = vex::motor(vex::PORT11, true);
vex::inertial InertialSensor = vex::inertial(vex::PORT10);
vex::controller mainControl = controller();

// //Filter function for driving ---------
int filter (int value) {
  if (abs(value) > 16) {
    return (value);
  } else {
    return (0);
  }
}
// //-------------------------
// //12.56 inches per rotation
#define TURNING_SPEED 35  //org 35
#define SLOW_TURNING_SPEED 20
#define WHEELDIAMETER  4.0      // Wheel diameter in inches
#define WHEELCIRCUMFERENCE  (WHEELDIAMETER * 3.145)
#define REVTOANGLE  (1.0/93.0)   //1 rotations to turn 90 degrees + slippage
#define INTAKESPD 80
#define ROLLERSPD 100
#define SHOOTERSPD 100
#define TURN_WEIGHT 0.65  // Reduce turning sensitivity when driving. Forward/backward are not affected
#define DRIVE_WEIGHT 0.8  // Reduce driving sensitivity when driving. Forward/backward are not affected

int IntakeRunning = 0;
int RollerRunning = 0;
int ShooterRunning = 0;
void turnLeft(float);
void turnRight(float);


//GLOBAL VAR.S --------------------------------------------------------
// //Status Variables
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

//SENSOR FUNCTIONS

void goFwd(float distance, float velocity){
    float numberDeg;
    numberDeg = (distance/WHEELCIRCUMFERENCE)*360;
    BackLeftMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    BackRightMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct);
}

void slideFwd(float distance, float velocity){
    BackLeftMotor.setStopping(vex::brakeType::coast);
    FrontLeftMotor.setStopping(vex::brakeType::coast);
    BackRightMotor.setStopping(vex::brakeType::coast);
    FrontRightMotor.setStopping(vex::brakeType::coast);
    float numberDeg;
    numberDeg = (distance/WHEELCIRCUMFERENCE)*360;
    BackLeftMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    BackRightMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct);
    BackLeftMotor.setStopping(vex::brakeType::brake);
    FrontLeftMotor.setStopping(vex::brakeType::brake);
    BackRightMotor.setStopping(vex::brakeType::brake);
    FrontRightMotor.setStopping(vex::brakeType::brake);
}

void goBack(float distance, float velocity){
    float numberDeg;
    numberDeg = (distance/WHEELCIRCUMFERENCE)*360;

    BackLeftMotor.rotateFor(directionType:: rev, numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(directionType:: rev, numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(directionType:: rev, numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    BackRightMotor.rotateFor(directionType:: rev, numberDeg,rotationUnits::deg,velocity,velocityUnits::pct);
}


void stopDriveTrain(void) {
  BackLeftMotor.stop();
  FrontLeftMotor.stop();
  FrontRightMotor.stop();
  BackRightMotor.stop();
}

void turnLeft(float degree){
    float numberOfRevs = degree * REVTOANGLE;
    BackLeftMotor.rotateFor(directionType::rev, numberOfRevs,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(directionType::rev, numberOfRevs,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(numberOfRevs,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    BackRightMotor.rotateFor(numberOfRevs,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct);
    
}

void turnRight(float degree){
    float numberRev = degree * REVTOANGLE;
    BackLeftMotor.rotateFor(numberRev,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(numberRev,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(directionType::rev, numberRev,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    BackRightMotor.rotateFor(directionType::rev, numberRev,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct);

}

void turnToRotationPID(double targetHeading){
  double error;
  double maxAllowedError = 0.5;
  double currentHeading;
  double kP = 1;
  double power;
  bool timerExpired = false;
  timer errorTimer = timer();
  double errorTimerMax = 500;

  Brain.Screen.clearScreen();
    
  errorTimer.clear();

  currentHeading = InertialSensor.rotation(rotationUnits::deg);
  error = targetHeading - currentHeading;

  while( (fabs(error) > maxAllowedError) && (timerExpired == false) ) {
    currentHeading = InertialSensor.rotation(rotationUnits::deg);
    error = targetHeading - currentHeading;
    power = error * kP;
    BackLeftMotor.spin(directionType::fwd, power, velocityUnits::pct);
    FrontLeftMotor.spin(directionType::fwd, power, velocityUnits::pct);
    FrontRightMotor.spin(directionType::rev, power, velocityUnits::pct);
    BackRightMotor.spin(directionType::rev, power, velocityUnits::pct);

    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print("Heading= ");
    Brain.Screen.print("%3.2f", currentHeading);
    Brain.Screen.newLine();
    Brain.Screen.print("Error= ");
    Brain.Screen.print("%3.2f", error);
    Brain.Screen.newLine();
    Brain.Screen.print("Power= ");
    Brain.Screen.print("%3.2f", power);
    Brain.Screen.newLine();
    Brain.Screen.print("Timer= ");
    Brain.Screen.print("%2.6f", errorTimer.time());
    
    if(fabs(error) > maxAllowedError) {
      errorTimer.clear();
    } 
    else {
     if (errorTimer.time() > errorTimerMax) {
       timerExpired = true;
      }
    }
    vex::task::sleep(50);
  }
}

void drivePID(double targetDistance){
  double error;
  double maxAllowedError = 0.25; // allowed error in inches
  double currentDistance;
  double kP = 1;
  double power;
  bool timerExpired = false;
  timer errorTimer = timer();
  double errorTimerMax = 500; // time in msec

  Brain.Screen.clearScreen();
    
  errorTimer.clear();

  FrontRightMotor.resetRotation(); // used front right motor to control driving distance

  currentDistance = 0;
  error = targetDistance - currentDistance;

  while( (fabs(error) > maxAllowedError) && (timerExpired == false) ) {
    currentDistance = (FrontRightMotor.rotation(rotationUnits::deg) / 360) * WHEELCIRCUMFERENCE;
    error = targetDistance - currentDistance;
    power = error * kP;
    
    BackLeftMotor.spin(directionType::fwd, power, velocityUnits::pct);
    FrontLeftMotor.spin(directionType::fwd, power, velocityUnits::pct);
    FrontRightMotor.spin(directionType::fwd, power, velocityUnits::pct);
    BackRightMotor.spin(directionType::fwd, power, velocityUnits::pct);

    Brain.Screen.setCursor(1, 1);
    Brain.Screen.print("Distance= ");
    Brain.Screen.print("%3.2f", currentDistance);
    Brain.Screen.newLine();
    Brain.Screen.print("Error= ");
    Brain.Screen.print("%3.2f", error);
    Brain.Screen.newLine();
    Brain.Screen.print("Power= ");
    Brain.Screen.print("%3.2f", power);
    Brain.Screen.newLine();
    Brain.Screen.print("Timer= ");
    Brain.Screen.print("%2.6f", errorTimer.time());
    
    if(fabs(error) > maxAllowedError) {
      errorTimer.clear();
    } 
    else {
     if (errorTimer.time() > errorTimerMax) {
       timerExpired = true;
      }
    }
    vex::task::sleep(50);
  }
  BackLeftMotor.stop(brake);
  FrontLeftMotor.stop(brake);
  FrontRightMotor.stop(brake);
  BackRightMotor.stop(brake);
}

void PIDTest () {
  drivePID(30);
  wait(1000, msec);
  drivePID(-30);
  wait(1000, msec);
  turnToRotationPID(90);
  wait(1000, msec);
  drivePID(12);
  wait(1000, msec);
  drivePID(-12);
  wait(1000, msec);
  turnToRotationPID(-90);
  wait(1000, msec);
}

void intake (){
  if (IntakeRunning == 0){
    LeftIntake.spin(vex::directionType::fwd, INTAKESPD, vex::velocityUnits::pct);
    RightIntake.spin(vex::directionType::fwd, INTAKESPD, vex::velocityUnits::pct);
    Roller.spin(vex::directionType::fwd, ROLLERSPD, vex::velocityUnits::pct);
    IntakeRunning = 1;
    RollerRunning = 1;
  }
  else{
    LeftIntake.stop();
    RightIntake.stop();
    Roller.stop();
    IntakeRunning = 0;
    RollerRunning = 0;
  }
}

void outtake(){
  if (IntakeRunning == 0){
    LeftIntake.spin(vex::directionType::rev, INTAKESPD, vex::velocityUnits::pct);
    RightIntake.spin(vex::directionType::rev, INTAKESPD, vex::velocityUnits::pct);
    IntakeRunning = 1;
  }
  else{
    LeftIntake.stop();
    RightIntake.stop();
    IntakeRunning = 0;
  } 
}

void rollerUp(){
  if (RollerRunning == 0){
    Roller.spin(vex::directionType::fwd, ROLLERSPD, vex::velocityUnits::pct);
    RollerRunning = 1;
  }
  else{
    Roller.stop();
    RollerRunning = 0;
  }
}

void rollerDown(){
  if (RollerRunning == 0){
    Roller.spin(vex::directionType::rev, ROLLERSPD, vex::velocityUnits::pct);
    RollerRunning = 1;
  }
  else{
    Roller.stop();
    RollerRunning = 0;
  }
}

void flipOpen(){
  rollerDown();
  vex::task::sleep(500);
  rollerDown();
}

// void rollerStartFwd(){
//     Roller.spin(vex::directionType::fwd, ROLLERSPD, vex::velocityUnits::pct);
//     RollerRunning = 1;
// }


// void rollerStop(){
//   Roller.stop();
//   RollerRunning = 0;
// }

void shoot(){
 if (ShooterRunning == 0){
     Shooter.spin(vex::directionType::fwd, SHOOTERSPD, vex::velocityUnits::pct);
     ShooterRunning = 1;
   }
   else{
     Shooter.stop();
     ShooterRunning = 0;
   }
}

void shootBack(){
  if (ShooterRunning == 0){
      Shooter.spin(vex::directionType::rev, SHOOTERSPD, vex::velocityUnits::pct);
      ShooterRunning = 1;
    }
    else{
      Shooter.stop();
      ShooterRunning = 0;
    }
}

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
  InertialSensor.calibrate();
  // waits for the Inertial Sensor to calibrate
  while (InertialSensor.isCalibrating()) {
    wait(100, msec);
  }
  InertialSensor.resetRotation();
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
  while (InertialSensor.isCalibrating()) {
    wait(100, msec);
  }

  PIDTest(); // testing PID function

  flipOpen();
  intake();       //start intake
  goFwd(2.5, 50);
  wait(500, msec);
  intake();       //stop intake
  rollerUp();     //start roller
  wait(250, msec);
  turnLeft(48);
  slideFwd(6.25, 40);
  shoot();        //start shooter
  wait(1000, msec);
  shoot();        //stop shooter, scored 1st goal
  goBack(45, 50);
  wait(250, msec);
  turnLeft(82);
  goFwd(27, 75);
  slideFwd(5, 30);
  shoot();        //start shooter
  wait(1100, msec); 
  shoot();        //stop shooter scored 2nd goal
  goBack(20, 75);
  turnLeft(50);
  intake();       //start intake
  goFwd(59, 80);
  turnRight(51);
  slideFwd(24, 50);
  shoot();
  turnRight(2);   //adjust
  wait(5000, msec);
  shoot();

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