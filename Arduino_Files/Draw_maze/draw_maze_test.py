import serial
import math
import matplotlib.pyplot as plt
import numpy as np

# data should be: right, right_forward, forward, left_forward, left
# each point is from 0 to 1023
sensor_data = [None, None, None, None, None]

robot_position = [0, 0]

unblocked_threshold = 950

# assume robot is facing forward at the beginning
# right, right_forward, forward, left_forward, left
direction_offsets = [(0, 1), (1, 1), (1, 0), (-1, 1), (0, -1)] 

maze_size = 5  # assume maze size 5x5
maze = np.full((maze_size*2+1, maze_size*2+1), 1)

for i in range(1,maze_size*2):
    for j in range(1, maze_size*2):
            maze[i][j] = 0


def update_maze(sensor_data, robot_position):
    # robot are in block 0-5 in maze
    # transferred to 1, 3, 5, 7, 9 in maze array
    x, y = robot_position
    mx, my = math.floor(x * 2) + 1, math.floor(y * 2) + 1
    print("x, y: ", x, y)
    print("mx, my: ", mx, my)

    for i, (dx, dy) in enumerate(direction_offsets):
        sensor_value = sensor_data[i]

        # check each sensor value
        print("Sensor and sensor_value: ", i, sensor_value)

        if sensor_value is not None:
            # calculate the wall position
            wall_x, wall_y = mx + dx, my + dy

            print("Sensor Val: ", sensor_value)
            print("Wall x, y: ", wall_x, wall_y)

            # update the maze
            if sensor_value < unblocked_threshold:
                if 0 <= wall_x < len(maze) and 0 <= wall_y < len(maze[0]):
                    maze[wall_x, wall_y] = 1
                    print("Blocked, maze[wall_x, wall_y]: ", maze[wall_x, wall_y])

    print(maze)


def draw_maze():
    plt.imshow(maze, cmap='binary')  # 使用黑白颜色映射
    plt.show()

if __name__ == "__main__":
    print(maze)

    sensor_data_samples = [
    [930, 950, 950, 950, 930],  # 两侧有障碍物
    [940, 950, 950, 950, 920],  # 两侧有障碍物
    [940, 950, 950, 950, 920],  # 两侧有障碍物
    [920, 950, 950, 950, 940],  # 两侧有障碍物
    [920, 950, 950, 950, 940],  # 两侧有障碍物
    [920, 950, 950, 950, 950],  # 右侧有障碍物，左侧无障碍物





    # [950, 1023, 900, 950, 1023],  # 仅前方有障碍物
    # [400, 950, 1023, 950, 1023],  # 右侧有障碍物
    # [950, 1023, 950, 200, 1023],  # 左前方有障碍物
    # [950, 300, 950, 1023, 950],   # 右前方有障碍物
    # [950, 1023, 1023, 1023, 500]  # 左侧有障碍物
    ]

    robot_position_samples = [
    [0.0, 0.1],
    [0.6, 0.2],
    [1.1, 0.3],
    [2.3, 0.5],
    [3.3, 0.6],
    [4.3, 0.8],
    [4.4, 0.9],



    # [1.0, 1.0],
    # [2.5, 1.5],
    # [3.0, 3.0],
    # [4.5, 2.5],
    # [1.0, 4.0]
    ]



    

    num = 1
    for sensor_data_sample, robot_position_sample in zip(sensor_data_samples, robot_position_samples):
        print("Test: ", num)
        num += 1   
        print("sensor_data_sample: ", sensor_data_sample)
        print("robot_position_sample: ", robot_position_sample)

        update_maze(sensor_data_sample, robot_position_sample)

        print("Robot Position:", robot_position_sample)
        print("Sensor Data:", sensor_data_sample)
        #print(maze)

        #draw_maze()
    


