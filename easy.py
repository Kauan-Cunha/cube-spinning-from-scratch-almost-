import pygame
from typing import Tuple, List
import time
import math

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

def centralize(pontos: Ponto, width: int, height:int) -> Ponto:
    res = []
    for ponto in pontos:
        res.append(Ponto(ponto.x + width//2, ponto.y + height//2, ponto.z))

    return res

def rotateZ(ponto: Ponto, teta: float):
    currX = ponto.x - 450
    currY = ponto.y - 450
    ponto.setX((currX*math.cos(teta) - currY*math.sin(teta))+ 450)
    ponto.setY((currX*math.sin(teta) + currY*math.cos(teta))+ 450)

def rotateX(ponto: Ponto, teta: float):
    currZ = ponto.z -200
    currY = ponto.y - 450
    ponto.setY((currY*math.cos(teta) - currZ*math.sin(teta)) + 450)
    ponto.setZ((currY*math.sin(teta) + currZ*math.cos(teta)) + 200)

def rotateY(ponto: Ponto, teta: float):
    currX = ponto.x - 450
    currZ = ponto.z - 200
    ponto.setX((currX*math.cos(teta) + currZ*math.sin(teta))+ 450)
    ponto.setZ((currZ*math.cos(teta)-currX*math.sin(teta))+ 200)

def drawLine(a: Ponto, b: Ponto, resolution: int = 100) -> List[Ponto]:
    """Return points that draw lines in image"""
    t, step = 0, 1/resolution 
    res = []
    #r : (x, y, z) = (x0, y0, z0) + t(x1 - x0, y1 - y0, z1 - z0)
    while t < 1:
        print
        res.append(Ponto(a.x + t*(b.x - a.x), a.y + t*(b.y - a.y), a.z + t*(b.z - a.z)))
        t += step

    return res

def main():
    #setup
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    
    vtxs = [Ponto(-200, 0, 100), Ponto(0, -200, 100), Ponto(200, 0, 100), Ponto(0, 200, 100),
             Ponto(-200, 0, 400), Ponto(0, -200, 400), Ponto(200, 0, 400), Ponto(0, 200, 400)]

    cVtxs =centralize(vtxs, WIDTH, HEIGHT)

    cVtxs.extend(drawLine(cVtxs[0], cVtxs[1]))
    cVtxs.extend(drawLine(cVtxs[1], cVtxs[2]))
    cVtxs.extend(drawLine(cVtxs[2], cVtxs[3]))
    cVtxs.extend(drawLine(cVtxs[3], cVtxs[0]))
    cVtxs.extend(drawLine(cVtxs[4], cVtxs[5]))
    cVtxs.extend(drawLine(cVtxs[5], cVtxs[6]))
    cVtxs.extend(drawLine(cVtxs[6], cVtxs[7]))
    cVtxs.extend(drawLine(cVtxs[7], cVtxs[4]))
    cVtxs.extend(drawLine(cVtxs[0], cVtxs[4]))
    cVtxs.extend(drawLine(cVtxs[1], cVtxs[5]))
    cVtxs.extend(drawLine(cVtxs[2], cVtxs[6]))
    cVtxs.extend(drawLine(cVtxs[3], cVtxs[7]))

    #draw
    running = True
    while running:
        screen.fill("purple")
        for ponto in cVtxs:
            rotateX(ponto, 0.005)
            # rotateY(ponto, 0.005)
            rotateZ(ponto, 0.005)
            pygame.draw.circle(screen, "red", (ponto.x, ponto.y), (VTX_RADIUS*DIST)//(2*(ponto.z + DIST)))

        time.sleep(0.001)
        pygame.display.flip()


if __name__ == "__main__":
    main()