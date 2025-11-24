// generated from rosidl_generator_c/resource/idl__description.c.em
// with input from my_robot_interfaces:msg/PoseCommand.idl
// generated code does not contain a copyright notice

#include "my_robot_interfaces/msg/detail/pose_command__functions.h"

ROSIDL_GENERATOR_C_PUBLIC_my_robot_interfaces
const rosidl_type_hash_t *
my_robot_interfaces__msg__PoseCommand__get_type_hash(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_type_hash_t hash = {1, {
      0xa1, 0x39, 0x14, 0xb8, 0x97, 0x73, 0xe1, 0x2f,
      0x96, 0x81, 0xd4, 0x2b, 0x61, 0x41, 0x86, 0x5f,
      0x8c, 0x62, 0xfa, 0xac, 0x14, 0x38, 0x09, 0x9f,
      0xc0, 0xe4, 0x0c, 0xaf, 0x49, 0x4d, 0x0e, 0x53,
    }};
  return &hash;
}

#include <assert.h>
#include <string.h>

// Include directives for referenced types

// Hashes for external referenced types
#ifndef NDEBUG
#endif

static char my_robot_interfaces__msg__PoseCommand__TYPE_NAME[] = "my_robot_interfaces/msg/PoseCommand";

// Define type names, field names, and default values
static char my_robot_interfaces__msg__PoseCommand__FIELD_NAME__x[] = "x";
static char my_robot_interfaces__msg__PoseCommand__FIELD_NAME__y[] = "y";
static char my_robot_interfaces__msg__PoseCommand__FIELD_NAME__z[] = "z";
static char my_robot_interfaces__msg__PoseCommand__FIELD_NAME__roll[] = "roll";
static char my_robot_interfaces__msg__PoseCommand__FIELD_NAME__pitch[] = "pitch";
static char my_robot_interfaces__msg__PoseCommand__FIELD_NAME__yaw[] = "yaw";
static char my_robot_interfaces__msg__PoseCommand__FIELD_NAME__cartesian_path[] = "cartesian_path";

static rosidl_runtime_c__type_description__Field my_robot_interfaces__msg__PoseCommand__FIELDS[] = {
  {
    {my_robot_interfaces__msg__PoseCommand__FIELD_NAME__x, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {my_robot_interfaces__msg__PoseCommand__FIELD_NAME__y, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {my_robot_interfaces__msg__PoseCommand__FIELD_NAME__z, 1, 1},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {my_robot_interfaces__msg__PoseCommand__FIELD_NAME__roll, 4, 4},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {my_robot_interfaces__msg__PoseCommand__FIELD_NAME__pitch, 5, 5},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {my_robot_interfaces__msg__PoseCommand__FIELD_NAME__yaw, 3, 3},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_DOUBLE,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
  {
    {my_robot_interfaces__msg__PoseCommand__FIELD_NAME__cartesian_path, 14, 14},
    {
      rosidl_runtime_c__type_description__FieldType__FIELD_TYPE_BOOLEAN,
      0,
      0,
      {NULL, 0, 0},
    },
    {NULL, 0, 0},
  },
};

const rosidl_runtime_c__type_description__TypeDescription *
my_robot_interfaces__msg__PoseCommand__get_type_description(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static bool constructed = false;
  static const rosidl_runtime_c__type_description__TypeDescription description = {
    {
      {my_robot_interfaces__msg__PoseCommand__TYPE_NAME, 35, 35},
      {my_robot_interfaces__msg__PoseCommand__FIELDS, 7, 7},
    },
    {NULL, 0, 0},
  };
  if (!constructed) {
    constructed = true;
  }
  return &description;
}

static char toplevel_type_raw_source[] =
  "float64 x\n"
  "float64 y\n"
  "float64 z\n"
  "float64 roll\n"
  "float64 pitch\n"
  "float64 yaw\n"
  "bool cartesian_path";

static char msg_encoding[] = "msg";

// Define all individual source functions

const rosidl_runtime_c__type_description__TypeSource *
my_robot_interfaces__msg__PoseCommand__get_individual_type_description_source(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static const rosidl_runtime_c__type_description__TypeSource source = {
    {my_robot_interfaces__msg__PoseCommand__TYPE_NAME, 35, 35},
    {msg_encoding, 3, 3},
    {toplevel_type_raw_source, 88, 88},
  };
  return &source;
}

const rosidl_runtime_c__type_description__TypeSource__Sequence *
my_robot_interfaces__msg__PoseCommand__get_type_description_sources(
  const rosidl_message_type_support_t * type_support)
{
  (void)type_support;
  static rosidl_runtime_c__type_description__TypeSource sources[1];
  static const rosidl_runtime_c__type_description__TypeSource__Sequence source_sequence = {sources, 1, 1};
  static bool constructed = false;
  if (!constructed) {
    sources[0] = *my_robot_interfaces__msg__PoseCommand__get_individual_type_description_source(NULL),
    constructed = true;
  }
  return &source_sequence;
}
