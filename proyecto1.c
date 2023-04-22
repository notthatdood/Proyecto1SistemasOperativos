#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <regex.h>
#define TOTAL_THREADS 3

//Made using code adapted from: 
// - https://stackoverflow.com/questions/13131982/create-thread-passing-arguments
// - https://www.ibm.com/docs/en/i/7.2?topic=functions-regcomp-compile-regular-expression
// - https://www.geeksforgeeks.org/regular-expressions-in-c/

struct Lectura {
  char text[1000000];
};

struct PortionStruct {
  char content[100000];
  long int size;
  long int threadNum;
  char expression[100];
};

void readData(char *fileName, struct Lectura *lecData) {
  FILE *f = fopen(fileName, "r");

  if (f != NULL) {
    while (fread(lecData, sizeof(struct Lectura), 1, f))
      ;
    fclose(f);
  } else {
    printf("Error opening the file to read data\n");
    exit(1);
  }
}

long int getFileSize(char *fileName) {
  FILE *f = fopen(fileName, "r");
  if (f != NULL){
    fseek(f, 0L, SEEK_END);
    long int res = ftell(f);
    fclose(f);
    return res - 1;
  }
}

void *PrintHello(char *text) {
  //char did;
  //did = (long)threadid;
  printf("%s\n", text);
  pthread_exit(NULL);
}

void partitionString(long int start, long int portionSize, struct Lectura *source, struct PortionStruct *portion){
  long int end = start+portionSize;
  int j = 0;
  for (long int i=start; i<end; i++){
    portion->content[j]=source->text[i];
    j++;
  }
  //for (long int i = 0; i<portionSize; i++){
  //  printf("%c", portion->content[i]);
  //}
}

void *wordSillas(void *parameters){    
  struct PortionStruct *portion;
  portion = (struct PortionStruct *)parameters;
  
  regex_t preg;
  
  //no se que hacen estos dos------
  size_t     nmatch = 2;
  regmatch_t pmatch[2];
  //-------------------------------
  
  if (regcomp(&preg, portion->expression, 0)){
  	//si retorna algo diferente de 0 es que hubo un error y entra aqui
  	free(portion);
    pthread_exit(NULL);
    return;
  }
  
  int result = regexec;
  
  if (result == 0){
  //Found a match
  printf("Se encontro una coincidencia en la posicion %d \n", pmatch[1].rm_so);
  }else if (result == REG_NOMATCH){
  //Didn't find any matches
  }
  
  free(portion);
  regfree(&preg);
  pthread_exit(NULL);
}

int main() {
  char fileName[] = "archivitodecente.txt";
  FILE *f = fopen(fileName, "r");
  
  //Reading
  struct Lectura lecResult;
  readData(fileName, &lecResult);
  
  //Getting Size
  long int tam = getFileSize(fileName);
  long int portionSize = tam/TOTAL_THREADS;
  
  //char portion[tam];
  //partitionString(0, portionSize, &lecResult, portion);
  pthread_t threads[TOTAL_THREADS];
  int rc;
  long int t;
  
  for(t=0; t<TOTAL_THREADS; t++) {
  	//creating text partition
  	int status;
  	struct PortionStruct *portion = malloc(sizeof *portion);
  	long int startPosition = t * portionSize;
  	partitionString(startPosition, portionSize, &lecResult, portion);
  	portion->size = portionSize;
  	portion->threadNum = t;
  	portion->expression = "abc|def"
  	
  	//creating threads
    printf("In main: creating thread %ld\n", t);
    if (pthread_create(&threads[t], NULL, wordSillas, (void *)portion)) {
      free(portion);
    }
  }
  
  pthread_exit(NULL);
  return 0;
}
