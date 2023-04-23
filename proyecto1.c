#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <regex.h>
#define TOTAL_THREADS 4

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
}

void cutString(long int start, struct PortionStruct *portion, char *copy){
  int j = 0;
  for (long int i=start; i<portion->size; i++){
    copy[j]=portion->content[i];
    j++;
  }
}

void copyString(int size, char *source, char *result){
  for (int i=0; i<size; i++){
    result[i]=source[i];
  }
}


void *wordSillas(void *parameters ){    
  struct PortionStruct *portion;
  portion = (struct PortionStruct *)parameters;
  
  long int inicio;
  long int final;
  long int cutCounter = 0;
  int hasMatch = 0;
  regex_t preg;
  size_t nmatch = 2;
  regmatch_t pmatch[2];
  char portionCopy[portion->size];
  copyString(portion->size, portion->content, portionCopy);
  //fgets(portion->content,300,); FUNCION PARA LEER POR PAGINA
  
  regcomp(&preg, portion->expression, REG_EXTENDED);
 	
  while(1){
  
    hasMatch = regexec(&preg, portionCopy, nmatch, pmatch, 0);
  
    inicio =  (pmatch[0].rm_so) + portion->size * portion->threadNum;
    final = (pmatch[0].rm_eo - 1) + portion->size * portion->threadNum;
    
    //break;
    if (cutCounter >= portion->size){
      break;
    }else if (hasMatch != 0){
      break;
    }
    printf("With the whole expression, "
             "a matched substring \"%.*s\" is found at position %d to %d.\n",
             pmatch[0].rm_eo - pmatch[0].rm_so, &portionCopy[pmatch[0].rm_so],
             pmatch[0].rm_so, pmatch[0].rm_eo - 1);
             
    printf("inicio %lu \n", cutCounter + inicio);
    printf("final %lu \n", cutCounter + final);
    cutCounter += final;
    cutString(cutCounter, portion, portionCopy);
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
  	char input[100] = "utilizando";
  	copyString(100, input, portion->expression);
  	//sportion->expression = {'a' ,'b','c','|','d','e','f'};
  	
  	//creating threads
    printf("In main: creating thread %ld\n", t);
    
    if (pthread_create(&threads[t], NULL, wordSillas, (void *)portion)) {
      free(portion);
    }
  }
  
  pthread_exit(NULL);
  return 0;
}
