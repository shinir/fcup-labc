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

PPMIMAGE* addBorder(PPMIMAGE* imgA, char* argv[]) {
  PPMIMAGE *imgB = (PPMIMAGE*) malloc(sizeof(PPMIMAGE));

  int np = atoi(argv[1]);
  int r = atoi(argv[2]);
  int g = atoi(argv[3]);
  int b = atoi(argv[4]);

  imgB->height = imgA->height+2*np;
  imgB->width = imgA->width+2*np;
  imgB->maxVal = imgA->maxVal;

  for(int i=0; i<2; i++)
    imgB->magicNumber[i] = imgA->magicNumber[i];
  imgB->mat = (PPMPIXEL **) malloc(sizeof(PPMPIXEL) * imgB->width * imgB->height);

  for(int i=0; i<imgB->height; i++)
    imgB->mat[i] = (PPMPIXEL *) malloc(imgB->width * sizeof(PPMPIXEL));

  for(int i=0; i<imgB->height; i++) {
    for(int j=0; j<imgB->width; j++) {
      if(j<np || j>=imgB->width-np || i<np || i>=imgB->height-np) {
        imgB->mat[i][j].R = r;
        imgB->mat[i][j].G = g;
        imgB->mat[i][j].B = b;
      }
      else {
        imgB->mat[i][j].R = imgA->mat[i-np][j-np].R;
        imgB->mat[i][j].G = imgA->mat[i-np][j-np].G;
        imgB->mat[i][j].B = imgA->mat[i-np][j-np].B;
      }
    }
  }

  return imgB;
}

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

int main(int argc, char *argv[]) {
  FILE *f1, *f2;

  PPMIMAGE *imgA = (PPMIMAGE *) malloc(sizeof(PPMIMAGE));
  PPMIMAGE *imgB = (PPMIMAGE *) malloc(sizeof(PPMIMAGE));

  if(argc == 7) {
    f1 = fopen(argv[5], "r");
    f2 = fopen(argv[6], "w");
  } else if(argc == 6) {
    f1 = fopen(argv[5], "r");
    f2 = stdout;
  } else if(argc == 5) {
    f1 = stdin;
    f2 = stdout;
  } else {
    failMessage("ERROR: unable to open file!\n");
  }

  readPPM(f1, imgA);
  imgB = addBorder(imgA, argv);
  writeNewImage(imgB,f2);

  fclose(f1);
  fclose(f2);

  free(imgA->mat);
  free(imgA);
  free(imgB->mat);
  free(imgB);
}
