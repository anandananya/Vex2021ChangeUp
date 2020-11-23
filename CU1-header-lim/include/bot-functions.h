#define TURNING_SPEED       35
#define SLOW_TURNING_SPEED  20
#define WHEELDIAMETER       4.0      // Wheel diameter in inches
#define WHEELCIRCUMFERENCE  (WHEELDIAMETER * 3.145)
#define REVTOANGLE      (1.0/90.0)   //1 rotations to turn 90 degrees
#define INTAKESPD       80
#define ROLLERSPD       100
#define SHOOTERSPD      100
#define TURN_WEIGHT     0.65  // Reduce turning sensitivity when driving. Forward/backward are not affected
#define DRIVE_WEIGHT    0.8  // Reduce driving sensitivity when driving. Forward/backward are not affected

extern int IntakeRunning;
extern int RollerRunning;
extern int ShooterRunning;

void goFwd(float distance, float velocity);
void slideFwd(float distance, float velocity);
void goBack(float distance, float velocity);
void stopDriveTrain(void);
void turnLeft(float degree);
void turnRight(float degree);
void intakeStart();
void intakeStop();
void intake ();
void outtake();
void rollerStart();
void rollerStop();
void rollerUp();
void rollerDown();
void shootStart();
void shootStop();
void shoot();
void shootBack();
void flipOpen();
