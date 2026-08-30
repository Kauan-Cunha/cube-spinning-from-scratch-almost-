#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/fb.h>

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


int abrir(char* filepath){
    int fp = open(filepath, O_RDWR);
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


int main(){
    int fp = abrir("/dev/fb0");

    struct fb_fix_screeninfo finfo;
    if (-1 == ioctl(fp, FBIOGET_FSCREENINFO, &finfo)) {
        perror("ERROR READING SCREEN INFO");
        exit(EXIT_FAILURE);
    }
    printf("%d", finfo.line_length);
}