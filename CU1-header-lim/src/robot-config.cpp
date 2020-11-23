#include "vex.h"

using namespace vex;
using signature = vision::signature;
using code = vision::code;

// A global instance of brain used for printing to the V5 Brain screen
brain  Brain;

// VEXcode device constructors
motor BackLeftMotor = motor(PORT20, ratio18_1, false);
motor BackRightMotor = motor(PORT2, ratio18_1, true);
motor FrontLeftMotor = motor(PORT5, ratio18_1, false);
motor FrontRightMotor = motor(PORT16, ratio18_1, true);
motor LeftIntake = motor(PORT1, ratio18_1, false);
motor RightIntake = motor(PORT3, ratio18_1, true);
motor Roller = motor(PORT8, ratio18_1, true);
motor Shooter = motor(PORT11, ratio18_1, true);
limit lim = limit(Brain.ThreeWirePort.H);
controller mainControl = controller(primary);

// VEXcode generated functions
// define variable for remote controller enable/disable
bool RemoteControlCodeEnabled = true;

/**
 * Used to initialize code/tasks/devices added using tools in VEXcode Pro.
 * 
 * This should be called at the start of your int main function.
 */
void vexcodeInit( void ) {
  // nothing to initialize
}