import socket
import pygame
import multiprocessing

HOST = "0.0.0.0"
TCP_PORT = 5000 
UDP_PORT = 3333

MAP_LEN = 134
MAP_MAZE = "0" * 134
FLAG_WIN = "00"
rows = 8
cols = 16
maze = [[0] * cols for _ in range(rows)]
des = (0, 0)
us1 = (0, 0)
us2 = (0, 0)
block = 80
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)
BLUE = (0, 0, 255)

def sync_map(shared_data):
    global MAP_MAZE, maze, des
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM, socket.IPPROTO_TCP)
    server_socket.bind((HOST, TCP_PORT))
    server_socket.listen(1)
    print("server is listening!....")
    
    client_socket_gen, addr = server_socket.accept()
    print("accept", addr)
    data = client_socket_gen.recv(MAP_LEN).decode('iso-8859-1')
    MAP_MAZE = data
    if MAP_MAZE == '0' * 134: return
    print("received:", data, len(data))
    client_socket_gen.send(MAP_MAZE.encode('iso-8859-1'))
    print('responed:', MAP_MAZE)
    client_socket_gen.close()
    print('disconnect to ', addr)

    client_socket_find, addr = server_socket.accept()
    print("accept", addr)
    data = client_socket_find.recv(MAP_LEN).decode('iso-8859-1')
    print("received: ", data, len(data))
    client_socket_find.send(MAP_MAZE.encode('iso-8859-1'))
    print('responed:', MAP_MAZE)
    client_socket_find.close()
    print('disconnect to ', addr)
    
    for i in range(128):
        maze[i // 16][i % 16] = int(MAP_MAZE[i])
    des = (int(MAP_MAZE[128]), int(MAP_MAZE[129:131]))
    
    # Cập nhật shared_data
    shared_data['maze'] = maze
    shared_data['des'] = des
    shared_data['map_maze'] = MAP_MAZE

def sync_pos(shared_data):
    global FLAG_WIN, us1 ,us2, MAP_MAZE
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
    server_socket.bind((HOST, UDP_PORT))
    server_socket.settimeout(3)
    d = 10
    while d>0:
        try:
            data, addr = server_socket.recvfrom(4)
            t = data.decode('iso-8859-1')
            if (not t.isdigit()): continue
            if t[0:3] == MAP_MAZE[128:131]: FLAG_WIN = '1' + t[3]
            server_socket.sendto(FLAG_WIN.encode('iso-8859-1'), addr)
            if t[3] == '0': 
                us1 = (int(t[0]), int(t[1:3]))
            else:
                us2 = (int(t[0]), int(t[1:3]))
            
            shared_data['us1'] = us1
            shared_data['us2'] = us2

            if FLAG_WIN[1] == '1': d = d - 1
        except socket.timeout:
            server_socket.close()
            break

def server(shared_data):
    global FLAG_WIN, MAP_MAZE
    while (True):
        FLAG_WIN = '00'
        MAP_MAZE = '0'* 134
        sync_map(shared_data)
        sync_pos(shared_data)

def visual(shared_data):
    global MAP_MAZE, maze, us1, us2, des
    print(us1, des, us2)
    pygame.init()
    screen = pygame.display.set_mode((block * cols, block * rows))
    pygame.display.set_caption("NoProblem")

    clock = pygame.time.Clock()
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                exit()

        screen.fill(WHITE)

        # Lấy dữ liệu từ shared_data
        maze = shared_data['maze']
        des = shared_data['des']
        us1 = shared_data['us1']
        us2 = shared_data['us2']

        for i in range(rows):
            for j in range(cols):
                x1 = j * block
                y1 = i * block
                x2 = x1 + block
                y2 = y1 + block
                if maze[i][j] == 0:
                    pygame.draw.line(screen, BLACK, (x1, y1), (x2, y1), 2)
                    pygame.draw.line(screen, BLACK, (x1, y1), (x1, y2), 2)
                elif maze[i][j] == 1:
                    pygame.draw.line(screen, BLACK, (x1, y1), (x1, y2), 2)
                elif maze[i][j] == 2:
                    pygame.draw.line(screen, BLACK, (x1, y1), (x2, y1), 2)

        x1 = des[1] * block
        y1 = des[0] * block
        x2 = x1 + block
        y2 = y1 + block
        pygame.draw.line(screen, RED, (x1, y1), (x2, y2), 2)
        pygame.draw.line(screen, RED, (x1, y2), (x2, y1), 2)

        x = us1[1] * block + block // 2
        y = us1[0] * block + block // 2
        pygame.draw.circle(screen, GREEN, (x, y), block // 2)

        x = us2[1] * block + block // 2
        y = us2[0] * block + block // 2
        pygame.draw.circle(screen, BLUE, (x, y), block // 2)

        pygame.display.flip()
        clock.tick(30)

if __name__ == '__main__':
    with multiprocessing.Manager() as manager:
        shared_data = manager.dict({
            'maze': [[0] * cols for _ in range(rows)],
            'des': (0, 0),
            'us1': (0, 0),
            'us2': (0, 0),
            'map_maze': '0' * MAP_LEN,
        })

        process_socket = multiprocessing.Process(target=server, args=(shared_data,))
        process_visual = multiprocessing.Process(target=visual, args=(shared_data,))

        process_socket.start()
        process_visual.start()

        process_socket.join()
        process_visual.join()
