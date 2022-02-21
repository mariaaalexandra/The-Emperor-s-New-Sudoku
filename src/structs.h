#include "bmp_header.h"

typedef struct
{
    unsigned char **bit_matrix;
    bmp_fileheader f;
    bmp_infoheader i;

}image;
