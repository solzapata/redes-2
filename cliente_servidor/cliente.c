#include <netinet/in.h> 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h> 
#include <sys/types.h>
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
    int sock = socket(AF_INET, SOCK_STREAM, 0); // creo socket
  
    // CONEXION
    int connectStatus = connect(sock, (struct sockaddr*)&servidor, sizeof(servidor));
    if (connectStatus == -1) { // catch
        printf("Ups! Hubo un error conectando cliente y servidor\n\n");
    } else {
        // BUCLE
        printf("Desea finalizar? Ingrese la letra Z: \n");
        do {                                        // escribe y lee en serv hasta que finalice el usuario
            fgets(mensaje, sizeof(mensaje), stdin);
            
            write(sock, mensaje, strlen(mensaje));  // escribir en servidor
            
            longitud = read(sock, mensaje, sizeof(mensaje));    // leer en servidor
            write(STDOUT_FILENO, mensaje, longitud);

            if(mensaje[0] == 'Z'){
                break;
            }
        } while (1);
    }
    
    // CIERRE
    close(sock);

    return 0;
}