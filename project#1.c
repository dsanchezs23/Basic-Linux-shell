#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

int FLAG = 1;

//-----------DECLARACION DE LOS METODOS-----------//

void Exit();

void executePipe(char **command);

void addCommadnInHistory(char buffer[]);

void readCommandInHistory(int pos);

int findHistoryWord(char buffer[]);

int verifyPharametersInHistoryCommand(char **command);

void checkHistoryCommand(char **command);

void methodWithOutAmpersand(char **command);

void methodWithAmpersand(char **command);

void splitParameters(char **command, char *aux, int ampersand);

int checkparameters(char *buffer, char **command);

void start(int argc, char *argv[]);

//---------------MAIN--------------//

int main(int argc, char *argv[]){

    start(argc, argv);		
	return 0;

}

//---------------MAIN--------------//


//---------------IMPLEMENTACION DE LOS METODOS--------------//
void start(int argc, char *argv[]){
    if (argc != 1){
        fprintf(stderr, "Command:\n %s \n There is an error with the parameters\n", argv[0]);
        exit(1);
    }

    char buffer[50];
    char* command[50];
    int symbol = 0;

    while (FLAG == 1 ){
        memset(buffer, 0, 50);
        printf("\nPrompt:~S ");
        if (fgets(buffer, 50, stdin) != NULL ) {
            //this is to eliminate the enter key at the final
            buffer[strcspn(buffer, "\r\n")] = 0;
        }

        if(buffer[0] != '\0'){
            addCommadnInHistory(buffer);
            symbol = checkparameters(buffer,command);

            if (strcmp(buffer, "exit") == 0){
                Exit();
                break;
            }
            else if(symbol == 3){
                checkHistoryCommand(command);
            }
            else{
                if (symbol == 2){//it's a pipe
                    executePipe(command); //buffer has the first command and parameters the second one
                }
                else if(symbol == 1){ //has an ampersand
                    methodWithAmpersand(command);
                }
                else{//doesn't have an ampersand
                    methodWithOutAmpersand(command);
                }
            }
        }
    }
}

int checkparameters(char *buffer, char **command){
    int symbol = 0;
    char *aux = strdup(buffer);
    char *parse1 = strtok(aux, "|");
    char *parse2 =  strtok(NULL, "|");

    if(findHistoryWord(buffer) == 1){
        symbol = 3;
        splitParameters(command, aux, 3);
        return symbol;
    }

    if (parse2 != NULL){ //Has a pipe
        symbol = 2;
        command[0] = parse1;
        command[1] = parse2;
    }
    else{//doesn't have a pipe
        //check if the command has an ampersand;
        if (parse1[strlen(parse1) - 1] == '&'){ //has an ampersand
            symbol = 1;
            splitParameters(command, aux, 1);
            return symbol;
        }
        else{//doesn't have an ampersand
            splitParameters(command,  aux, 0);
            return symbol;
        }
    }

    return symbol;
}

