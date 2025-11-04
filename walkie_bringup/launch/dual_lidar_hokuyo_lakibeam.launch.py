#!/usr/bin/env python3
import os
import yaml
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import (EmitEvent, RegisterEventHandler)
from launch.event_handlers import OnProcessStart
from launch.events import matches_action
from launch_ros.actions import Node, LifecycleNode
from launch_ros.event_handlers import OnStateTransition
from launch_ros.events.lifecycle import ChangeState
from lifecycle_msgs.msg import Transition


def generate_launch_description():
    # ========== Dual Lidar Configuration ==========
    # Single parameter file path for both lidars
    dual_lidar_config_file_path = os.path.join(
        get_package_share_directory('robot_bringup'),
        'config', 'lidar',
        'dual_lidar_config.yml'
    )

    # Load configuration file
    with open(dual_lidar_config_file_path, 'r') as file:
        lidar_config = yaml.safe_load(file)

    # Extract Hokuyo parameters
    hokuyo_config_params = lidar_config['urg_node2']['ros__parameters']
    hokuyo_launch_config = lidar_config['urg_node2']['launch_config']
    
    # Extract Lakibeam parameters
    lakibeam_config_params = lidar_config['lakibeam_lidar']['ros__parameters']

    # Hokuyo URG lifecycle node
    hokuyo_lifecycle_node = LifecycleNode(
        package='urg_node2',
        executable='urg_node2_node',
        name=hokuyo_launch_config['node_name'],
        remappings=[('scan', hokuyo_launch_config['scan_topic'])],
        parameters=[hokuyo_config_params],
        namespace='',
        output='screen',
    )

    # Hokuyo: Unconfigure -> Inactive transition
    hokuyo_configure_event_handler = RegisterEventHandler(
        event_handler=OnProcessStart(
            target_action=hokuyo_lifecycle_node,
            on_start=[
                EmitEvent(
                    event=ChangeState(
                        lifecycle_node_matcher=matches_action(hokuyo_lifecycle_node),
                        transition_id=Transition.TRANSITION_CONFIGURE,
                    ),
                ),
            ],
        ),
    )

    # Hokuyo: Inactive -> Active transition
    hokuyo_activate_event_handler = RegisterEventHandler(
        event_handler=OnStateTransition(
            target_lifecycle_node=hokuyo_lifecycle_node,
            start_state='configuring',
            goal_state='inactive',
            entities=[
                EmitEvent(
                    event=ChangeState(
                        lifecycle_node_matcher=matches_action(hokuyo_lifecycle_node),
                        transition_id=Transition.TRANSITION_ACTIVATE,
                    ),
                ),
            ],
        ),
    )

    # ========== Lakibeam Configuration ==========
    # Lakibeam node
    lakibeam_node = Node(
        package='lakibeam1',
        name='lakibeam_lidar_node',
        executable='lakibeam1_scan_node',
        parameters=[lakibeam_config_params],
        output='screen'
    )

    # ========== Launch Description ==========
    ld = LaunchDescription()
    
    # Nodes
    ld.add_action(hokuyo_lifecycle_node)
    ld.add_action(lakibeam_node)
    
    # Event handlers for Hokuyo lifecycle (only if auto_start is true)
    if hokuyo_launch_config['auto_start']:
        ld.add_action(hokuyo_configure_event_handler)
        ld.add_action(hokuyo_activate_event_handler)
    
    return ld

