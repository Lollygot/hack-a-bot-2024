import serial
import math
import numpy as np

# it is anti clockwise for rotation

# data should be: right, right_forward, forward, left_forward, left
# each point is from 0 to 1023
sensor_data = [None, None, None, None, None]

robot_position = [0, 0]

unblocked_threshold = 950

# right, right_forward, forward, left_forward, left
direction_offsets = [(0, 1), (1, 1), (1, 0), (-1, 1), (0, -1)] 

# directions: right, right_forward, forward, left_forward, left
directions = {
    'right': (0, -1),
    'right_forward': (1, -1),
    'forward': (1, 0),
    'left_forward': (1, 1),
    'left': (0, 1)
}


n = 5  # assume maze size 5x5

# store maze in a dictionary
maze = {(x, y): {'up': False, 'right': False, 'down': False, 'left': False}
        for x in range(n) for y in range(n)}


def generate_rotation_matrix(theta):
    """Generate a 2D rotation matrix according to the given angle"""
    radians = np.radians(theta)
    c, s = np.cos(radians), np.sin(radians)
    return np.array([[c, -s], [s, c]])


def rotate_vector(vector, theta):
    """Rotate a 2D vector by a given angle in degrees"""
    rot_matrix = generate_rotation_matrix(theta)
    return rot_matrix.dot(vector)


def directions_with_rotation(angle):
    rotated_sensor_data = {}

    basic_direction_vectors = {
        'up': np.array([1, 0]),
        'right': np.array([0, -1]),
        'down': np.array([-1, 0]),
        'left': np.array([0, 1])
    }

    min_angle = {dir: float('inf') for dir in basic_direction_vectors.keys()}
    closest_direction_for_each_basic_dir = {dir: None for dir in basic_direction_vectors.keys()}
    for direction, vector in directions.items():
        # 旋转向量
        rotated_vector = rotate_vector(vector, angle)

        # 寻找最接近的基本方向
        for basic_direction, basic_vector in basic_direction_vectors.items():
            angle_with_basic = angle_between(rotated_vector, basic_vector)

            #if angle_with_basic < min_angle:
            if angle_with_basic < min_angle[basic_direction]:
                if angle_with_basic < np.pi / 2:  # 只有当角度小于 90 度时才更新
                    print(f"Updating min_angle from {min_angle} to {angle_with_basic}, with direction {basic_direction}, and sensor_direction {direction}")
                    #prev_min = min_angle
                    #min_angle = angle_with_basic
                    min_angle[basic_direction] = angle_with_basic
                    closest_direction_for_each_basic_dir[basic_direction] = direction

                # print("Angle with basic direction: ", angle_with_basic, " with direction: ", basic_direction, "prev_min angle: ", prev_min)
                print("Sensor value: ", sensor_data[list(directions.keys()).index(direction)], " at angle: ", angle)

        # 关联传感器数据
        # sensor_value = sensor_data[list(directions.keys()).index(direction)]
        # rotated_sensor_data[closest_direction] = sensor_value
    # 更新每个基本方向的传感器值
    for basic_dir, closest_dir in closest_direction_for_each_basic_dir.items():
        if closest_dir is not None:
            sensor_value = sensor_data[list(directions.keys()).index(closest_dir)]
            rotated_sensor_data[basic_dir] = sensor_value

    #print("Rotated sensor data: ", rotated_sensor_data, " at angle: ", angle)

    return rotated_sensor_data


def angle_between(v1, v2):
    """Calculate the angle between two vectors in radians"""
    unit_v1 = v1 / np.linalg.norm(v1)
    unit_v2 = v2 / np.linalg.norm(v2)
    dot_product = np.dot(unit_v1, unit_v2)
    angle = np.arccos(dot_product)
    return angle

# def update_blockage(maze, position, sensor_data, block_threshold):
#     x, y = position
    
#     # directions: right, right_forward, forward, left_forward, left
#     directions = {
#         'right': (0, 1),
#         'right_forward': (1, 1),
#         'forward': (1, 0),
#         'left_forward': (-1, 1),
#         'left': (0, -1)
#     }

#     reverse_directions = {
#         'right': 'left',
#         'right_forward': 'left_forward',
#         'forward': 'up',   # 这里我假设'forward'方向的反方向是'up'
#         'left_forward': 'right_forward',
#         'left': 'right'
#     }

#     for direction, offset in directions.items():
#         dx, dy = offset
#         neighbor_x, neighbor_y = x + dx, y + dy
#         sensor_value = sensor_data[list(directions.keys()).index(direction)]

#         if sensor_value is not None and 0 <= neighbor_x < n and 0 <= neighbor_y < n:
#             if sensor_value < block_threshold:
#                 maze[(x, y)][direction] = True
#                 # 阻挡从邻居节点回到当前节点的路径
#                 # reverse_direction = {(-1, 1): 'left_forward', (0, -1): 'right',
#                 #                      (1, -1): 'right_forward', (0, 1): 'left', (-1, 0): 'forward'}
#                 # maze[(neighbor_x, neighbor_y)][reverse_direction[(-dx, -dy)]] = True
#                 reverse_dir = reverse_directions[direction]
#                 maze[(neighbor_x, neighbor_y)][reverse_dir] = True

#                 print("Blocked: ", direction, " at ", (x, y))
#                 print("Blocked: ", reverse_dir, " at ", (neighbor_x, neighbor_y))

