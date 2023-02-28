/// @brief
/// @return

// Module type definitions:
#define SERVOMODTYPE 0
#define ADCMODTYPE 1
#define IOMODTYPE 2
#define STEPMODTYPE 3

#define MOVE_DONE 0x01 // set when move done (trap. pos mode), when goal
					   // vel. has been reached (vel mode) or when not servoing
// Servo Module LOAD_TRAJ control byte bit definitions:
#define LOAD_POS 0x01	  //+4 bytes
#define LOAD_VEL 0x02	  //+4 bytes
#define LOAD_ACC 0x04	  //+4 bytes
#define LOAD_PWM 0x08	  //+1 byte
#define ENABLE_SERVO 0x10 // 1 = servo mode, 0 = PWM mode
#define VEL_MODE 0x20	  // 1 = velocity mode, 0 = trap. position mode
#define REVERSE 0x40	  // 1 = command neg. PWM or vel, 0 = positive
#define MOVE_REL 0x40	  // 1 = move relative, 0 = move absolute
#define START_NOW 0x80	  // 1 = start now, 0 = wait for START_MOVE command

// Servo Module STOP_MOTOR control byte bit definitions:
#define AMP_ENABLE 0x01	 // 1 = raise amp enable output, 0 = lower amp enable
#define MOTOR_OFF 0x02	 // set to turn motor off
#define STOP_ABRUPT 0x04 // set to stop motor immediately
#define STOP_SMOOTH 0x08 // set to decellerate motor smoothly
#define STOP_HERE 0x10	 // set to stop at position (4 add'l data bytes required)
#define ADV_FEATURE 0x20 // enable features in ver. CMC

typedef long long int (*Func_NmcInit)(const char *, unsigned int);
typedef long long int (*Func_NmcGetModType)(byte addr);
typedef long long int (*Func_NmcNoOp)(byte addr);
typedef long long int (*Func_NmcGetStat)(byte addr);
typedef long long int (*Func_NmcShutdown)(void);

typedef long long int (*Func_ServoResetPos)(byte addr);
typedef long long int (*Func_ServoStopMotor)(byte addr, byte mode);
typedef long long int (*Func_ServoLoadTraj)(byte addr, byte mode, long pos, long vel, long acc, byte pwm);
typedef long long int (*Func_ServoSetGain)(byte addr, short int kp, short int kd, short int ki,
										   short int il, byte ol, byte cl, short int el,
										   byte sr, byte dc);

extern "C" Func_NmcInit LoadNmcInit(HINSTANCE hModule);
extern "C" Func_NmcGetModType LoadNmcGetModType(HINSTANCE hModule);
extern "C" Func_NmcNoOp LoadNmcNoOp(HINSTANCE hModule);
extern "C" Func_NmcGetStat LoadNmcGetStat(HINSTANCE hModule);
extern "C" Func_NmcShutdown LoadNmcShutdown(HINSTANCE hModule);

extern "C" Func_ServoResetPos LoadServoResetPos(HINSTANCE hModule);
extern "C" Func_ServoStopMotor LoadServoStopMotor(HINSTANCE hModule);
extern "C" Func_ServoLoadTraj LoadServoLoadTraj(HINSTANCE hModule);
extern "C" Func_ServoSetGain LoadServoSetGain(HINSTANCE hModule);