void splitParameters(char **command, char *aux, int sign){
    int i =0;
    char *token = strtok(aux, " ");
    while (token != NULL){
        command[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    
    if (sign == 3){ //historial command
        if (command[1] != NULL){ //in case it has an !#
            command[1][0] = ' ';//to eliminate the '!'
        }
        command[i] = NULL;
        return;
    }

    else if (sign == 1){//has an ampersand
        command[i - 1] = NULL; //to eliminate the ampersand
        return;
    }
    else{
        command[i] = NULL;
        return;
    }
}

void methodWithAmpersand(char **command){
    pid_t pid = fork();

	if (pid < 0){
		exit(1);
	}

	else if (pid == 0){// Child 
        execvp(command[0], command);
        printf("\nThere is an error in the command\n");
        Exit();
	}

	else{//Parent
	}
    return;
}

void methodWithOutAmpersand(char **command){

    pid_t pid = fork();
	if (pid < 0){
		exit(1);
	}

	else if (pid == 0){// Child 
        execvp(command[0], command);
        printf("\nThere is an error in the command\n");
        Exit();
	}

	else{//Parent
    	wait(NULL);
	}

    return;
}

void checkHistoryCommand(char **command){
    int pos =0;
    char historyWord[] = "historial";

    if (command[1] == NULL){
        if (strcmp(command[0], historyWord) == 0 && command[1] == NULL){            
            readCommandInHistory(pos);
        }
        else{
            printf("\n\nError: check the command\n\n");
        }
    }
    else{
        pos = verifyPharametersInHistoryCommand(command);
        if (pos != 0){
            readCommandInHistory(pos);
        }
        else{
            printf("\n\nError: check the command\n\n");
        }
    }
}

int verifyPharametersInHistoryCommand(char **command){
    int pos = 0;
    char num [15] = "";
    int i = 1;

    while (command[1][i] != '\0'){
        if (command[1][i] != '!'){
            num[i-1] = command[1][i];
        }
        i++;
    }
    
    pos = atoi(num);
    if ((pos != 0)){
        memset(num, 0, sizeof(num));
        return pos;
    }
    memset(num, 0, sizeof(num));
    return pos;
}

int findHistoryWord(char buffer[50]){
    char historyWord[] = "historial";

    for (int i = 0; i < sizeof(historyWord) - 1; i++){
        if (buffer[i] != historyWord[i]){//didn't find it
            return 0;
        }

    }
    return 1;
}

void readCommandInHistory(int pos){
    char bufferHistory[100];
    char aux[255][255] = {};
    size_t i = 0;
    FILE *fp;
	fp = fopen ( "History.txt", "r" );        
	if (fp==NULL) {
        fputs ("There's an error opening the file",stderr); 
        exit (1);
    }

    while (feof(fp) == 0){
        if (fgets(bufferHistory,100,fp) != NULL){
            for (size_t j = 0; j < strlen(bufferHistory) - 1; j++){
                aux[i][j] = bufferHistory[j];            
            }
            i++;
            memset(bufferHistory, 0, 100); 
        }
    }
    i--;

    size_t k = 0;
    if (pos != 0){
        for(k; k < i; k++){
            if (k + 1 == pos){
                printf("%d- %s\n", k + 1, aux[k]);
            }
        }
    }
    else{
        for(k; k < i; k++){
            printf("%d- %s\n", k + 1, aux[k]);
        }
    }

	fclose (fp);
}

void addCommadnInHistory(char buffer[]){
    char *copy;
    size_t i = 0;
    copy = strdup (buffer);
    size_t completeBuffer = strlen(buffer);

    copy[completeBuffer] = '\n';
    copy[++completeBuffer] = '\0';

    FILE *fp;
	fp = fopen ( "History.txt", "a");        
	if (fp==NULL) {
        fputs ("There's an error opening the file",stderr);
        exit (1);
    }
    else{
        fputs(copy, fp);
    }
    fclose (fp);
    //memset(copy, 0, i); 
    free(copy);
}

void executePipe(char **command){
    char *aux[50];
    char *aux2[50]; 

    checkparameters(command[0],aux);
    checkparameters(command[1],aux2);

    pid_t pid;
    int fd[2];

    pipe(fd);
    pid = fork(); 

    if(pid == 0) {
        dup2(fd[1], STDOUT_FILENO);
        execvp(aux[0], aux);
        Exit();
    } else {
        waitpid(pid, 0, 0);
    }

    close(fd[1]);

    pid = fork();

    if(pid == 0) {
        dup2(fd[0], STDIN_FILENO);
        execvp(aux2[0], aux2);
        Exit();
    } else {
        waitpid(pid, 0, 0);
    }
    
    /*close(fd[0]);
    close(fd[1]);*/
}

void Exit(){
    FLAG = 0;
}

//---------------IMPLEMENTACION DE LOS METODOS--------------//

//---------------BIBLIOGRAFIA--------------//

/*Bibliography
https://parzibyte.me/blog/2018/11/18/fgets-vs-scanf-en-c/
https://pablohaya.com/2013/10/12/diferencia-entre-scanf-gets-y-fgets/
https://www.programacion.com.py/escritorio/c/creacion-y-duplicacion-de-procesos-en-c-linux
https://www.iteramos.com/pregunta/57789/como-eliminar-espacios-vacios-en-un-char-array-en-c
https://linuxhint.com/exec_linux_system_call_c/
https://man7.org/linux/man-pages/man3/exec.3.html
https://www.geeksforgeeks.org/exec-family-of-functions-in-c/
https://medium.com/10-goto-10/manejo-de-archivos-en-c-19f85b31ddd
http://icaro.eii.us.es/descargas/ficheros.pdf
https://www.geeksforgeeks.org/builtin-command-in-linux-with-examples/

https://www.youtube.com/watch?v=l-UhKLdh4aY
*/




