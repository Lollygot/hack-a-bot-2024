import pygame
bot_1_x, bot_1_y = 2, 0
bot_2_x, bot_2_y = 0, 2

array_of_incoming_data = [
    (2, 0), # bot 1 initial
    (0, 2), # bot 2 inital
    (1, 0), # bot 1
    (0, 1), # bot 2
    (2, 0), # bot 1
    (1, 1), # bot 2
    (2, 1), # bot 1
    (1, 2), # bot 2
    (1, 1), # bot 1
    (2, 2), # bot 2
    (0, 1), # bot 1
    (1, 2), # bot 2
    (0, 0), # bot 1
    (1, 1), # bot 2
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
current_rect_index = 0
bot_1_index = 0
bot_2_index = 0



backtracking = False
# this is used for the main loop
running = True








# rect class 
class Rect:
    '''Class for each of the rectangle block displayed on screen including the four lines surrounding the rectangles'''

    def __init__(self, x:int, y:int, i):

        self.walls = {'top':True, 'left': True, 'right': True, 'bottom':True}
        self.visited = False
        self.x_coord = x
        self.y_coord = y
        self.color = 'darkgreen'
        self.index = i

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
        
        rect = Rect(x, y, j + i*RECT_NUM_HOR)
        # print(rect)
        rectangles.append(rect)
        x += RECT_WIDTH
    y += RECT_HEIGHT
    x = 0





def find_index(x, y):
    '''This function returns the index of the rectangle in the 1D array'''
    return y * RECT_NUM_HOR + x







# pygame setup
pygame.init()
screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
clock = pygame.time.Clock()





bot_1 = rectangles[bot_1_index]
bot_2 = rectangles[bot_2_index]



def remove_walls(current, next):
    
    '''Simple mathematics to calculate the difference in x and y values
    Then difference is used to determine this walls from each rectangle to remove'''

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



while running:
    # for the quit button
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    
    
    # to clear the screen after every frame
    # screen.fill((0, 0, 0))
    
    # next two elements are popped from the array
    if len(array_of_incoming_data) > 0:
        screen.fill((0, 0, 0))
        
        
        next_bot_1_x, next_bot_1_y = array_of_incoming_data.pop(0)
        next_bot_2_x, next_bot_2_y = array_of_incoming_data.pop(0)
        
        bot_1_index = find_index(next_bot_1_x, next_bot_1_y)
        bot_2_index = find_index(next_bot_2_x, next_bot_2_y)
        print("Bot 1 index")
        print(bot_2_index)
        print("Bot 2 index")
        print(bot_2_index)
        
        
        remove_walls(bot_1, rectangles[bot_1_index])
        remove_walls(bot_2, rectangles[bot_2_index])
        
        bot_1 = rectangles[bot_1_index]
        bot_2 = rectangles[bot_2_index]
        
        bot_1.set_current(True)
        bot_2.set_current(True)
        

    


    

    
        # Drawing the rectangles
        for rectangle in rectangles:
            rectangle.draw(screen)
            
        bot_1.set_current(False)
        bot_2.set_current(False)
        
    else:
        for rectangle in rectangles:
            rectangle.set_current(False)
            rectangle.draw(screen)
            
    pygame.display.flip()

    clock.tick(frames_per_second)  # limits FPS

pygame.quit()