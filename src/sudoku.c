#include "cJSON.h"
#include <stdio.h>
#include <stdlib.h>
#include "structs.h"
#include <string.h>


//functie de parsare
image *read_info(char *array) 
{   
    //parsez fiecare camp din structura de tip image si atribui valoarea
    image *bmp = calloc(1, sizeof(image));
    if (bmp == NULL) {
        fprintf(stderr, "Couldn't allocate\n");
        exit(1);
    }

    cJSON *picture = cJSON_Parse(array);

    cJSON *file_h = cJSON_GetObjectItem(picture, "file_header");

    cJSON *set = cJSON_GetObjectItem(file_h, "offset");
    cJSON *sign = cJSON_GetObjectItem(file_h, "signature");
    cJSON *reserve = cJSON_GetObjectItem(file_h, "reserved");
    cJSON *f_size = cJSON_GetObjectItem(file_h, "file_size");

    bmp->f.imageDataOffset = set->valueint;
    bmp->f.fileMarker1 = sign->valuestring[0];
    bmp->f.fileMarker2 = sign->valuestring[1];
    bmp->f.unused1 = (unsigned short)(reserve->valueint);
    bmp->f.unused2 = (unsigned short)(reserve->valueint);
    bmp->f.bfSize = f_size->valueint;

    cJSON *info_h = cJSON_GetObjectItem(picture, "info_header");

    cJSON *color_u = cJSON_GetObjectItem(info_h, "colors_used");
    cJSON *i_size = cJSON_GetObjectItem(info_h, "size");
    cJSON *color_i = cJSON_GetObjectItem(info_h, "colors_important");
    cJSON *img_size = cJSON_GetObjectItem(info_h, "image_size");
    cJSON *y = cJSON_GetObjectItem(info_h, "y_pixels_per_meter");
    cJSON *x = cJSON_GetObjectItem(info_h, "y_pixels_per_meter");
    cJSON *w = cJSON_GetObjectItem(info_h, "width");
    cJSON *h =cJSON_GetObjectItem(info_h, "height");
    cJSON *p = cJSON_GetObjectItem(info_h, "planes");
    cJSON *b_count = cJSON_GetObjectItem(info_h, "bit_count");
    cJSON *comp = cJSON_GetObjectItem(info_h, "compression");

    bmp->i.biClrUsed = color_u->valueint; 
    bmp->i.biSize = i_size->valueint;
    bmp->i.biClrImportant = color_i->valueint;
    bmp->i.biSizeImage = img_size->valueint;
    bmp->i.biXPelsPerMeter = y->valueint;
    bmp->i.biXPelsPerMeter = x->valueint;
    bmp->i.width = w->valueint;
    bmp->i.height = h->valueint;
    bmp->i.planes = p->valueint;
    bmp->i.bitPix = b_count->valueint;
    bmp->i.biCompression = comp->valueint;

    cJSON *byte = NULL;
    cJSON *bitmap = cJSON_GetObjectItem(picture, "bitmap");

    //Aloc matricea de octeti si verific daca s-a realizat cu succes
    bmp->bit_matrix = calloc(bmp->i.height, sizeof(unsigned char *));
    if (bmp->bit_matrix == NULL) {
        fprintf(stderr, "Couldn't allocate\n");
        exit(1);
    }

    for (int i = 0; i < bmp->i.height; i++) {
        bmp->bit_matrix[i] = calloc(bmp->i.width * 3, sizeof(unsigned char *));
        if (bmp->bit_matrix == NULL) {
            fprintf(stderr, "Couldn't allocate\n");
            exit(1);
        }    
    }

    int line = 0, col = 0;
    cJSON_ArrayForEach(byte, bitmap) {
        bmp->bit_matrix[line][col] = (unsigned char)(byte->valueint);
        col++;
        if (col == bmp->i.width * 3) {
            line++;
            col = 0;
        }
    }

    cJSON_Delete(picture);

    return bmp;
}

//functie afisare structura bmp in filename
void print_info(image *bmap, char *filename)
{
    //Deschid fisierul si verific daca s-a realizat cu succes
    FILE *out = fopen(filename, "wb");
    if (out == NULL) {
        fprintf(stderr, "Can't open file\n");
        exit(2);
    }

    //afisez fiecare camp al structurii de tip image
    fwrite(&(bmap->f), sizeof(bmp_fileheader), 1, out);
    fwrite(&(bmap->i), sizeof(bmp_infoheader), 1, out);

    //afisez fiecare element
    for (int i = bmap->i.height - 1; i >= 0; i--) {
        fwrite(bmap->bit_matrix[i], sizeof(unsigned char),
        bmap->i.width * 3, out);
        fflush(out);
        fseek(out, 1, SEEK_CUR);
    }


    fseek(out, -1, SEEK_CUR);
    char eof = 0;
    fwrite(&eof, sizeof(char), 1, out);
    fclose(out);
    return;
}

