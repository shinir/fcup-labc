#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct ppmPixel {
  int R;
  int G;
  int B;
};
typedef struct ppmPixel PPMPIXEL;

struct ppmImage {
  char magicNumber[3]; //ID file P3
  int width;
  int height;
  int maxVal;
  PPMPIXEL **mat;      //MATRIX (pixel img)
};
typedef struct ppmImage PPMIMAGE;


void failMessage(char *c) {
  printf("%s", c);
  exit(1);
}

PPMIMAGE* readPPM(FILE *f1, PPMIMAGE* imgA) {
  char magicNumber[4];

  // read magic number
  fgets(magicNumber, 4, f1);
  strcpy(imgA->magicNumber, magicNumber);
  //READ size
  fscanf(f1, "%d %d\n", &imgA->width, &imgA->height);

  fscanf(f1, "%d\n", &imgA->maxVal);
  if(imgA->maxVal != 255) {
    failMessage("ERROR 1: invalid max value color!\n");
  }
  //ALLOC memory
  imgA->mat = (PPMPIXEL**) malloc(imgA->width * imgA->height * sizeof(PPMPIXEL));
  for(int i=0; i<imgA->height; i++)
    imgA->mat[i] = (PPMPIXEL *) malloc(imgA->width * sizeof(PPMPIXEL));

  if(!imgA) {
    failMessage("ERROR 1: unable to allocate memory!\n");
  }

  //READ pixel
  for(int i=0; i<imgA->height; i++) {
    for(int j=0; j<imgA->width; j++) {
      fscanf(f1, "%d %d %d", &imgA->mat[i][j].R, &imgA->mat[i][j].G, &imgA->mat[i][j].B);
    }
  }

  return imgA;
}

PPMIMAGE* rotateImage(PPMIMAGE *imgA) {
  PPMIMAGE *imgB = (PPMIMAGE*) malloc(sizeof(PPMIMAGE));

  //int size = sizeof(PPMPIXEL) * imgA->width * imgA->height;
  imgB->height = imgA->height;
  imgB->width = imgA->width;
  imgB->maxVal = imgA->maxVal;

  for(int i = 0; i < 2; i++)
    imgB->magicNumber[i] = imgA->magicNumber[i];
  imgB->mat = (PPMPIXEL **) malloc(sizeof(PPMPIXEL) * imgA->width * imgA->height);

  for(int i=0; i<imgB->height; i++)
    imgB->mat[i] = (PPMPIXEL *) malloc(imgB->width * sizeof(PPMPIXEL));

  for(int i=0; i<imgA->height; i++) {
    for(int j=0; j<imgA->width; j++) {
      imgB->mat[i][j].R = imgA->mat[i][imgA->width-1-j].R;
      imgB->mat[i][j].G = imgA->mat[i][imgA->width-1-j].G;
      imgB->mat[i][j].B = imgA->mat[i][imgA->width-1-j].B;
    }
  }

  return imgB;
}

//WRITE image
void writeNewImage(PPMIMAGE *imgB, FILE *f2) {
  int i, j;
  fprintf(f2, "%s\n%d %d\n%d\n", imgB->magicNumber, imgB->width, imgB->height, imgB->maxVal);

  // write pixels
  for(i=0; i<imgB->height; i++) {
    for(j=0; j<imgB->width; j++) {
      fprintf(f2, "%d %d %d\n", imgB->mat[i][j].R, imgB->mat[i][j].G, imgB->mat[i][j].B);
    }
  }
}

//FUNCTION main
int main(int argc, char *argv[]) {
  FILE *f1, *f2;

  PPMIMAGE *imgA = (PPMIMAGE *) malloc(sizeof(PPMIMAGE));
  PPMIMAGE *imgB = (PPMIMAGE *) malloc(sizeof(PPMIMAGE));

  if(argc == 3) {
    f1 = fopen(argv[1], "r");
    f2 = fopen(argv[2], "w");
  } else if(argc == 2) {
    f1 = fopen(argv[1], "r");
    f2 = stdout;
  } else if(argc == 1) {
    f1 = stdin;
    f2 = stdout;
  } else {
    failMessage("ERROR: unable to open file!\n");
  }

  readPPM(f1, imgA);
  imgB = rotateImage(imgA);
  writeNewImage(imgB,f2);

  fclose(f1); //CLOSE and disassociates file
  fclose(f2); //CLOSE and disassociates file

  free(imgA->mat);
  free(imgA);
  free(imgB->mat);
  free(imgB);
}
