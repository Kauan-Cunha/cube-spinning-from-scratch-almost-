import pygame
from typing import Tuple, List
import time
import math
from abc import ABC, abstractmethod

WIDTH = 900
HEIGHT = WIDTH

VTX_RADIUS = 20
DIST = 200

class Ponto():
    def __init__(self, x: float, y: float, z: float):
        self.x = x
        self.y = y
        self.z = z

    def setX(self, new_x: float):
        self.x = new_x
    def setY(self, new_y: float):
        self.y = new_y
    def setZ(self, new_z: float):
        self.z = new_z

class Forma(ABC):
    def __init__(self):
        """
        Classe abstrata de uma forma.
        self.vtxs: lista de pontos dos vértices
        self.conn: ordem de conexão das arestas da lista self.vtxs
        """
        self.vtxs: List[Ponto] = []
        self.conn: List[Tuple[int, int]] = []
        self.linePoints: List[Ponto] = []

    @abstractmethod
    def construct(self):
        pass

    def drawLines(self, resolution:int = 100):
        for edge in self.conn:
            self.linePoints.extend(drawLine(self.vtxs[edge[0]], self.vtxs[edge[1]]))

    def rotateX(self, teta: float):
        for ponto in self.vtxs:
            rotateX(ponto, teta)
        for ponto in self.linePoints:
            rotateX(ponto, teta)
    def rotateY(self, teta: float):
        for ponto in self.vtxs:
            rotateY(ponto, teta)
        for ponto in self.linePoints:
            rotateY(ponto, teta)
    def rotateZ(self, teta: float):
        for ponto in self.vtxs:
            rotateZ(ponto, teta)
        for ponto in self.linePoints:
            rotateZ(ponto, teta)

class Cube(Forma):
    def __init__(self):
        super().__init__()

    def construct(self, sideSize: float):
        half = sideSize/2
        self.vtxs.extend([Ponto(-half, -half, -half), Ponto(half, -half, -half), Ponto(half, half, -half),Ponto(-half, half, -half),
                         Ponto(-half, -half, half), Ponto(half, -half, half), Ponto(half, half, half), Ponto(-half, half, half)])

        self.conn.extend([(0, 1), (1, 2), (2, 3), (3, 0), (4, 5), (5, 6), (6, 7), (7, 4),
                  (0, 4), (1, 5), (2,6), (3, 7)])

class RegularTriangle(Forma):
    def __init__(self):
        super().__init__()

    def construct(self, sideSize):
        h = (sideSize*math.sqrt(3))/2
        a = (sideSize*math.sqrt(3))/6
        half = sideSize/2
        self.vtxs.extend([Ponto(a, -half, -a), Ponto(a, half, -a), Ponto(a-h, 0, -a),
                          Ponto(0, 0, h-a)])

        self.conn.extend([(0, 1), (0, -1), (1, 2), (1, -1), (2, 0), (2, -1)])
        

def centralize(pontos: Ponto, width: int, height:int) -> Ponto:
    res = []
    for ponto in pontos:
        res.append(Ponto(ponto.x + width//2, ponto.y + height//2, ponto.z))

    return res

def rotateZ(ponto: Ponto, teta: float):
    currX = ponto.x
    currY = ponto.y
    ponto.setX((currX*math.cos(teta) - currY*math.sin(teta)))
    ponto.setY((currX*math.sin(teta) + currY*math.cos(teta)))

def rotateX(ponto: Ponto, teta: float):
    currZ = ponto.z 
    currY = ponto.y 
    ponto.setY((currY*math.cos(teta) - currZ*math.sin(teta)))
    ponto.setZ((currY*math.sin(teta) + currZ*math.cos(teta)))

def rotateY(ponto: Ponto, teta: float):
    currX = ponto.x
    currZ = ponto.z
    ponto.setX((currX*math.cos(teta) + currZ*math.sin(teta)))
    ponto.setZ((currZ*math.cos(teta)-currX*math.sin(teta)))

def drawLine(a: Ponto, b: Ponto, resolution: int = 100) -> List[Ponto]:
    """Return points that draw lines in image"""
    t, step = 0, 1/resolution 
    res = []
    #r : (x, y, z) = (x0, y0, z0) + t(x1 - x0, y1 - y0, z1 - z0)
    while t < 1:
        res.append(Ponto(a.x + t*(b.x - a.x), a.y + t*(b.y - a.y), a.z + t*(b.z - a.z)))
        t += step

    return res

def drawForma(screen: pygame.Surface, forma: Forma, showVtx:bool = True, showEdge:bool = True):
    #desenha vtx
    if showVtx:
        for ponto in forma.vtxs:
            pygame.draw.circle(screen, "red", (ponto.x + WIDTH//2, ponto.y + HEIGHT//2), (VTX_RADIUS*DIST)//(2*(ponto.z + 500 + DIST)))

    # desenha aresta
    if showEdge:
        for ponto in forma.linePoints:
            pygame.draw.circle(screen, "red", (ponto.x + WIDTH//2, ponto.y + HEIGHT//2), (VTX_RADIUS*DIST)//(2*(ponto.z + 500 + DIST)))

def main():
    #setup
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))

    #inicialize the Cube
    forma: Forma = RegularTriangle()
    forma.construct(300)
    forma.drawLines(resolution=500)

    #draw
    running = True
    while running:
        screen.fill("purple")


        drawForma(screen, forma)
        forma.rotateY(0.005)
        forma.rotateZ(0.005)
        forma.rotateX(0.005)

        time.sleep(0.0005)
        pygame.display.flip()


if __name__ == "__main__":
    main()