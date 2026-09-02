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

#define RADIUS 3


// struct v4l2_pix_format
// {
//     __u32 width;
//     __u32 height;
//     __u32 pixelformat;
    
//     enum vl
// };


// struct v4l2_framebuffer{
//     __u32 capability;
//     __u32 flags;
//     void *base; //posição inicio do buffer (pixel superior esquerdo)
    

// }
typedef struct coord{
    float x, y, z;
}Coord;

typedef struct point
{
    Coord coord;
    int n;
    Coord *r_points; //points of the radiuns (Manhattan Distance)

}Point;

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

void draw_buffer(int *buff, Point *points, int n, int width, int height, int rgb){
    for(int i = 0; i<n; i++){
        for(int j = 0; j < points[i].n; j++){
            int x = (int)points[i].r_points[j].x + (int)width / 2;
            int y = (int)points[i].r_points[j].y + (int)height / 2;
            int offset = y * width + x;
            buff[offset] = rgb;
            // lseek(fd, offset, SEEK_SET);
            // write(fd, rgb, 4);
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

    Point points[8];
    points[0] = init_point(-200, 0, -200,RADIUS);
    points[1] = init_point(0, -200, -200,RADIUS);
    points[2] = init_point(200, 0, -200,RADIUS);
    points[3] = init_point(0, 200, -200,RADIUS);
    points[4] = init_point(-200, 0, 200,RADIUS);
    points[5] = init_point(0, -200, 200,RADIUS);
    points[6] = init_point(200, 0, 200,RADIUS);
    points[7] = init_point(0, 200, 200,RADIUS);

    int buffer[1920*1080];
    // int clean_buffer[1920*1080];
    
    int n = 0;
    while (1){
        //draw new frame
        draw_buffer(buffer, points, 8, 1920, 1080, WHITE);
        lseek(fp, 0, SEEK_SET);
        write(fp, buffer, 1920*1080*4);
        usleep(10000);
        //cleanscreen
        draw_buffer(buffer, points, 8, 1920, 1080,  BLACK);
        lseek(fp, 0, SEEK_SET);
        write(fp, buffer, 1920*1080*4);
        for(int k = 0; k < 8; k++){
            // printf("entrou");
            rotateX(&points[k], 0.005);
            rotateY(&points[k], 0.005);
            rotateY(&points[k], 0.005);
        }
    }
}