//functie de creare fisier output
char *file (char *string, int task)

{   
    char *nr2;

    //aloc dinamic verific daca alocarea a avut loc cu succes
    char *output = calloc(30, sizeof(char));
    if (output == NULL) {
        fprintf(stderr, "Couldn't allocate\n");
        exit(1);        
    }

    //verific daca am ajuns la bonus sau nu
    if (string[12] != 's') {

        //retin valoarea lui board
        nr2 = string + 19;
        nr2[2] = '\0';
        
        //memorez in variabila output formatul cerut de fisier
        if (task != 3) {
            sprintf(output, "output_task%d_board%s.bmp", task, nr2);
        }
        else {
            sprintf(output, "output_task%d_board%s.json", task, nr2);
        }
    }
    else {
        nr2 = string + 19;
        nr2[2] = '\0';
        
        sprintf(output, "output_bonus_board%s.bmp", nr2);
    }

    //returnez sirul de caractere ce reprezinta numele fisierului
    return output;
}

//functie de oglindire a cifrelor
void mirror (unsigned char **matrix)
{
    int i, j, line, column, y, x, k;

    //parcurg fiecare casuta cu numere
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            line = i * 8 + 2;
            column = j * 8 + 2;
            //interschimb primii doi pixeli cu ultimii doi
            for (y = 0; y < 5; y++) {
                for (x = 0; x < 2; x++) {
                    for (k = 0; k < 3; k++) {
                        unsigned char aux =
                        matrix[line + y][column * 3 + x * 3 + k];
                        
                        matrix[line + y][column * 3 + x * 3 + k] =
                        matrix[line + y][column * 3 + (4 - x) * 3 + k];
                        
                        matrix[line + y][column * 3 + (4 - x) * 3 + k] = aux;
                    }
                }
            }
        }
    }
    return;
}

//functie creare matrice cu numere intregi
int **mapping (unsigned char **matrix)
{
    int i, j, k, x, y, line, column, **mat_int;

    //aloc dinamic o matrice de numere intregi si verific alocarea
    mat_int = calloc(9, sizeof(int *));
    if (mat_int == NULL) {
        fprintf(stderr, "Couldn't allocate\n");
        exit(1);
    }

    for (i = 0; i < 9; i++) {
        mat_int[i] = calloc(9, sizeof(int));
        if (mat_int[i] == NULL) {
            fprintf(stderr, "Couldn't allocate\n");
            exit(1);
        }
    }

    //in matricea de caractere matr retin octetii pentru fiecare cifra
    char matr[9][26] = {
        "aaaaraaaaraaaaraaaaraaaar",
        "rrrrraaaarrrrrrraaaarrrrr",
        "rrrrraaaarrrrrraaaarrrrrr",
        "raaarraaarrrrrraaaaraaaar",
        "rrrrrraaaarrrrraaaarrrrrr",
        "rrrrrraaaarrrrrraaarrrrrr",
        "rrrrraaaaraaaaraaaaraaaar",
        "rrrrrraaarrrrrrraaarrrrrr",
        "rrrrrraaarrrrrraaaarrrrrr"
    };

    char current[26];

    //parcurg fiecare casuta ce contine cifre
    for (i = 0; i < 9; i++) {
        for (j = 0; j < 9; j++) {
            line = i * 8 + 2;
            column = j * 8 + 2;
            for (y = 0; y < 5; y++) {
                for (x = 0; x < 5; x++) {
                    /*verific daca pixelul este alb sau roz
                    si actualizez in vectorul current valoarea*/
                    if (matrix[line + y][column * 3 + x * 3] == 255) {
                        current[y * 5 + x] = 'a';
                    }
                    else {
                        current[y * 5 + x] = 'r';
                    }
                }
            }

            current[25] = '\0';
            for (k = 0; k < 9; k++) {
                /*verific daca sirul de caractere coincide cu o
                linie din matricea matr si adaug valoarea intreaga
                in matricea matr*/
                if (strcmp(current, matr[k]) == 0) {
                    mat_int[i][j] = k + 1;
                    break;
                }
            }
        }
    }

    return mat_int;
}

