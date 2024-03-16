import serial
import math
import numpy as np

# data should be: right, right_forward, forward, left_forward, left
# each point is from 0 to 1023
sensor_data = [None, None, None, None, None]

robot_position = [0, 0]

unblocked_threshold = 950

# right, right_forward, forward, left_forward, left
direction_offsets = [(0, 1), (1, 1), (1, 0), (-1, 1), (0, -1)] 


n = 5  # assume maze size 5x5

# store maze in a dictionary
maze = {(x, y): {'up': False, 'right': False, 'down': False, 'left': False}
        for x in range(n) for y in range(n)}

def update_blockage(maze, position, sensor_data, block_threshold):
    x, y = position
    
    # directions: right, right_forward, forward, left_forward, left
    directions = {
        'right': (0, 1),
        'right_forward': (1, 1),
        'forward': (1, 0),
        'left_forward': (-1, 1),
        'left': (0, -1)
    }

    reverse_directions = {
        'right': 'left',
        'right_forward': 'left_forward',
        'forward': 'up',   # 这里我假设'forward'方向的反方向是'up'
        'left_forward': 'right_forward',
        'left': 'right'
    }

    for direction, offset in directions.items():
        dx, dy = offset
        neighbor_x, neighbor_y = x + dx, y + dy
        sensor_value = sensor_data[list(directions.keys()).index(direction)]

        if sensor_value is not None and 0 <= neighbor_x < n and 0 <= neighbor_y < n:
            if sensor_value < block_threshold:
                maze[(x, y)][direction] = True
                # 阻挡从邻居节点回到当前节点的路径
                # reverse_direction = {(-1, 1): 'left_forward', (0, -1): 'right',
                #                      (1, -1): 'right_forward', (0, 1): 'left', (-1, 0): 'forward'}
                # maze[(neighbor_x, neighbor_y)][reverse_direction[(-dx, -dy)]] = True
                reverse_dir = reverse_directions[direction]
                maze[(neighbor_x, neighbor_y)][reverse_dir] = True

                print("Blocked: ", direction, " at ", (x, y))
                print("Blocked: ", reverse_dir, " at ", (neighbor_x, neighbor_y))

                
def print_maze(maze, size):
    # top wall
    print(" " + "_ " * size)

    for x in range(size):
        # left wall
        print("|", end="")

        for y in range(size):
            # bottom wall
            if x+1 < 5 and maze[(x+1, y)]['up']:
                print("_", end="")
            #if y > 0 and y+1 < 5 and maze[(x, y + 1)]['up']:  # check 'forward' of the previous block
            #    print("_", end="")
            else:
                print(" ", end="")

            # right wall
            if y < size - 1:
                if maze[(x, y)]['right']:
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
        # ([930, 950, 950, 950, 930], (1, 1)),
        # ([1023, 1023, 950, 1023, 1023], (2, 2)),
        # ([1023, 940, 940, 940, 1023], (3, 3)),
        # ([950, 950, 1023, 950, 950], (4, 4)),
        ([930, 950, 1023, 950, 930], (0, 0)),
        ([930, 950, 950, 950, 930], (1, 0)),
        ([930, 950, 950, 950, 930], (2, 0)),
        ([930, 950, 950, 950, 930], (3, 0)),
        ([949, 950, 1023, 950, 949], (4, 0)),

        ([950, 950, 930, 950, 950], (0, 1)),
        ([950, 950, 930, 950, 950], (0, 2)),
        ([950, 950, 930, 950, 950], (0, 3)),
        ([950, 950, 930, 950, 950], (0, 4)),


        ([950, 950, 930, 950, 950], (4, 4))


    ]

    # update maze
    for sensor_data, position in test_data:
        print("Update: ")

        #print(maze)

        print_maze(maze, n)

        update_blockage(maze, position, sensor_data, unblocked_threshold)


    print_maze(maze, n)
