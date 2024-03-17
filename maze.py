import pygame
import math
import serial
import struct

PAYLOAD_SIZE = 24

ser = serial.Serial("COM3", 115200)

bot_1_x, bot_1_y = 0, 2
bot_2_x, bot_2_y = 2, 0

# anything from port
# sensor_data_bot_1 = []
# sensor_data_bot_2 = []

# JUST KEEP APPENDING THE DIRECTIONS IF THE BOT IS MOVING
bot_1_dir = [
    'E',
    'E',
    'W',
    'W',
    'N',
    'N',

]
bot_2_dir = [
    'W',
    'E',
    'S',
    'W',
    'W',
    'N',
]

# program variables
SCREEN_WIDTH = 600
SCREEN_HEIGHT = 600

# you can edit these variables to produce a maze of any number of rectangles
RECT_WIDTH = 200
RECT_HEIGHT = 200

RECT_NUM_HOR = int(SCREEN_WIDTH/RECT_WIDTH)
RECT_NUM_VER = int(SCREEN_HEIGHT/RECT_HEIGHT)
matrix_size = RECT_NUM_VER * RECT_NUM_HOR

# frames per second
frames_per_second = 0.5

# this array is for all of the rectangles
rectangles = []

bot_1 = None
bot_2 = None
# you can reset the current index to change the initial position of the algorithm
bot_1_index = 0
bot_2_index = 0

# this is used for the main loop
running = True

# rect class
class Rect:
    '''Class for each of the rectangle block displayed on screen including the four lines surrounding the rectangles'''

    def __init__(self, x:int, y:int, i, array_x, array_y):

        self.walls = {'top':True, 'left': True, 'right': True, 'bottom':True}
        self.visited = False
        self.x_coord = x
        self.y_coord = y
        self.color = 'darkgreen'
        self.index = i
        # self.TD_index = (i % RECT_NUM_HOR, i // RECT_NUM_HOR) # 2D index
        self.x = array_x
        self.y = array_y

        # These are the length of each of the four lines
        # Not much used
        self.left_line = 5
        self.rigth_line = 5
        self.top_line = 5
        self.bottom_line = 5

        # for the rectangle between the lines
        self.rectangle = pygame.Rect(x, y, x+RECT_WIDTH, y+RECT_HEIGHT )

    def draw(self, screen):
        '''Draws the main rectangle and draws all the four walls depending on the value of booleans'''
        pygame.draw.rect(screen, self.color, self.rectangle)
        # For top line
        if self.walls['top'] == True:
            pygame.draw.line(screen, 'black',( self.x_coord, self.y_coord),( self.x_coord+RECT_WIDTH, self.y_coord), self.top_line)
         # # for left line
        if self.walls['left'] == True:
            pygame.draw.line(screen, 'black',( self.x_coord, self.y_coord),( self.x_coord, self.y_coord+RECT_WIDTH), self.left_line)
         # # for bottom line
        if self.walls['bottom']== True:
            pygame.draw.line(screen, 'black',( self.x_coord, self.y_coord+RECT_WIDTH),( self.x_coord+RECT_WIDTH, self.y_coord+RECT_WIDTH), self.bottom_line)
         # # for right line
        if self.walls['right'] == True:
            pygame.draw.line(screen, 'black',( self.x_coord+RECT_WIDTH, self.y_coord),( self.x_coord+RECT_WIDTH, self.y_coord+RECT_WIDTH), self.rigth_line)

    def set_current(self, bool):
        '''Use this function to reset colors for the current rectangle and the other rectangles'''
        if bool:
            self.color = 'red'
        else:
            self.color = 'lightgreen'
        self.visited = True

    def __str__(self):
        '''return the string with the coordinates of the rectangle'''
        return "Coordinates: "+ str(self.x_coord)+ ', ' + str(self.y_coord)

# starting coordinates
x, y = 0, 0
# intial setup
# adds the rectangle classes to the array
for i in range(RECT_NUM_VER):

    for j in range(RECT_NUM_HOR):

        rect = Rect(x, y, j + i*RECT_NUM_HOR, j, i)
        # print(rect)
        rectangles.append(rect)
        x += RECT_WIDTH
    y += RECT_HEIGHT
    x = 0