//functie de verificare daca jocul respecta regulile
int verify (int ** mat_int)
{
    int verif = 1, i, j, k;
    /*verific daca cifrele din matrice
    respecta regulile jocului*/
    for (i = 0; i < 9; i ++) {
        for (j = 0; j < 9; j++) {
            if (mat_int[i][j] == 0) {
                verif = 0;
                break;
            }
            for (k = j + 1; k < 9; k++) {
                if (mat_int[i][j] == mat_int[i][k] || mat_int[j][i]
                == mat_int[k][i]) {
                    verif = 0;
                    break;
                }
            }
            if (verif == 0) {
                break;
            }
        }
        if (verif == 0) {
            break;
        }
    
    }

    /*returnez valoarea de adevar care indica
    daca matricea tabla e corecta sau nu*/
    return verif;
}

//creez fisierul de tip json
void task3(char *string, int logic, char *final)
{
    cJSON *input_file = NULL;
    cJSON *game_state = NULL;
    char *print = NULL, *file = NULL, *af;
    file = calloc(15, sizeof(char));
    af = calloc(15, sizeof(char));
    

    cJSON *result = cJSON_CreateObject();
    if (result == NULL) {
        exit(3);
    }

    strcpy(file, string + 14);
    sprintf(af, "%s.json", file);
    input_file = cJSON_CreateString(af);
    
    cJSON_AddItemToObject(result, "input_file", input_file);
    if (logic == 1) {
        game_state = cJSON_CreateString("Win!");
    }
    else {
        game_state = cJSON_CreateString("Loss :(");
    }
    cJSON_AddItemToObject(result, "game_state", game_state);
    print = cJSON_Print(result);
    if (string == NULL)
    {
        fprintf(stderr, "Failed to print monitor.\n");
    }
    FILE *out = fopen(final, "wt");
    fprintf(out, "%s\n", print);
    fclose(out);
    free(af);
    free(file);
    free(print);
    cJSON_Delete(result);
    return;
}

//functie citire din bmp
image *read_bmp(char *filename)
{   
    //deschid fisierul si verific operatia
    FILE *in = fopen(filename, "rb");
    fflush(stdout);
    if (in == NULL) {
        fprintf(stderr, "Can't open file\n");
        exit(2);
    }

    image *bmp = calloc(1, sizeof(image));
    if (bmp == NULL) {
        fprintf(stderr, "Couldn't allocate\n");
        exit(1);
    }

    //citesc fiecare camp din structura de tip image
    fread(&(bmp->f), sizeof(bmp_fileheader), 1, in);
    fread(&(bmp->i), sizeof(bmp_infoheader), 1, in);

    bmp->bit_matrix = calloc(bmp->i.height, sizeof(unsigned char *));
    if (bmp->bit_matrix == NULL) {
        fprintf(stderr, "Couldn't allocate\n");
        exit(1);
    }

    for (int i = 0; i < bmp->i.height; i++) {
        bmp->bit_matrix[i] = calloc(bmp->i.width * 3, sizeof(unsigned char *));
        if (bmp->bit_matrix == NULL) {
            fprintf(stderr, "Couldn't allocate\n");
            exit(1);
        }    
    }

    int padding = bmp->i.biSizeImage / bmp->i.height - bmp->i.width * 3;

    for (int i = bmp->i.height - 1; i >= 0; i--) {
        fread(bmp->bit_matrix[i], sizeof(unsigned char), bmp->i.width * 3, in);
        fseek(in, padding, SEEK_CUR);
    }

    fclose(in);

    return bmp;

}

//functie verificare casuta goala
int is_empty (int line, int col, int **matrix)
{
    if (matrix[line][col] == 0) {
       return 1;
    }
    else {
        return 0;
    }
}

//functie verificare reguli joc
int rules(int line, int column, int n, int **matr)
{
    int logic, i, j;
    //verific pe orizontala
    for (i = 0; i < 9; i++) {
        if (matr[line][i] == n) {
            logic = 0;
            return logic;
        }
    }

    //verific pe verticala
    for (i = 0; i < 9; i++) {
        if (matr[i][column] == n) {
            logic = 0;
            return logic;
        }
    }

    //verific in grup de 9 casute
    for (i = 0; i < 3; i++) {
        for(j = 0; j < 3; j++) {
            if(matr[line - line % 3 + i][column - column % 3 + j] == n) {
                logic = 0;
                return logic;
            }
        }
    }
    logic = 1;
    return logic;
}

