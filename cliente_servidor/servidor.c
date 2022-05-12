#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <stdbool.h>
#include <stdarg.h>
#include <err.h>
#include <errno.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define MENSAJE "8000 version 1.0\n\n"

char buffer[128];
struct sockaddr_in servidor;

// valida si los argumentos son validos
void valida_argumentos(int num){
    if (num != 2){
		printf("ups! la cantidad de argumentos es invalida\n");
		exit(-1);
	}
}

void asigna_servidor(char* puerto) {
    servidor.sin_family = AF_INET;  
	servidor.sin_port = htons(puerto);
	servidor.sin_addr.s_addr = INADDR_ANY;
}

void abre_socket(int sd) {
    if(sd == -1) {
		printf("ups! no se pudo abrir el socket\n");
		exit(-1);
	}
}

void binding(int sd) {
	int bindeo = bind(sd,(struct sockaddr*)&servidor, sizeof(servidor)); 
	
	if(bindeo == -1){
		printf("ups! no se pudo asociar el socket\n");
		exit(-1);
	}
}

void escucha(int sd) {
	int escucha = listen(sd, 1);

	if(escucha == -1) {
		printf("ups! no se pudo escuchar\n");
		exit(-1);
	}
}

void valida_peticiones(int csd) {
	if(csd == -1) {
		printf("ups! error en las peticiones del cliente\n");
		exit(-1);
	}
}

void escribe(int csd) {
	int escrito = write(csd, MENSAJE, sizeof(MENSAJE));

    if(escrito == -1) {
        printf("ups! no se pudo escribir en fichero\n");
        exit(-1);
    }
}

int main(int argc, char *argv[]){
    int puerto = atoi(argv[1]);                     // puerto
	int size_servidor;								// tamaño del servidor

	// cantidad de argumentos son validos?
    valida_argumentos(argc);

    // asigna la ip y puerto
    asigna_servidor(puerto);

	// creo socket
    int sd = socket(AF_INET, SOCK_STREAM, 0);       // socket
    abre_socket(sd); 

	// binding
	binding(sd);

	// empiezo a escuchar al cliente
	escucha(sd);

	// acepto peticiones
	size_servidor = sizeof(servidor);
	int csd = accept(sd, (struct sockaddr*)&servidor, (socklen_t*)&size_servidor);
	valida_peticiones(csd);

	// escribimos en fichero del cliente
	escribe(csd);

	// se cierra cliente y servidor
    close(csd);
    close(sd);

	return 0;
}