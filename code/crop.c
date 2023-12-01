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


//WRITE image
void writeNewImage(PPMIMAGE *imgA, FILE *f2, char* argv[]) {
  int a = atoi(argv[1]);
  int b = atoi(argv[2]);
  int c = atoi(argv[3]);
  int d = atoi(argv[4]);

  if(c>=imgA->width)
    c = imgA->width-1;
  if(d>=imgA->height)
    d = imgA->height-1;

  int w = c-a+1;
  int h = d-b+1;

  fprintf(f2, "%s%d %d\n%d\n", imgA->magicNumber, w, h, imgA->maxVal);

  for(int i=b; i<=d; i++) {
    for(int j=a; j<=c; j++) {
      fprintf(f2, "%d %d %d\n", imgA->mat[i][j].R, imgA->mat[i][j].G, imgA->mat[i][j].B);
    }
  }
}

//FUNCTION main
int main(int argc, char *argv[]) {
  FILE *f1, *f2;

  PPMIMAGE *imgA = (PPMIMAGE *) malloc(sizeof(PPMIMAGE));

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
  writeNewImage(imgA,f2,argv);

  fclose(f1);
  fclose(f2);

  free(imgA->mat);
  free(imgA);
}
