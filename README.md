# RobotSim
UE4 connect RosBridge eachohter with tcp

UE4模拟ROS的机械臂数据，利用TCP通过Rosbridge进行数据通信

资源的导入 

一： 

kr16_2机械臂的fbx文件使用第三方软件导出,例如win10自带的画图软件

kr16_2机械臂资源的原始git地址：https://github.com/ros-industrial/kuka_experimental

二：

使用URoboSim插件 + UE4 4.18（URoboSim目前不支持更高版本的引擎）版本，将kr16_2的URDF文件拖入编辑器，并勾选合并选项,选择全部导入，将步骤一的fbx全部导入到编辑器。

三：

将UE4 4.18引擎编辑器中导入的机械臂资源(migrate)迁入到目标高版本中，这里我们使用的是4.24.3

说明：如果不使用4.18版本先行导入URDF，而直接使用高版本的版本导入，会出现导入后的机械臂零件mesh表面出现破口的问题。所以建议先用4.18过渡下。产生这个问题的原因是因为

UE4 高版本已经丢弃了以前封装的fbx导入函数,使用了新的接口，但是感觉效果并不好。很奇怪。

URoboSim的git地址：https://github.com/robcog-iai/UROSBridge


后续我会将这个改成全自动化的导入URDF，并且支持高版本的引擎。 

work on UE4 4.24.3
