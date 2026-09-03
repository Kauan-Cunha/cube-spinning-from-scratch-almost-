#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/fb.h>
#include <string.h>
#include <math.h>
#include <unistd.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

#define BLACK 0
#define WHITE 0xFFFFFFFF

#define RADIUS 2

typedef struct conn{
    int s, d;
}Conn;

typedef struct coord{
    float x, y, z;
}Coord;

typedef struct point{
    Coord coord;
    int n;
    Coord *r_points; //points of the radiuns (Manhattan Distance)

}Point;

typedef struct shape{
    Point vtxs[8256];
    int nPoints;
    Conn conn[8128];
    int nConn;
}Shape;

Shape initShape(){
    Shape newShape;
    newShape.nPoints = 0;
    return newShape;
}


void addConn(Shape* shape, int src, int dest){
    shape->conn[shape->nConn].s = src;
    shape->conn[shape->nConn].d = dest;
    shape->nConn += 1;
}

Coord init_coord(float x, float y, float z){
    Coord new;
    new.x = x;
    new.y = y;
    new.z = z;
    return new;
}

Point init_point(float x, float y, float z, int radius){
    Point *point = malloc(sizeof(Point));
    point->r_points = malloc(4*radius*radius*sizeof(Coord));
    if (!point || !point->r_points){
        printf("Erro de alocação.\n");
        exit(EXIT_FAILURE);
    }
    
    point->coord = init_coord(x, y, z);
    
    int k = 0;
    for(int i = x - radius; i < x + radius; i++){
        for(int j = y - radius; j < y + radius; j++){
            point->r_points[k] = init_coord(i, j, z);
            k++;
        }
    }
    point->n = k;
    return *point;
}

void addPoint(Shape* shape, float x, float y, float z, int radius){
    shape->vtxs[shape->nPoints] = init_point(x, y, z, radius);
    shape->nPoints += 1;
}

void draw_buffer(int *buff, Shape *shape, int width, int height, int rgb){
    Point *points = shape->vtxs;
    for(int i = 0; i<shape->nPoints; i++){
        for(int j = 0; j < points[i].n; j++){
            int x = (int)points[i].r_points[j].x + (int)width / 2;
            int y = (int)points[i].r_points[j].y + (int)height / 2;
            int offset = y * width + x;
            buff[offset] = rgb;
        }
    }
}

int abrir(char* filepath){
    int fp = open(filepath, O_RDWR | O_TRUNC);
    printf("FP_ID: %d\n", fp);
    if (fp < 0){
        printf("ERRO(id:%d))   ", errno);
        switch (errno)
        {
        case EACCES:
            printf("NÃO HÁ PERMISSÃO DE ACESSO DO DEVICE\n");
            break;
        case EBUSY:
            printf("O DEVICE ESTÁ SENDO UTILIZADO\n");
            break;
        case ENXIO:
            printf("NENHUM DESPOSITIVO NO ENDEREÇO ENCONTRADO\n");
            break;
        case ENOMEM:
            printf("NÃO FOI POSSIVEL ALOCAR MEMÓRIA \n");
            break;
        case EMFILE:
            printf("MUITOS ARQUIVOS ABERTOS \n");
            break;
        case ENFILE:
            printf("MUITOS ARQUIVOS ABERTOS NO SISTEMA\n");
            break;
        case ENOENT:
            printf("NÃO EXISTE ARQUIVO OU DIRETÓRIO DO DEVICE\n");
            break;
        default:
            printf("nenhum");
            break;
        }
    exit(errno);
    }
    printf("Abriu o device com sucesso!\n");
    return fp;
}

void rotateX(Point *point, float teta){
    for(int i = 0; i < point->n; i++){
        float currZ = point->r_points[i].z;
        float currY = point->r_points[i].y;
        point->r_points[i].y = ((currY*cos(teta) - currZ*sin(teta)));
        point->r_points[i].z = ((currY*sin(teta) + currZ*cos(teta)));
    }

    point->coord.y = ((point->coord.y*cos(teta) - point->coord.z*sin(teta)));
    point->coord.z = ((point->coord.y*sin(teta) + point->coord.z*cos(teta)));
}

