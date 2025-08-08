import math
theta_0 = math.radians(0.0)
theta_1 = math.radians(30.0)
theta_2 = math.radians(90.0)
theta_3 = math.radians(-20)
theta_4 = 0.0
theta_5 = 0.0

l1 = 0.105
l2 = 0.125
l3 = 0.175

x = -l1*math.sin(theta_0)*math.cos(theta_1) + l2*math.cos(theta_2) + l3*math.cos(theta_3)
y = l1*math.cos(theta_0)*math.cos(theta_1) + l2*math.cos(theta_2) + l3*math.cos(theta_3)
z = l1*math.sin(theta_1) + l2*math.sin(theta_2) + l3*math.sin(theta_3)

print(f"x: {x:.3f}, y: {y:.3f}, z: {z:.3f}")