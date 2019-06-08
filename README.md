bachelor-thesis
=================

robot typing
--------------
I buyed a better robot arm, it's more stable and more accurate, with this robot I am able to realize funktions like typing. I am still trying to use this robot to do the picking up with viual servo control, but I meet some problems. this robot use a 4 link linkage to reduce the weight of link, but this make the modelling process pretty diffcult, because by ROS the URDF flie dosen't support such kind of colsed linkage structure. 

![](https://github.com/william-in-kit/opencv-robot/blob/master/typing-robot.gif)

visual servo control with a 3 Dof robot arm
-----------------------------------------------
I build this robot with 3 Dof to test visual servo control, the camera was fixed at the second link.There are two examples, one for ping-pong tracking, another is visual servo control with obstacle avoidance. 

visual servo control with obstacle avoidance
---------------------------------------------
![visual servo control with obstacle avoidanc](https://github.com/william-in-kit/opencv-robot/blob/master/visual_servo_control.gif)

pingpong-tracking
-------------------
![pingpong-tracking](https://github.com/william-in-kit/opencv-robot/blob/master/pingpong-tracking.gif)