def update_blockage(maze, position, sensor_data, block_threshold, angle):
    x, y = position
    rotated_sensor_data = directions_with_rotation(angle)

    # Reverse direction mapping
    adjusted_directions = {
        'up': (1, 0),
        'right': (0, -1),
        'down': (-1, 0),
        'left': (0, 1)
    }

    reverse_directions = {
        'up': 'down',
        'right': 'left',
        'down': 'up',
        'left': 'right'
    }

    # Update blockages in the maze
    for direction, offset in adjusted_directions.items():
        dx, dy = offset
        neighbor_x, neighbor_y = x + dx, y + dy
        sensor_value = rotated_sensor_data.get(direction)

        print("Sensor value: ", sensor_value, " at ", (x, y), " direction: ", direction)

        if sensor_value is not None and 0 <= neighbor_x < n and 0 <= neighbor_y < n:
            if sensor_value < block_threshold:
                maze[(x, y)][direction] = True
                reverse_dir = reverse_directions[direction]
                maze[(neighbor_x, neighbor_y)][reverse_dir] = True

                print("Blocked: ", direction, " at ", (x, y))
                print("Blocked: ", reverse_dir, " at ", (neighbor_x, neighbor_y))
                print("Update: ", direction, " at ", (x, y), " to ", maze[(x, y)][direction])

                
def print_maze(maze, size):
    # top wall
    print(" " + "_ " * size)

    for x in range(size):
        # left wall
        print("|", end="")

        for y in range(size):
            # bottom wall
            if x+1 < 5 and (maze[(x, y)]['up']):
                print("_", end="")
            #if y > 0 and y+1 < 5 and maze[(x, y + 1)]['up']:  # check 'forward' of the previous block
            #    print("_", end="")
            else:
                print(" ", end="")

            # right wall
            if y < size - 1:
                if maze[(x, y)]['left']:
                    print("|", end="")
                else:
                    print(" ", end="")
            else:
                print("|", end="")  # right wall
        print() 

    # bottom
    print(" " + "- " * size)


if __name__ == "__main__":
    test_data = [
        # right side
        # ([930, 950, 1023, 980, 950], (0, 1), 90),
        # ([930, 950, 950, 950, 950], (1, 1), 0),
        # ([930, 950, 950, 950, 950], (2, 1), 0),
        # ([930, 950, 950, 950, 950], (3, 1), 0),
        # ([949, 950, 1023, 950, 959], (4, 1), 0),

        # ([930, 950, 1023, 980, 950], (0, 2), 0),
        # ([930, 950, 950, 950, 950], (1, 2), 0),
        # ([930, 950, 950, 950, 950], (2, 2), 90),
        # ([930, 950, 950, 950, 950], (3, 2), 0),
        # ([949, 950, 1023, 950, 959], (4, 2), 0),

        # ([930, 950, 1023, 980, 950], (0, 3), 0),
        # ([930, 950, 950, 950, 950], (1, 3), 0),
        # ([930, 950, 950, 950, 950], (2, 3), 90),
        # ([930, 950, 950, 950, 950], (3, 3), 0),
        # ([949, 950, 1023, 950, 959], (4, 3), 0),

        # ([930, 950, 1023, 980, 950], (0, 4), 0),
        # ([930, 950, 950, 950, 950], (1, 4), 0),
        # ([930, 950, 950, 950, 950], (2, 4), 0),
        # ([930, 950, 950, 950, 950], (3, 4), 0),
        # ([949, 950, 1023, 950, 959], (4, 4), 0),

        # bottom side
        # ([950, 970, 930, 980, 960], (0, 0), 0),
        # ([950, 970, 930, 980, 960], (0, 1), 0),
        # ([950, 970, 930, 980, 960], (0, 2), 0),
        # ([950, 970, 930, 980, 960], (0, 3), 0),
        # ([950, 970, 930, 980, 960], (0, 4), 0),

        # ([950, 970, 930, 980, 960], (1, 0), 0),
        # ([950, 970, 930, 980, 960], (1, 1), 0),
        # ([950, 970, 930, 980, 960], (1, 2), 0),
        # ([950, 970, 930, 980, 960], (1, 3), 0),
        # ([950, 970, 930, 980, 960], (1, 4), 0),

        # ([950, 970, 930, 980, 960], (2, 0), 0),
        # ([950, 970, 930, 980, 960], (2, 1), 0),
        # ([950, 970, 930, 980, 960], (2, 2), 0),
        # ([950, 970, 930, 980, 960], (2, 3), 0),
        # ([950, 970, 930, 980, 960], (2, 4), 0),

        # ([950, 970, 930, 980, 960], (3, 0), 0),
        # ([950, 970, 930, 980, 960], (3, 1), 0),
        # ([950, 970, 930, 980, 960], (3, 2), 0),
        # ([950, 970, 930, 980, 960], (3, 3), 0),
        # ([950, 970, 930, 980, 960], (3, 4), 0),

        # ([950, 970, 930, 980, 960], (4, 0), 0),
        # ([950, 970, 930, 980, 960], (4, 1), 0),
        # ([950, 970, 930, 980, 960], (4, 2), 0),
        # ([950, 970, 930, 980, 960], (4, 3), 0),
        # ([950, 970, 930, 980, 960], (4, 4), 0),

        # # left side
        # ([950, 950, 950, 950, 930], (0, 3), 0),
        # ([950, 950, 950, 950, 930], (1, 3), 90),
        # ([950, 950, 950, 950, 930], (2, 3), 270),

        # top sides
        # ([950, 970, 930, 980, 960], (2, 0), 180),
        # ([950, 970, 930, 980, 960], (2, 1), 180),
        # ([950, 970, 930, 980, 960], (2, 2), 180),

        # # left top
        # ([950, 950, 950, 930, 950], (3, 3), 0),

        # # right top
        # ([950, 930, 950, 930, 950], (2, 4), 0)


    ]

    # update maze
    for sensor_data, position , angle in test_data:
        print("Update: ")

        #print(maze)

        print_maze(maze, n)

        update_blockage(maze, position, sensor_data, unblocked_threshold, angle)


    print_maze(maze, n)

    #print(maze)