def find_index(x, y):
    '''This function returns the index of the rectangle in the 1D array'''
    x = math.floor(x)
    y = math.floor(y)
    return int(y * (RECT_NUM_HOR) + x)

def process_sensor_data(data):
    '''EVERYTHING RELATED TO THE SENSOR DATA WILL BE DONE HERE'''
    #
    pass













# pygame setup
pygame.init()
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
clock = pygame.time.Clock()

bot_1 = rectangles[find_index(bot_1_x, bot_1_y)]
bot_1.set_current(False)
bot_2 = rectangles[find_index(bot_2_x, bot_2_y)]
bot_2.set_current(False)

def remove_walls(current, next):

    '''Simple mathematics to calculate the difference in x and y values
    Then difference is used to determine this walls from each rectangle to remove'''

    # if not check_valid_move(current, next):
    #     return
    # I was making a major blunder by reversing the subtractions
    x_diff = next.x_coord - current.x_coord
    y_diff = next.y_coord - current.y_coord

    if x_diff == RECT_WIDTH:  # Moving to the right
        current.walls['right'] = False
        next.walls['left'] = False
    elif x_diff == -RECT_WIDTH:  # Moving to the left
        current.walls['left'] = False
        next.walls['right'] = False
    elif y_diff == RECT_HEIGHT:  # Moving downwards
        current.walls['bottom'] = False
        next.walls['top'] = False
    elif y_diff == -RECT_HEIGHT:  # Moving upwards
        current.walls['top'] = False
        next.walls['bottom'] = False

def get_next_index(current, direction):
    '''This function returns the index of the next rectangle in the 1D array'''
    if direction == 'N':
        return find_index(current.x, current.y - 1)
    elif direction == 'E':
        return find_index(current.x + 1, current.y)
    elif direction == 'S':
        return find_index(current.x, current.y + 1)
    elif direction == 'W':
        return find_index(current.x - 1, current.y)
    else:
        return (current.x, current.y)

bot_1_direction = ''
bot_2_direction = ''

while running:
    # for the quit button
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    change = False

    # to clear the screen after every frame
    # screen.fill((0, 0, 0))

    # next two elements are popped from the array

    if bot_1_dir:
        screen.fill((0, 0, 0))
        bot_1_direction = bot_1_dir.pop(0)
        bot_1_index = get_next_index(bot_1, bot_1_direction)
        remove_walls(bot_1, rectangles[bot_1_index])
        bot_1 = rectangles[bot_1_index]
        bot_1.set_current(True)

        for rectangle in rectangles:
            rectangle.draw(screen)

        bot_1.set_current(False)
        bot_2.set_current(False)

        change = True

    if bot_2_dir:
        screen.fill((0, 0, 0))
        bot_2_direction = bot_2_dir.pop(0)
        bot_2_index = get_next_index(bot_2, bot_2_direction)
        remove_walls(bot_2, rectangles[bot_2_index])
        bot_2 = rectangles[bot_2_index]
        bot_2.set_current(True)


        # Drawing the rectangles

        # Drawing the rectangles
        for rectangle in rectangles:
            rectangle.draw(screen)

        bot_1.set_current(False)
        bot_2.set_current(False)

        change = True

    if not change:
        for rectangle in rectangles:
            rectangle.set_current(False)
            rectangle.draw(screen)

    pygame.display.flip()

    # read any input data
    data = ser.read(PAYLOAD_SIZE)

    # use short and float instead of int and double since arduino data type sizes are smaller than standard
    id, irLeft, irLeftFront, irFront, irRightFront, irRight, x, y, bearing = struct.unpack("<hhhhhhfff", data)
    process_sensor_data({
        "id": id,
        "irLeft": irLeft,
        "irLeftFront": irLeftFront,
        "irFront": irFront,
        "irRightFront": irRightFront,
        "irRight": irRight,
        "x": x,
        "y": y,
        "bearing": bearing
    })

    clock.tick(frames_per_second)

ser.close()
pygame.quit()
