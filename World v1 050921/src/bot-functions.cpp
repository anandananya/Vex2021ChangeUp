#include "vex.h"
#include "bot-functions.h"

int IntakeRunning = 0;
int RollerRunning = 0;
int ShooterRunning = 0;
int lim_pressed = 0;
int launched = 0;
// hey XD if you need to put more ints here do 'extern int IDK LOL;' in bot-functions.h


void setDrivetoBrake(void){
    BackLeftMotor.setStopping(vex::brakeType::brake);
    FrontLeftMotor.setStopping(vex::brakeType::brake);
    BackRightMotor.setStopping(vex::brakeType::brake);
    FrontRightMotor.setStopping(vex::brakeType::brake);
}

void setDrivetoBrakeType(vex::brakeType brakeValue){
    BackLeftMotor.setStopping(brakeValue);
    FrontLeftMotor.setStopping(brakeValue);
    BackRightMotor.setStopping(brakeValue);
    FrontRightMotor.setStopping(brakeValue);
}
void goFwd(float distance, float velocity){
    float numberDeg;
    numberDeg = (distance/WHEELCIRCUMFERENCE)*360;
    BackLeftMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    BackRightMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct);
}

void slideFwd(float distance, float velocity){
    float numberDeg;
    numberDeg = (distance/WHEELCIRCUMFERENCE)*360;
    setDrivetoBrakeType(coast);
    BackLeftMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    BackRightMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct);
    setDrivetoBrakeType(brake);
}


void slideFwdNonBlock(float distance, float velocity){
    setDrivetoBrakeType(coast);
    float numberDeg;
    numberDeg = (distance/WHEELCIRCUMFERENCE)*360;
    BackLeftMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    BackRightMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    setDrivetoBrakeType(brake);
}


