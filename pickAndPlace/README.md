Operational Control with iCub
=============================

# Prerequisites
By now, you should be an expert of the following components :wink::
- [Joint Control](http://wiki.icub.org/iCub/main/dox/html/icub_motor_control_tutorial.html).
- [Cartesian Control](http://wiki.icub.org/iCub/main/dox/html/icub_cartesian_interface.html).
- [Gaze Control](http://wiki.icub.org/iCub/main/dox/html/icub_gaze_interface.html).

# Assignment
We want you to develop a module that employs the `Joint Interface`, the `Cartesian Interface`
and the `Gaze Interface` to accomplish the following tasks:

1. Make iCub **look down at the table**.
1. **Detect the object** lying on the table.
1. Retrieve the **object position** in the Cartesian domain.
1. Let iCub **look at the object**.
1. Based on the object position, **select the best hand**.
1. **Control the iCub fingers** to achieve suitable grasp configurations.
1. Let iCub **approach the object** to enable a top grasp.
1. Ask iCub to **grasp the object**.
1. **Lift the object**.

This assignment is peculiar in such a way that **we can interface the simulator
and the real robot indifferently**, being the code capable of dealing with object
location retrieval in both conditions, transparently to the user.

The outcome should look like the animation below with the simulator:

![grasp-it](/misc/grasp-it.gif)

Well, it comes out that grasping a rigid sphere in a simulated environment
is not that easy :blush: since dynamics rules here.
Anyway, the same code running on the real robot should perform way better,
as for example in the video below:

[![real-robot](/misc/real-robot.png)](https://www.youtube.com/watch?v=rITQlGuXXOw)

Some of the points reported above have been already addressed in the code (e.g. [**object detection and location retrieval**](./src/main.cpp#L182)), so you need to **fill in the missing gaps** highlighted by the comment `// FILL IN THE CODE`.

:warning: Don't speed up the movements by increasing the trajectory time of the operational controllers: they're already all set to work with the real robot.

Once done, you can test your code in two ways along with the simulator:

1. **Manually**: running the _yarpmanager scripts_ provided from within [**app/scripts**](./app/scripts) and yielding corresponding commands to the _module rpc port_.
1. **Automatically**: [running the script **test.sh**](https://github.com/vvv-school/vvv-school.github.io/blob/master/instructions/how-to-run-smoke-tests.md) in the **smoke-test** directory. Take into account these important points:
    1. We use a **timeout of _240 s_** to check the status of rpc communication, meaning that you have _240 s_ max to accomplish each rpc command.
    1. When you reply to rpc commands, we assume the robot has **finished the movement** (e.g. see [_this line_](./src/main.cpp#L361) in the `respond` method).
    1. The smoke-test will add a random displacement to the initial position of the ball in order to force the use of both hands :wink:

If you pass the test on the simulator, **book the robot** to get a real experience!

## Notes
- To make the blue ball show up within the simulator, you have to turn on the
flag **RENDER::objects** in the [**`iCub_parts_activation.ini`**](https://github.com/robotology/icub-main/blob/master/app/simConfig/conf/iCub_parts_activation.ini#L28) file.

    To do so, follow these steps (the **smoke-test** does them for you):
    ```sh
    # import the file for customization
    $ yarp-config context --import simConfig iCub_parts_activation.ini

    # open the file
    $ gedit ~/.local/share/yarp/contexts/simConfig/iCub_parts_activation.ini
    ```
    Now, edit the file by setting the option **objects** equal to **on** (under the group _RENDER_).

    Also, you might find this [**resource**](https://github.com/robotology/QA/issues/42) quite useful to get accustomed with configuration files in Yarp :smiley:
- Alternatively, you can use this [**model**](https://github.com/robotology-playground/icub-gazebo-wholebody/tree/master/worlds/iCub_and_Table) within **Gazebo**. Be careful, the **smoke-test** does work only with iCub_SIM.

# [How to complete the assignment](https://github.com/vvv-school/vvv-school.github.io/blob/master/instructions/how-to-complete-assignments.md)
