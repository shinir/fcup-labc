#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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

PPMIMAGE* concat(PPMIMAGE *imgA, PPMIMAGE *imgB) {
  PPMIMAGE *imgC = (PPMIMAGE*) malloc(sizeof(PPMIMAGE));
  int w = imgA->width+imgB->width;
  int h;

  if(imgA->height > imgB->height)
    h = imgB->height;
  else
    h = imgA->height;

  imgC->height = h;
  imgC->width = w;
  imgC->maxVal = imgA->maxVal;

  for(int i = 0; i < 2; i++)
    imgC->magicNumber[i] = imgA->magicNumber[i];
  imgC->mat = (PPMPIXEL **) malloc(sizeof(PPMPIXEL) * w * h);

  for(int i=0; i<imgC->height; i++)
    imgC->mat[i] = (PPMPIXEL *) malloc(w * sizeof(PPMPIXEL));

//        k=imgA->width-imgB->width-2;
  for(int i=0; i<h; i++) {
    int k=0;
    for(int j=0; j<w; j++) {
      if(j<imgA->width) {
        imgC->mat[i][j].R = imgA->mat[i][j].R;
        imgC->mat[i][j].G = imgA->mat[i][j].G;
        imgC->mat[i][j].B = imgA->mat[i][j].B;
      }
      else {
        imgC->mat[i][j].R = imgB->mat[i][k].R;
        imgC->mat[i][j].G = imgB->mat[i][k].G;
        imgC->mat[i][j].B = imgB->mat[i][k].B;
        k++;
      }
    }
  }

  return imgC;
}

//WRITE image
void writeNewImage(PPMIMAGE *imgC, FILE *f3) {
  int i, j;
  fprintf(f3, "%s\n%d %d\n%d\n", imgC->magicNumber, imgC->width, imgC->height, imgC->maxVal);

  // write pixels
  for(i=0; i<imgC->height; i++) {
    for(j=0; j<imgC->width; j++) {
      fprintf(f3, "%d %d %d\n", imgC->mat[i][j].R, imgC->mat[i][j].G, imgC->mat[i][j].B);
    }
  }
}

//FUNCTION main
int main(int argc, char *argv[]) {
  FILE *f1, *f2, *f3;

  PPMIMAGE *imgA = (PPMIMAGE *) malloc(sizeof(PPMIMAGE));
  PPMIMAGE *imgB = (PPMIMAGE *) malloc(sizeof(PPMIMAGE));
  PPMIMAGE *imgC = (PPMIMAGE *) malloc(sizeof(PPMIMAGE));

  if(argc == 4) {
    f1 = fopen(argv[1], "r");
    f2 = fopen(argv[2], "r");
    f3 = fopen(argv[3], "w");
  } else if(argc == 3) {
    f1 = fopen(argv[1], "r");
    f2 = fopen(argv[2], "r");
    f3 = stdout;
  } else if(argc == 2) {
    f1 = stdin;
    f3 = stdout;
  } else {
    failMessage("ERROR: unable to open file!\n");
  }


  readPPM(f1, imgA);
  readPPM(f2, imgB);
  imgC = concat(imgA, imgB);
  writeNewImage(imgC,f3);

  //rewind(f1);
  fclose(f1);
  fclose(f2);
  fclose(f3);

  free(imgA->mat);
  free(imgA);
  free(imgB->mat);
  free(imgB);
  free(imgC->mat);
  free(imgC);
}