void rotateY(Point *point, float teta){
    for(int i = 0; i < point->n; i++){
        float currZ = point->r_points[i].z;
        float currX = point->r_points[i].x;
        point->r_points[i].x = ((currX*cos(teta) + currZ*sin(teta)));
        point->r_points[i].z = ((currZ*cos(teta) - currX*sin(teta)));
    }
    point->coord.x = ((point->coord.x*cos(teta) + point->coord.z*sin(teta)));
    point->coord.z = ((point->coord.z*cos(teta) - point->coord.x*sin(teta)));
}

void rotateZ(Point *point, float teta){
    for(int i = 0; i < point->n; i++){
        float currY = point->r_points[i].y;
        float currX = point->r_points[i].x;
        point->r_points[i].x = ((currX*cos(teta) - currY*sin(teta)));
        point->r_points[i].y = ((currX*sin(teta) + currY*cos(teta)));
    }
    point->coord.x = ((point->coord.x*cos(teta) + point->coord.y*sin(teta)));
    point->coord.y = ((point->coord.x*sin(teta) - point->coord.y*cos(teta)));
}

/*
INIT SOME OF THE SHAPES
*/
Shape initCube(float sideSize){
    float half = sideSize/2;
    Shape cube = initShape();
    addPoint(&cube, -half, -half, -half, RADIUS);
    addPoint(&cube, half, -half, -half, RADIUS);
    addPoint(&cube, half, half, -half, RADIUS);
    addPoint(&cube, -half, half, -half, RADIUS);
    addPoint(&cube, -half, -half, half, RADIUS);
    addPoint(&cube, half, -half, half, RADIUS);
    addPoint(&cube, half, half, half, RADIUS);
    addPoint(&cube, -half, half, half, RADIUS);

    addConn(&cube, 0, 1);
    addConn(&cube, 1, 2);
    addConn(&cube, 2, 3);
    addConn(&cube, 3, 0);
    addConn(&cube, 4, 5);
    addConn(&cube, 5, 6);
    addConn(&cube, 6, 7);
    addConn(&cube, 7, 4);
    addConn(&cube, 0, 4);
    addConn(&cube, 1, 5);
    addConn(&cube, 2, 6);
    addConn(&cube, 3, 7);

    return cube;
}

Shape drawLines(Shape *shape, int resolution){
    for(int i = 0; i < shape->nConn; i++){
        float t = 0;
        float step = 1/resolution;
        Point a = shape->vtxs[shape->conn[i].s];
        Point b = shape->vtxs[shape->conn[i].d];

        while (t < 1){
            addPoint(shape, a.coord.x + t*(b.coord.x - a.coord.x), a.coord.y + t*(b.coord.y - a.coord.y), a.coord.z + t*(b.coord.z - a.coord.z),RADIUS);
        }

    }
}
          
void shapeRotate(Shape* shape, float tetaX, float tetaY, float tetaZ){
    for(int k = 0; k < shape->nPoints; k++){
            rotateX(&shape->vtxs[k], tetaX);
            rotateY(&shape->vtxs[k], tetaY);
            rotateY(&shape->vtxs[k], tetaZ);
    }
}

int main(){
    int fp = abrir("/dev/fb0");

    struct fb_var_screeninfo finfo;
    if (-1 == ioctl(fp, FBIOGET_VSCREENINFO, &finfo)) {
        perror("ERROR READING SCREEN INFO");
        exit(EXIT_FAILURE);
    }
    printf("\nINFORMAÇÕES:\nWIDTH: %dpx\nHEIGHT: %dpx\nBITS_PER_PIXEL: %dbits\n\n", finfo.xres, finfo.yres, finfo.bits_per_pixel);
    int fb_width = finfo.xres;
    int fb_height = finfo.yres;
    int fb_bpp = finfo.bits_per_pixel;
    int fb_bytes = fb_bpp / 8;

    
    int *buffer = malloc(sizeof(int)*fb_height*fb_width);
    Shape cube = initCube(400);
    drawLines(&cube, 4);

    int n = 0;
    while (1){
        //draw new frame
        draw_buffer(buffer, &cube, fb_width, fb_height, WHITE);
        lseek(fp, 0, SEEK_SET);
        write(fp, buffer, 1920*1080*4);
        usleep(10000);
        //cleanscreen
        draw_buffer(buffer, &cube, fb_width, fb_height,  BLACK);
        lseek(fp, 0, SEEK_SET);
        write(fp, buffer, 1920*1080*4);

        //rodando
        shapeRotate(&cube, 0.005, 0.005, 0.005);
    }
}