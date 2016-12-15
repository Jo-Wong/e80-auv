# e80-auv
Arduino project for the Teensy microcontroller used for the HMC E80 AUV

Copy this folder into the Arduino libraries folder.

The main arduino sketch is in the auv_controller folder.


## Main Classes

### Logger

The `Logger` class handles all the logging. It is written in a way to hide away the intricacies of logging the heterogenous set of variables a user may want to log.

In the main program's setup function, the `Logger` object's `include` function is run, passing pointers to any `DataSource` objects that should be logged. 

Next, the `Logger` object's `init` function is run, which automatically finds an unused filename. Each log creates two files, `INFXXX.txt` and `LOGXXX.bin`, where the `XXX` is the smallest number for which a file with the same name doesn't already exist on the SD card. 

The `INF` file describes the schema of the data being logged. It's first line contains comma separated column names, and its second line contains comma separated column data types. The `LOG` file contains the raw binary values of each variable being logged, with each row of data written in the order described by the `INF` file.

The logging actually happens through two functions: `log` and `write`;

The `log` function samples all the `DataSource` objects that were registered to the `Logger` during setup by the `include` function. It calls the `writeDataBytes` function of each `DataSource`, to write the binary values of the variables to be logged to a buffer. Internally, the `Logger` object keeps two queues of 512 byte buffers, one contained empty buffers and one containing full buffers. The `log` function writes the data to an empty buffer obtained from the empty queue, and passes buffers to the full queue once 512 bytes are written. **Note, the `log` function does NOT actually write anything to the SD card.** The fact that this function only writes to a buffer means it takes a consistantly short amount of time (~750us in practice).

The actual writing to the SD card is handled by the `write` function. This function loops as long as there is space in the LOG file on the SD card available to write. Upon each loop, it checks the full queue for any full 512 byte buffers that need to be written to the SD card. If there is one, it writes it to the SD card. 

**The `Logger` is written such that the main `loop` of the Teensy only calls the `write` function. All other operations that the microcontroller must handle should be placed in a function that is called by a Timer Driven Interrupt (using an `IntervalTimer`, e.g.)** 


### DataSource

`DataSource` is an abstract class representing a source of data to be logged. `DataSource` objects must have a `varNames` String member which contains the names of the columns, a `varTypes` String member which contains the data types of all the columns, a `getCSVString` function which outputs the current data values into a CSV string (though this isn't used anymore), and a `writeDataBytes` function which writes the raw binary data of each variable to the passed in buffer.

### SensorGPS, SensorIMU

These classes are wrappers for the IMU and GPS sensors, offering functions to poll the sensors for new data, and keeping internal state variables holding the latest received data. These classes implement the DataSource interface.

### StateEstimator

The `StateEstimator` class defines an object which handles all the state estimation of the robot. It keeps an internal `state` struct which stores the current estimate of the state. It is `init`ialized with the loop period (in seconds) and the origin gps coordinates.

It includes functions to incorporate data from the IMU and GPS, as well as a function which takes in the current commanded motor drive and applies an forward kinematic model to determine the state of the robot after one loop period.

It also implements the `DataSource` interface to plug into the `Logger` framework.

### PathController

The `PathController` class defines an object that handles switching the waypoint the robot is following. Upon initialization, the object reads in a trajectory from a file called "traj.txt" on the SD card. This file is written such that each line defines a waypoint, in the format:

```
latitude,longitude,orientation
```

The `control` function takes in the current state, and a pointer to the desired waypoint. It updates the desired waypoint if the the distance between the robot and the waypoint is less than a certain success radius.

### VelocityController

The VelocityController's `control` function takes in the current state and desired waypoint, and from them, computes the necessary linear and rotational velocities using P control on the distance error, the error between the heading of the robot and the heading needed to move toward the goal position, and the error between the heading of the robot and the desired heading at the waypoint.

It implements the [controller described on the HMC 190Q website](https://www.hmc.edu/lair/E190Q/E190Q-Lecture04-PointTracking.pdf). 

### MotorController

The `control` function in the MotorController takes in the current state and the desired velocities as commanded by the velocity controller. It then determines what PWM value to send to each of the motors, and sets these speeds in the motorDriver.

These 'speed' values are currently calculated assuming that the dynamics of the robot can be approximated by a simple linear model given in MATLAB syntax as:
```
 [v; w] = A*[rpwm; lpwm]
```
The linear model can be inverted to calculate the motor pwm values:
```
 [rpwm; lpwm] = inv(A)*[v; w]
```

The values of matrix A were estimated by recording a video of the robot motion at various commanded PWM values to each motor, and using point tracking in Adobe After Effects to calculate the trajectories.

The values of both the matrix A and the matrix inv(A) are specified in the `Params.h` file.

### MotorDriver

The `MotorDriver` class defines an object that handles the actual driving of the robot. The public `left` and `right` integer datamembers can be set to numbers from -127 to +127, representing max reverse and forward speeds respectively.

The `apply` function determins what PWM signals to send to which pins based upon these values, and sets up the PWM module to send the appropriate signals. Note that the DC motors on the robot don't turn at all until a certain minimum PWM value, after which the relationship between PWM and thrust is fairly linear. These minimum PWM values are specified in `Params.h` as the `MOTOR_DEADZONE` values. The `apply` function takes this into account, adjusting the PWM values accordingly such that the `left` and `right` numbers relate linearly to the thrust output.

### Params.h

This file `#define`s several constants which are used throughout the codebase.