#define TURNING_SPEED       90 //45
#define SLOW_TURNING_SPEED  20
#define WHEELDIAMETER       4.0      // Wheel diameter in inches
#define WHEELCIRCUMFERENCE  (WHEELDIAMETER * 3.145)
#define REVTOANGLERIGHT      (1.0/89.5)   //1 rotations to turn 93.5 degrees
#define REVTOANGLELEFT      (1.0/87.5)   //1 rotations to turn 91.25 degrees 87.5
#define INTAKESPD       100
#define ROLLERSPD       100
#define ROLLERSPDDRV    80
#define SHOOTERSPD      85 // 85 worked for the longest time ;-;
#define TURN_WEIGHT     0.75  // Reduce turning sensitivity when driving. Forward/backward are not affected //april 11 was .6
#define DRIVE_WEIGHT    0.9  // Reduce driving sensitivity when driving. Forward/backward are not affected ap 11 .9

extern int IntakeRunning;
extern int RollerRunning;
extern int ShooterRunning;
extern int lim_pressed;
extern int launched;

void setDrivetoBrakeType(vex::brakeType brakeValue);
void goFwd(float distance, float velocity);
void slideFwd(float distance, float velocity);
void slideFwdNonBlock(float distance, float velocity);
void slideBackNonBlock(float distance, float velocity);
void setDrivetoBrake(void);

void goBack(float distance, float velocity);
void goFwdUltrasound(float distance, float velocity);
void goDistanceFromBackwall(float distance);

void stopDriveTrain(void);
void turnLeft(float degree);
void swingLeft(float distance, float LeftRightRatio);
void swingRight(float distance, float LeftRightRatio);
void turnRight(float degree);
void turnLeftGyro(float degree);
void turnRightGyro(float degree);
void turnRightArc(float degree);
void alignToHeading(float targetHeading);
void intakeStart();
void intakeStartSlow();
void intakeStartSlowDrive();
void intakeToggleDriver();
void intakeStop();
void intakeAndRoller ();
void outtakeAndRollerDown();
void outtake();
void outtakeSlow();
void intakeBalls(int numberOfBalls);
void rollerStart();
void rollerStartDrv();
void rollerStartSlow();
void rollerStop();
void rollerUp();
void rollerUpDrv();
void rollerDown();
void rollerDownDrv();
void shootStart();
void shootStop();
void resetLaunchStatus();
void prepForLimShootLoaded();
void limShootLoaded();
void limShootLoadedPreStart();
void limShoot();
void shoot();
void shootBack();
void flipOpen();
void flipOpenStart();
void flipOpenStop();
void limShootnew();
void shootDriveSlow();
void drivePI(double targetDistance);