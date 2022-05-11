#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

char buffer[128];
struct sockaddr_in servidor;

// valida si los argumentos son validos
void valida_argumentos(int num){
    if (num != 3){
		printf("ups! la cantidad de argumentos es invalida\n");
		exit(-1);
	}
}

// guarda los datos del servidor
void guardo_servidor(char* ip, char* puer) {
    servidor.sin_family = AF_INET;
   	servidor.sin_port = htons(puer);
    servidor.sin_addr.s_addr = inet_addr(ip);
}

// efectiviza la conexion
void conexion(int sd) {
    if(sd == -1) {                          
		printf("ups! error en la conexion\n");
		exit(-1);
	} else {
        int conecta = connect(sd,(struct sockaddr*)&servidor,sizeof(servidor));

        if(conecta == -1){
            printf("ups! error de conexion\n");
            exit(-1);
        } else {
            printf("conectado!!\n");		
        }
    }
}

// lee el mensaje del servidor
void lee_escribe(int sd) {
    int lectura = read(sd, buffer, sizeof(buffer));

    if(lectura == -1) {
        printf("ups! no se pudo leer\n");
        exit(-1);
    }
}

// imprime datos del buffer
void imprime() {
    printf("dato del servidor: %s\n\n", buffer);
}

int main(int argc, char *argv[]){
    char ip[32];                                // ip
    int puerto = atoi(argv[2]);                 // puerto

	// cantidad de argumentos son validos?
    valida_argumentos(argc);

    // guardo la ip y puerto
    guardo_servidor(argv[1], puerto);

	// hago la conexion
    int sd = socket(AF_INET, SOCK_STREAM, 0);   // socket
    conexion(sd);

    // lectura y copia del dato en buffer
    lee_escribe(sd);

    // imprimo el dato en buffer
    imprime();

    // se desconecta
	close(sd);          

	return 0;
}