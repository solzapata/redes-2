#include <netinet/in.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h> 
#include <sys/types.h>
#include <string.h>
#include <ctype.h>
#include <arpa/inet.h>
#define PORT 8000
  
int main(int argc, char const* argv[]) {
    int longitud;
    char mensaje[255];   

    // SERVIDOR
    struct sockaddr_in servidor;
    servidor.sin_family = AF_INET;
    servidor.sin_port = htons(PORT);    // numero de puerto
    servidor.sin_addr.s_addr = INADDR_ANY;  // numero internet

    // SOCKET
    int socketD = socket(AF_INET, SOCK_STREAM, 0);             
  
    // CONEXION
    bind(socketD, (struct sockaddr*)&servidor, sizeof(servidor)); // bind socket a la IP y puerto
    listen(socketD, 64);    // escucha conexiones
    printf("Cargando...");
    int socketCliente = accept(socketD, NULL, NULL);
    
    // LECTURA Y RESPUESTA
    do {
    	// Leemos lo que nos envia el cliente y le respondemos que recibimos el mensaje
        if(longitud == 0){
            break;
        } else {
            longitud = read(socketCliente, mensaje, sizeof(mensaje)); // lee mensaje

            write(STDOUT_FILENO, mensaje, longitud); // responde
        }
    } while (1);
   
    // CIERRE
    close(socketD);
    close(socketCliente);
  
    return 0;
}