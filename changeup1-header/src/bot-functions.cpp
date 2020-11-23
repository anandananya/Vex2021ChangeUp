#include "bot-functions.h"
#include "vex.h"

int IntakeRunning = 0;
int RollerRunning = 0;
int ShooterRunning = 0;

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

// Functions: Scoring
// Intake Functions
void intakeStart() {
    LeftIntake.spin(vex::directionType::fwd, INTAKESPD, vex::velocityUnits::pct);
    RightIntake.spin(vex::directionType::fwd, INTAKESPD, vex::velocityUnits::pct);
    IntakeRunning = 1;
}

void intakeStop() {
  LeftIntake.stop();
  RightIntake.stop();
  IntakeRunning = 0;
}

void intakeAndRoller (){
  if (IntakeRunning == 0){
    intakeStart();
    rollerStart();
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

// Roller Functions
void rollerStart(){
  Roller.spin(vex::directionType::fwd, ROLLERSPD, vex::velocityUnits::pct);
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

void shootStart(){
  Shooter.spin(vex::directionType::fwd, SHOOTERSPD, vex::velocityUnits::pct);
  ShooterRunning = 1;
}

void shootStop(){
  Shooter.stop();
  ShooterRunning = 0;
}
void shoot(){
 if (ShooterRunning == 0){
     shootStart();
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
  vex::task::sleep(500);
  rollerDown();
}