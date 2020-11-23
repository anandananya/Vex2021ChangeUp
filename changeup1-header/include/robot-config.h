using namespace vex;

extern brain Brain;

// VEXcode devices
extern motor BackLeftMotor;
extern motor BackRightMotor;
extern motor FrontLeftMotor;
extern motor FrontRightMotor;
extern motor LeftIntake;
extern motor RightIntake;
extern motor Roller;
extern motor Shooter;
extern controller mainControl;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void  vexcodeInit( void );