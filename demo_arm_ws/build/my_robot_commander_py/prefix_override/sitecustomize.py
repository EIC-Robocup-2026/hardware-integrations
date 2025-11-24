import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/ryu/hardware-integrations/demo_arm_ws/install/my_robot_commander_py'