void slideBackNonBlock(float distance, float velocity){
    setDrivetoBrakeType(coast);
    float numberDeg;
    numberDeg = (distance/WHEELCIRCUMFERENCE)*360;
    BackLeftMotor.rotateFor(directionType:: rev, numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(directionType:: rev, numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    BackRightMotor.rotateFor(directionType:: rev, numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(directionType:: rev, numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    setDrivetoBrakeType(brake);
}

void goBack(float distance, float velocity){
    float numberDeg;
    numberDeg = (distance/WHEELCIRCUMFERENCE)*360;

    BackLeftMotor.rotateFor(directionType:: rev, numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(directionType:: rev, numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(directionType:: rev, numberDeg,rotationUnits::deg,velocity,velocityUnits::pct, false);
    BackRightMotor.rotateFor(directionType:: rev, numberDeg,rotationUnits::deg,velocity,velocityUnits::pct);
}

void drivePI(double targetDistance) {
  // Brain.Screen.print("HEY BESTIE UWU");
  double motorspeed = 0, currentDistance = 0,
         error = (targetDistance - currentDistance), errorSum = 0, deltaE = 0,
         lastError = 0, maxAllowedError = 0.75, errorTimerMax = 50;
  double kP = 8, kI = 0, kD = 20; //working 8,0,20
  bool timerExpired = false;
  timer errorTimer = timer();
  errorTimer.clear();
  FrontRightMotor
      .resetRotation(); // used front right motor to control driving distance
  FrontLeftMotor.resetRotation();

  while ((fabs(error) > maxAllowedError) && (timerExpired == false)) {
    currentDistance = ((FrontRightMotor.rotation(rotationUnits::deg) +
                        FrontLeftMotor.rotation(rotationUnits::deg)) /
                       2 / 360) *
                      WHEELCIRCUMFERENCE;
    error = targetDistance - currentDistance;
    errorSum += error;
    deltaE = (error - lastError) / 5000;
    //** deltaE = (lastError - error)/-500? How do u get the value of the error
    //at a given time IN EQUAL INTERVALS)??
    motorspeed = (error * kP) + (errorSum * kI) + (deltaE * kD);

    BackLeftMotor.spin(directionType::fwd, motorspeed, velocityUnits::pct);
    FrontLeftMotor.spin(directionType::fwd, motorspeed, velocityUnits::pct);
    FrontRightMotor.spin(directionType::fwd, motorspeed, velocityUnits::pct);
    BackRightMotor.spin(directionType::fwd, motorspeed, velocityUnits::pct);

    lastError = error;
    
    if (fabs(error) > maxAllowedError) {
      errorTimer.clear();
    } else {
      if (errorTimer.time() > errorTimerMax) {
        timerExpired = true;
      }
    }
    vex::task::sleep(20);
  }
  BackLeftMotor.stop(brake);
  FrontLeftMotor.stop(brake);
  FrontRightMotor.stop(brake);
  BackRightMotor.stop(brake);
}

#define MAX_DISTANCE_ERROR  1.0
#define ADJUST_SPEED  40

void goFwdUltrasound(float distance, float velocity){
    float error = 0;
    float currentReading = ultrasonic.distance(distanceUnits::in);
    float targetReading = currentReading + distance;

    goFwd(distance, velocity);
    vex::task::sleep(25);
    
    currentReading = ultrasonic.distance(distanceUnits::in);
    error = targetReading - currentReading;
    if ( abs(error) > MAX_DISTANCE_ERROR ){
      if (error > 0)
        goFwd(error, ADJUST_SPEED);
      else {
        goBack(-error, ADJUST_SPEED);
      }
    }
}


void goBackUltrasound(float distance, float velocity){
    float error = 0;
    float currentReading = ultrasonic.distance(distanceUnits::in);
    float targetReading = currentReading - distance;

    goBack(distance, velocity);
    vex::task::sleep(25);
    
    currentReading = ultrasonic.distance(distanceUnits::in);
    error = targetReading - currentReading;
    if ( abs(error) > MAX_DISTANCE_ERROR ){
      if (error > 0)
        goFwd(error, ADJUST_SPEED);
      else {
        goBack(-error, ADJUST_SPEED);
      }
    }
}

void goDistanceFromBackwall(float targetDistance){
    float error = 0;
    float currentReading = ultrasonic.distance(distanceUnits::in);

    error = targetDistance - currentReading;

  mainControl.Screen.newLine();
  mainControl.Screen.print("C %3.2f, E %3.2f", currentReading, error); 

    if ( abs(error) > MAX_DISTANCE_ERROR ){
      if (error > 0)
        goFwd(error, ADJUST_SPEED);
      else {
        goBack(-error, ADJUST_SPEED);
      }
    }

}
void stopDriveTrain(void) {
  BackLeftMotor.stop();
  FrontLeftMotor.stop();
  FrontRightMotor.stop();
  BackRightMotor.stop();
}

void turnLeft(float degree){
    float numberOfRevs = degree * REVTOANGLELEFT;
    setDrivetoBrakeType(hold);
    BackLeftMotor.rotateFor(directionType::rev, numberOfRevs,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(directionType::rev, numberOfRevs,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(numberOfRevs,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    BackRightMotor.rotateFor(numberOfRevs,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct);
    setDrivetoBrakeType(brake);
}

void turnLeftSlow(float degree){
    float numberOfRevs = degree * REVTOANGLELEFT;
    setDrivetoBrakeType(hold);
    BackLeftMotor.rotateFor(directionType::rev, numberOfRevs,rotationUnits::rev,TURNING_SPEED/2,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(directionType::rev, numberOfRevs,rotationUnits::rev,TURNING_SPEED/2,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(numberOfRevs,rotationUnits::rev,TURNING_SPEED/2,velocityUnits::pct, false);
    BackRightMotor.rotateFor(numberOfRevs,rotationUnits::rev,TURNING_SPEED/2,velocityUnits::pct);
    setDrivetoBrakeType(brake);
}

void turnRight(float degree){
    float numberRev = degree * REVTOANGLERIGHT;
    setDrivetoBrakeType(hold);
    BackLeftMotor.rotateFor(numberRev,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(numberRev,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(directionType::rev, numberRev,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    BackRightMotor.rotateFor(directionType::rev, numberRev,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct);
    setDrivetoBrakeType(brake);

}

void turnRightSlow(float degree){
    float numberRev = degree * REVTOANGLERIGHT;
    setDrivetoBrakeType(hold);
    BackLeftMotor.rotateFor(numberRev,rotationUnits::rev,TURNING_SPEED/2,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(numberRev,rotationUnits::rev,TURNING_SPEED/2,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(directionType::rev, numberRev,rotationUnits::rev,TURNING_SPEED/2,velocityUnits::pct, false);
    BackRightMotor.rotateFor(directionType::rev, numberRev,rotationUnits::rev,TURNING_SPEED/2,velocityUnits::pct);
    setDrivetoBrakeType(brake);

}

#define MAX_TURNING_ERROR 1.5 //1.0,1.1

// DONT USE THIS. NEEDS TO BE FIXED
void turnLeftGyro(float degree){
    float targetHeading = 0;
    float error = 0;
    float currentHeading = InertialSensor.heading(rotationUnits::deg);
    error = currentHeading - degree;  // Turning left is negative
    
    mainControl.Screen.clearScreen();
    mainControl.Screen.setCursor(1,1);
    mainControl.Screen.print("tLG c%3f er%3f", currentHeading, error);


    turnLeft(error);
    vex::task::sleep(50);
    currentHeading = InertialSensor.heading(rotationUnits::deg);
    error = currentHeading - degree;

    mainControl.Screen.setCursor(2,1);
    mainControl.Screen.print("tLG1 c%3f er%3f", currentHeading, error);

    if (abs(error) > MAX_TURNING_ERROR){
      if (error > 0) {        // +ve error means turned too much left
        turnRight(-error);
      }
      else{
        turnLeft(error);
      }
    }
}

// DONT USE THIS. NEEDS TO BE FIXED
void turnRightGyro(float degree){
    float targetHeading = 0;
    float error = 0;
    float currentHeading = InertialSensor.heading(rotationUnits::deg);
    targetHeading = currentHeading + degree;  // Turning right is positive
  
    turnRight(degree);
    vex::task::sleep(25);
    currentHeading = InertialSensor.heading(rotationUnits::deg);
    error = targetHeading - currentHeading;
    if (fabs(error) > MAX_TURNING_ERROR){
      if (error > 0) {        // +ve error means not turned sufficient right
        turnRight(error);
      }
      else{
        turnLeft(-error);
      }
    }
}

void alignToHeading(float targetHeading){
    int count = 0;
    float error = 0;
    float currentHeading = InertialSensor.heading(rotationUnits::deg);
    error = targetHeading - currentHeading;
    if (error < -315) {
      error = error + 360;  //modulo 360
    }

    if (fabs(error) > 45) {
      Brain.Screen.setCursor(1, 1);
      Brain.Screen.print("aToHd c%f er%f", currentHeading, error);
    //  return;
    }

    while ((fabs(error) > MAX_TURNING_ERROR) && count++ < 3) {
      if (error > 0) {        // +ve error means not turned sufficient right
        turnRightSlow(error);
      }
      else{
        turnLeftSlow(-error);
      }
      wait(25, msec);
      currentHeading = InertialSensor.heading(rotationUnits::deg);
      error = targetHeading - currentHeading;
      if (error < -315) {
        error = error + 360;  //modulo 360
      }
    }
}

#define SWING_SPEED 100
void swingLeft(float distance, float LeftRightRatio){
    float numberDeg;
    numberDeg = (distance/WHEELCIRCUMFERENCE)*360;
    BackLeftMotor.rotateFor(numberDeg*LeftRightRatio,rotationUnits::deg,SWING_SPEED*LeftRightRatio,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(numberDeg*LeftRightRatio,rotationUnits::deg,SWING_SPEED*LeftRightRatio,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(numberDeg,rotationUnits::deg,SWING_SPEED,velocityUnits::pct, false);
    BackRightMotor.rotateFor(numberDeg,rotationUnits::deg,SWING_SPEED,velocityUnits::pct);

}

void swingRight(float distance, float LeftRightRatio){
    float numberDeg;
    numberDeg = (distance/WHEELCIRCUMFERENCE)*360;
    BackRightMotor.rotateFor(numberDeg*LeftRightRatio,rotationUnits::deg,SWING_SPEED*LeftRightRatio,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(numberDeg*LeftRightRatio,rotationUnits::deg,SWING_SPEED*LeftRightRatio,velocityUnits::pct, false);
    FrontLeftMotor.rotateFor(numberDeg,rotationUnits::deg,SWING_SPEED,velocityUnits::pct, false);
    BackLeftMotor.rotateFor(numberDeg,rotationUnits::deg,SWING_SPEED,velocityUnits::pct);

}

void turnRightArc(float degree){
    float numberRev = 2*degree * REVTOANGLERIGHT;
//    BackLeftMotor.rotateFor(numberRev,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
//    FrontLeftMotor.rotateFor(numberRev,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    FrontRightMotor.rotateFor(directionType::rev, numberRev,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct, false);
    BackRightMotor.rotateFor(directionType::rev, numberRev,rotationUnits::rev,TURNING_SPEED,velocityUnits::pct);

}
// Functions: Scoring
// Intake Functions
void intakeStart() {
    LeftIntake.spin(vex::directionType::fwd, INTAKESPD, vex::velocityUnits::pct);
    RightIntake.spin(vex::directionType::fwd, INTAKESPD, vex::velocityUnits::pct);
    IntakeRunning = 1;
}

void intakeStartSlow() {
    LeftIntake.spin(vex::directionType::fwd, INTAKESPD*0.7, vex::velocityUnits::pct);
    RightIntake.spin(vex::directionType::fwd, INTAKESPD*0.7, vex::velocityUnits::pct);
    IntakeRunning = 1;
}

void intakeStartSlowDrive() {
  if (IntakeRunning == 0){
    LeftIntake.spin(vex::directionType::fwd, INTAKESPD*0.7, vex::velocityUnits::pct);
    RightIntake.spin(vex::directionType::fwd, INTAKESPD*0.7, vex::velocityUnits::pct);
    IntakeRunning = 1;
  }
  else{
    LeftIntake.stop();
    RightIntake.stop();
  }
}

void intakeStop() {
  LeftIntake.stop();
  RightIntake.stop();
  IntakeRunning = 0;
}

void intakeToggleDriver(){
  if (IntakeRunning == 0){
    intakeStart();
    IntakeRunning = 1;
  }
  else{
    intakeStop();
    IntakeRunning = 0;
  }
}

void intakeAndRoller (){
  if (IntakeRunning == 0){
    intakeStart();
    rollerStartDrv();
  }
  else{
    intakeStop();
    // rollerStop();
  }
}

void outtakeAndRollerDown(){
  if (IntakeRunning == 0){
    outtake();
    rollerDown();
  }
  else{
    intakeStop();
    rollerStop();
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

void outtakeSlow() {
    if (IntakeRunning==0){
      LeftIntake.spin(vex::directionType::rev, INTAKESPD*0.4, vex::velocityUnits::pct);
      RightIntake.spin(vex::directionType::rev, INTAKESPD*0.4, vex::velocityUnits::pct);
      IntakeRunning = 1;
    }
    else{
      LeftIntake.stop();
      RightIntake.stop();
      IntakeRunning = 0;
    }
}


void intakeBalls(int numberOfBalls){
  int presses = 0;
  int targetPresses = numberOfBalls;
  while (presses < targetPresses){
    intakeStart();
    wait(250, msec);
  }
}

// Roller Functions
void rollerStart(){
  Roller.spin(vex::directionType::fwd, ROLLERSPD, vex::velocityUnits::pct);
  RollerRunning = 1;
}

void rollerStartDrv(){
  Roller.spin(vex::directionType::fwd, ROLLERSPDDRV, vex::velocityUnits::pct);
  RollerRunning = 1;
}

void rollerStartSlow(){
  Roller.spin(vex::directionType::fwd, ROLLERSPD/2, vex::velocityUnits::pct);
  RollerRunning = 1;
}

void rollerStop(){
  Roller.stop();
  RollerRunning = 0;
}

void rollerUp(){
  if (RollerRunning == 0){
    rollerStart();
  }
  else{
    rollerStop();
  }
}

void rollerUpDrv(){
  if (RollerRunning == 0){
    rollerStartDrv();
  }
  else{
    rollerStop();
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

void rollerDownSlow(){
  Roller.spin(vex::directionType::rev, ROLLERSPD/2, vex::velocityUnits::pct);
  RollerRunning = 1;
}

void rollerDownDrv(){
  if (RollerRunning == 0){
    Roller.spin(vex::directionType::rev, ROLLERSPDDRV, vex::velocityUnits::pct);
    RollerRunning = 1;
  }
  else{
    Roller.stop();
    RollerRunning = 0;
  }
}
void shootStart(){
  Shooter.spin(vex::directionType::fwd, SHOOTERSPD, vex::velocityUnits::pct);
  ShooterRunning = 1;

}

void shootStop(){
  Shooter.stop();
  ShooterRunning = 0;
}

void prepForLimShootLoaded() {
  int i = 0;
  rollerStartSlow();
  while (!lim.pressing() && (i<150)){
    i++;
    wait(10, msec);
  }
  wait(25, msec);

  rollerStop();
}
void limShootLoaded(){
  int i = 0;
  shootStart();
  wait (100, msec);
  rollerStart();

  while(!launched && (i<20)){
    wait(50, msec);  
    i = i+1;
  }
  wait(125, msec);

  rollerStop();
}

void limShootLoadedPreStart() {
  int i = 0;
  resetLaunchStatus();
  wait(50,msec);
  rollerStart();

  while(!launched && (i<30)){
    wait(50, msec);  
    i = i+1;
  }
  if (!launched) {
    mainControl.Screen.setCursor(1,1);
    mainControl.Screen.print("WRN2:BallNotLaunched");
  }
  wait(80, msec);  //0205 125 to 100
  rollerStop();
}

//test
void limShootnew(){
  int i = 0;
  shootStart();
  wait (100, msec);
  rollerStart();

  while(!launched && (i<20)){
    wait(100, msec);  
    i = i+1;
  }
}

void limShoot(){
  // shootStart();
  int i = 0;
  int preL = launched;  // prior state of launched
  mainControl.Screen.clearScreen();
  mainControl.Screen.setCursor(1,1);

  shootStart();
  while(!launched && (i<45)){
    wait(50, msec);  
    i = i+1;
  }
  mainControl.Screen.newLine();
  mainControl.Screen.print("limSt %d, pl %d lncd %d", i, preL, launched); 
  wait(50, msec);              //50     
  shootStop();
}

void shoot(){
 if (ShooterRunning == 0){
     Shooter.spin(directionType::fwd, SHOOTERSPD*0.75, velocityUnits::pct);
     ShooterRunning = 1;
   }
   else{
     shootStop();
   }
}

void shootDriveSlow(){
 if (ShooterRunning == 0){
     Shooter.spin(vex::directionType::fwd, 0.68*SHOOTERSPD, vex::velocityUnits::pct);
     ShooterRunning = 1;
   }
  else{
    shootStop();
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

void flipOpen(){
  rollerDown();
  vex::task::sleep(150);
  rollerStop();
}

void flipOpenStart(){
  rollerDownSlow();
}

void flipOpenStop(){
  rollerStop();
}



