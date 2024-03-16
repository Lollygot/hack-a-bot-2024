import pygame

# 初始化 Pygame
pygame.init()

# 设置窗口大小和标题
width, height = 500, 500
screen = pygame.display.set_mode((width, height))
pygame.display.set_caption('Maze')

# 定义颜色
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)

# 迷宫数据：1代表墙壁，0代表通道
maze_data = [
    [1, 1, 1, 1, 1, 1],
    [1, 0, 0, 0, 0, 1],
    [1, 0, 1, 1, 0, 1],
    [1, 0, 1, 0, 0, 1],
    [1, 0, 0, 0, 1, 1],
    [1, 1, 1, 1, 1, 1]
]

# 设置迷宫每个方块的大小
block_size = width // len(maze_data)

# 绘制迷宫的函数
def draw_maze(maze):
    for y, row in enumerate(maze):
        for x, cell in enumerate(row):
            rect = pygame.Rect(x*block_size, y*block_size, block_size, block_size)
            if cell == 1:
                pygame.draw.rect(screen, BLACK, rect)
            else:
                pygame.draw.rect(screen, WHITE, rect)

# 游戏主循环
running = True
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
    
    screen.fill(WHITE)  # 用白色填充屏幕
    
    draw_maze(maze_data)  # 绘制迷宫
    
    pygame.display.flip()  # 更新屏幕显示
    
pygame.quit()