//functie adaugare numar
void add (int x, int y, int res, int **mat_int, unsigned char **matrix)
{
    //element este numarul ce trebuie adaugat
    int element = mat_int[x][y], i, j, line, col;
    line = x * 8 + 2;
    col = y * 8 + 2;

    //codificari cifre
    char matr[10][26] = {
        "aaaamaaaamaaaamaaaamaaaam",
        "mmmmmaaaammmmmmmaaaammmmm",
        "mmmmmaaaammmmmmaaaammmmmm",
        "maaammaaammmmmmaaaamaaaam",
        "mmmmmmaaaammmmmaaaammmmmm",
        "mmmmmmaaaammmmmmaaammmmmm",
        "mmmmmaaaamaaaamaaaamaaaam",
        "mmmmmmaaammmmmmmaaammmmmm",
        "mmmmmmaaammmmmmaaaammmmmm",
        "raaarararaaaraaarararaaar"
    };

    // modificare octeti in functie de culoare
    if (res == 0) {
        for (i = 0; i < 5; i++) {
            for (j = 0; j < 5; j++) {
                if (matr[9][i * 5 + j] == 'r') {
                    matrix[line + i][col * 3 + j * 3] = 0;
                    matrix[line + i][col * 3 + j * 3 + 1] = 0;
                    matrix[line + i][col * 3 + j * 3 + 2] = 255;
                }
                else {
                    matrix[line + i][col * 3 + j * 3] = 255;
                    matrix[line + i][col * 3 + j * 3 + 1] = 255;
                    matrix[line + i][col * 3 + j * 3 + 2] = 255;
                }

            }
        }
        return;
    }

    for (i = 0; i < 5; i++) {
        for (j = 0; j < 5; j++) {
            if (matr[element - 1][i * 5 + j] == 'm') {
                matrix[line + i][col * 3 + j * 3 + 1] = 0;
            }

        }
    }

    return;
}

//backtracking
int sudoku(int line, int column, int **matr, image *bmp)
{
    int i;

    if (line == 8 && column == 9) {
        return 1;
    }

    if (column == 9) {
        return sudoku(line + 1, 0, matr, bmp);
    }

    if (matr[line][column] != 0) {
      return sudoku(line, column + 1, matr, bmp);
    }

    //se verifica regulile si se adauga numarul daca este corespunzator
    if (matr[line][column] == 0) {
        for (i = 1; i <= 9; i++) {
            if (rules(line, column, i, matr)) {
                matr[line][column] = i;
                if (sudoku(line, column + 1, matr, bmp)) {
                    add(line, column, 1, matr, bmp->bit_matrix);
                    return 1;
                }
            }
            matr[line][column] = 0;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    image *bmp;
    int **mat_int;

    //daca este task1 se fac toate cele 3 taskuri odata
    if (strstr(argv[1], "task1")) {
        //deschid fisierul si verific operatia
        FILE *input = fopen(argv[1], "rt");
        if (input == NULL) {
            fprintf(stderr, "Can't open file\n");
            exit(2);
        }
        char *filename = file(argv[1], 1);

        char *input_char = calloc(700000, sizeof(char));
        if (input_char == NULL) {
            fprintf(stderr, "Couldn't allocate\n");
            exit(1);
        }

        //citesc din fisier
        fscanf(input, "%s\n", input_char);
        fclose(input);
        
        bmp = read_info(input_char);

        print_info(bmp, filename);
        free(filename);

        mirror(bmp->bit_matrix);
        filename = file(argv[1], 2);

        print_info(bmp, filename);
        free(filename);

        mat_int = mapping(bmp->bit_matrix);
        int validation = verify(mat_int);
        filename = file(argv[1], 3);
        task3(argv[1], validation, filename);
        free(filename);

        free(input_char);
    }
    else if (strstr(argv[1], "task4")) {
        bmp = read_bmp(argv[1]);
        char *filename = file(argv[1], 4);
        mat_int = mapping(bmp->bit_matrix);
        
        sudoku(0, 0, mat_int, bmp);
        print_info(bmp, filename);
        free(filename);
    }
    else {
        bmp = read_bmp(argv[1]);
        char *filename = file(argv[1], 5);
        mat_int = mapping(bmp->bit_matrix);

        int res = sudoku(0, 0, mat_int, bmp);
        if (res) {
            print_info(bmp, filename);
            free(filename);
        }
        else {
            // functie care pune X rosu pe toate casutele
            for (int i = 0; i < 9; i++) {
                for (int j = 0; j < 9; j++) {
                    add(i, j, 0, mat_int, bmp->bit_matrix);
                }
            }
            print_info(bmp, filename);
            free(filename);
        }
    }

    //dezaloc matricea
    for (int i = 0; i < 9; i++) {
        free(mat_int[i]);
    }

    free(mat_int);
    for (int i = 0; i < bmp->i.height; i++) {
        free(bmp->bit_matrix[i]);
    }

    free(bmp->bit_matrix);
    free(bmp);

    return 0;
}


