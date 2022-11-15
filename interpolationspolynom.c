#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>

// Muster Ausgae für input.dat
// 1.0  1.1
// 2.5 -0.5
// 3.4  2.0
// 4.0 -1.0
// 4.5  1.1
// (py-venv3.5.6) codem@thinkpad:~/work/ModSim/2020SS/ub1/src$ ./interpolationspolynom
//
//   Ausgabe der dividierten Differenzen D:
//  1.10000
//  -0.50000  -1.06667
//  2.00000  2.77778  1.60185
//  -1.00000  -5.00000  -5.18519  -2.26235
//  1.10000  4.20000  8.36364  6.77441  2.58193
//
// f(x) = +a_0+a_1*(x-x0)+a_2*(x-x0)*(x-x1)+a_3*(x-x0)*(x-x1)*(x-x2)+a_4*(x-x0)*(x-x1)*(x-x2)*(x-x3)
// f(x) = +1.100000+-1.066667*(x-1.000000)+1.601852*(x-1.000000)*(x-2.500000)+-2.262346*(x-1.000000)*(x-2.500000)*(x-3.400000)+2.581930*(x-1.000000)*(x-2.500000)*(x-3.400000)*(x-4.000000)

//

size_t getline(char **lineptr, size_t *n, FILE *stream)
{
  char *bufptr = NULL;
  char *p = bufptr;
  size_t size;
  int c;

  if (lineptr == NULL)
  {
    return -1;
  }
  if (stream == NULL)
  {
    return -1;
  }
  if (n == NULL)
  {
    return -1;
  }
  bufptr = *lineptr;
  size = *n;

  c = fgetc(stream);
  if (c == EOF)
  {
    return -1;
  }
  if (bufptr == NULL)
  {
    bufptr = malloc(128);
    if (bufptr == NULL)
    {
      return -1;
    }
    size = 128;
  }
  p = bufptr;
  while (c != EOF)
  {
    if ((p - bufptr) > (size - 1))
    {
      size = size + 128;
      bufptr = realloc(bufptr, size);
      if (bufptr == NULL)
      {
        return -1;
      }
    }
    *p++ = c;
    if (c == '\n')
    {
      break;
    }
    c = fgetc(stream);
  }

  *p++ = '\0';
  *lineptr = bufptr;
  *n = size;

  return p - bufptr - 1;
}

// In dieser Funktion werden die Wertepaare abgezählt,
// indem die entsprechenden Zeilen gezählt werden.
//
// Die Ausgabe dieser Funktion ist gleich der Anzahl der Wertepaare.
int getNumberOfPoints(char *name)
{
  FILE *fp;
  char *line = NULL;
  size_t len = 0;

  if ((fp = fopen(name, "r")) == NULL)
  {
    exit(EXIT_FAILURE);
  }

  int cnt = 0;
  while (getline(&line, &len, fp) != -1)
  {
    cnt++;
  }

  free(line);

  return cnt;
}

// In dieser Funktion werden die Wertepaare eingelesen und
// in Form von Arrays x[N] und y[N] übergeben.
void readFile(char *name, double x[], double y[])
{
  FILE *fp;
  char *line = NULL;
  size_t len = 0;

  if ((fp = fopen(name, "r")) == NULL)
  {
    exit(EXIT_FAILURE);
  }

  int cnt = 0;
  while (getline(&line, &len, fp) != -1)
  {
    sscanf(line, "%lf %lf", &x[cnt], &y[cnt]);
    cnt++;
  }

  free(line);
}

double secantOfTwoPoints(double x1, double y1, double x2, double y2)
{
  return ((y2 - y1) / (x2 - x1));
}

int main(int argc, char *argv[])
{
  // Abzählen der Wertepaare.
  int N = getNumberOfPoints("input.dat");

  double x[N]; // Vektor für die Abstände der Messungen
  double y[N]; // Vektor für die gemessenen Werte

  // Einlesen der Daten.
  readFile("input.dat", x, y);

  // Laufvariablen
  int i, j;
  double D[N][N]; // für eine Matrix der Dimnsion NxN

  // Berechnen Sie die dividierten Differenzen:
  for (i = 0; i < N + 1; i++)
  {
    printf("i = %4d \n", i);
    D[i][0] = y[i];
    printf("D[%i][0] = %.2f \n", i, D[i][0]);
    for (j = 1; j<i+1; j++){
      D[i][j] = secantOfTwoPoints(x[i-j],D[i-1][j-1],x[i],D[i][j-1]) ;
    }
  }
  // Geben Sie hier die dividierten Differenzen in Form einer Matrix aus:
  printf("\n  Ausgabe der dividierten Differenzen D: \n");
  for (i=0; i<N; i++){
    for (j=0; j<N; j++){
      printf("%.5f, ",D[i][j]);
    }
  printf("\n");
  }
  /*********************************************************************/
  /* Zu Erinnerung:                                                    */
  /* %3d     - Ganzzahl mit 3 Stellen                                  */
  /* %.14lf  - Fließkommazahl mit 14 Nachkommastellen                  */
  /* \n      - Neue Zeile                                              */
  /* Beispiel: printf("%3d: % .2lf % .2lf\n", Variablen);              */
  /*          000: -0.01  0.01                                         */
  /*********************************************************************/

  // Weisen Sie dem Vektor a die Polynomkoeffizienten aus den entsprechenden Einträgen der Matrix D zu:
  double a[N]; // Vektor der Länge N
 for (i=0;i<N+1;i++){
  a[i] = D[i][i]; 
 }
  // Geben Sie das Polynom aus:

  int useplotter = 1;

  if (useplotter)
  {
    // Plotten des Polynoms
    FILE *gp = popen("gnuplot -p", "w");
    fprintf(gp, "set key right bottom box; set xrange [0:5]; set yrange [-9:3.5]; set xlabel \"x\"; set ylabel \"y\";\n");
    fprintf(gp, "f(x) = ");
    for (i = 0; i < N; i++)
    {
      fprintf(gp, "+%lf", a[i]);
      for (j = 0; j < i; j++)
      {
        fprintf(gp, "*(x-%lf)", x[j]);
      }
    }
    fprintf(gp, ";\n");
    fprintf(gp, "plot f(x) lc 3 ti \"Interpolation\", \"input.dat\" lc 4 ps 3 lw 2 ti \"data\";\n");

    pclose(gp);
  }

  return 0;
}
