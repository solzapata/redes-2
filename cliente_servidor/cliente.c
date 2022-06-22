#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "cliente.h"

#define VERSION "1.0"
#define CLIENTE "cltFtp" 
#define CODIGO "220" 
#define USUARIO "USUARIO" 
#define CONTRASENIA "CONTRASENIA"
#define RETR "RETR" 

#define CODIGO200 "200"
#define CODIGO226 "226"
#define CODIGO229 "229"
#define CODIGO331 "331"
#define CODIGO530 "530"
#define CODIGO550 "550"

#define GET  "get"
#define PUERTO "PUERTO"

struct sockaddr_in clienteDir;// Estructura de mi socket cliente
struct sockaddr_in localDir;// Estructura de mi cliente local.
struct sockaddr_in dataDir;// Estructura de datos
char servidor_data[256]; // Tamaño de mi server buffer
char operacion [256];
char buffer_cliente [256];
int estado = 0;// Flag de estado de mi login. 0=login correcto; 1=login incorrecto. 
char aux[256];

void valida_argumentos(int num) {
    if (num < 3 || num > 3) {
		printf("ups! la cantidad de argumentos es invalida\n");
		exit(-1);
	}
}

int crea_socket(void) {
	int sd = socket(AF_INET, SOCK_STREAM, 0); 

	if (sd == -1) {
		printf("ups! error en la conexion\n");
		exit(-1);
	} else {
		#ifdef DEBUG
		printf("conectado!!\n");
		#endif
	}

	return sd;
}

void datos_cliente(char* ip, char* puerto) {
	clienteDir.sin_family = AF_INET; //Familia del cliente
	clienteDir.sin_port = htons(atoi(puerto));//Puerto para la conexión
	clienteDir.sin_addr.s_addr = inet_addr(ip);//Dirección del cliente
}

void conexion(int sockd) {
	int connect_stat;
	connect_stat = connect(sockd,(struct sockaddr *)&clienteDir,sizeof(clienteDir));
	
    if (connect_stat < 0) {
		printf("ups! no se pudo establecer la conexion\n");
		exit(-1);
	} else {
		printf("conexion establecida!\n");		
	}
}

void datos_local(int sd) {
    localDir.sin_family = AF_INET;
    localDir.sin_port = htons(0);
    localDir.sin_addr.s_addr = INADDR_ANY;

	socklen_t localsz=sizeof(localDir);

	getsockname(sd,(struct sockaddr*)&localDir,&localsz);
}

void vaciar(char * buff) {
    memset(buff, 0, sizeof(*buff));
}

void escribir(int s, int n) {
	if (n == 0) {
		int write_stat = write(s, buffer_cliente, sizeof(buffer_cliente));

		if(write_stat == -1) {
			printf("ups! no se pudo escribir en el socket\n");
			exit(-1);
		} else{
			#ifdef DEBUG
			printf("se escribio con exito!\n");
			printf("mensaje: %s", buffer_cliente);
			#endif
		}
	} else if(n == 1) {
		int write_stat = write(s,operacion,sizeof(operacion));

		if(write_stat == -1) {
			printf("ups! no se pudo escribir en el socket\n");
			exit(-1);
		} else { 
			#ifdef DEBUG
			printf("escritura con exito!!\n");
			#endif
		}
	}
}

void leer(int s) {
	int reads = read(s, servidor_data, sizeof(servidor_data));

	if(reads == -1) {
		printf("[-]No se pudo recibir el mensaje\n");
		exit(-1);
	} else{
		#ifdef DEBUG
		printf("lectura con exito!!\n");
		#endif
	}
}

void imprimir(char *buff){
	printf("%s\n",buff);

}

void logueo (int s) {
	char usuario [60];
	char contrasenia [60];
	char *auxstring;

	printf("usuario: ");

	fgets(usuario, sizeof(usuario), stdin);

	operacion_string(operacion,'\n','\0');
	sprintf(buffer_cliente,"%s %s\r\n", USUARIO, usuario);

	escribir(s, 0);
	leer(s);

	auxstring = retorna_string(servidor_data);

	if (strncmp(auxstring, CODIGO331, strlen(CODIGO331)) == 0) {
		printf("contrasenia: ");
		
        fgets(contrasenia,sizeof(contrasenia),stdin);
		operacion_string(operacion,'\n','\0');
		
        vaciar(buffer_cliente);
		
        sprintf(buffer_cliente,"%s %s\r\n", CONTRASENIA, contrasenia);
		
        escribir(s, 0);
		leer(s);
		
        printf("%s\n", servidor_data);
	} else if (strncmp(auxstring, CODIGO530, strlen(CODIGO530)) == 0) {
        imprimir(servidor_data);
        estado = 1;
     
        close(s);
    }
}

void setea_estructura(char* ip,char* puerto) {
	dataDir.sin_family = AF_INET; 
	dataDir.sin_port = htons(atoi(puerto));
	dataDir.sin_addr.s_addr = inet_addr(ip);
}

void pide_input(char *operacion){
	printf("< ");
	fgets(operacion,256,stdin);
	operacion_string(operacion,'\n','\0');
}

char* retorna_string(char * buffer){ //Retorno mi código en string
    sprintf(aux, "%s",buffer);
	return strtok(aux, " ");
}

void tomar_parametro(char * buff, char *param){ 
	char *auxcommand = buff;
	char *aux2;

	strtok(auxcommand, " ");

	aux2=strtok (NULL,"\0");

	strcpy(param,aux2);
}

void operacion_string(char *operacion, char o, char d){
    if ((strlen(operacion) > 0) && (operacion[strlen (operacion) - 1] == o)){
        operacion[strlen (operacion) - 1] = d;
	}
}

void convertir_ip( char * txt, char replace, char new){
	int stlen= strlen(txt);
	for (int i=0; i<stlen;i++){
		if (txt[i]==replace){
			txt[i]=new;
		}
	}

}

void convertir_puerto (int puerto, int *max, int * min){
	*max=(puerto >>8) & 0b0000000011111111;
	*min=(puerto)& 0b0000000011111111;

}

int exito_conexion(int sd,socklen_t sockt){
    int csd=accept(sd,(struct sockaddr *)&dataDir, &sockt);
    if(csd < 0){
        printf("ups! no se pudo aceptar\n");
        exit(-1);
    }
    else{
		#ifdef DEBUG
        printf("accept exitoso!!\n");
		#endif
	}
	return csd;
}

void leer_archivo(int sdds, char *filename,long int bsize){// Copio el archivo
	int nbr;
	FILE * fp;
	fp=fopen(filename,"wb");
	if (fp==NULL){
		printf("ups! no se pudo escribir\n");
		exit(-1);
	}
	long int aux22=0;
	long int auxsz=bsize;
	while(1){
		if(auxsz>=512){
			nbr=read(sdds,servidor_data,512);
		}
		else{
			if(auxsz != 0){
			nbr=read(sdds,servidor_data,auxsz+1);
			}
		}
		
		if(nbr==-1){
			break;
		}
		fprintf(fp, "%s",servidor_data);
		memset(servidor_data,0,sizeof(servidor_data));
		aux22=aux22+nbr;
		if(auxsz<=0) break;
		auxsz=auxsz-nbr;
		
	}
	#ifdef DEBUG
	printf("se escribio con exito!!\n");
	#endif
	fclose(fp); // Cierro el archivo
}

int obtener_tamanio(char *buff){
	char buffaux[256];
	sprintf(buffaux, "%s", buff);
	strtok(buffaux, " ");
	strtok(NULL, " ");
	strtok(NULL, " ");
	strtok(NULL, " ");
	char * aux=strtok(NULL, " ");
	int nsize=atoi(aux);

	return nsize;
}


int main(int argc,char * argv[]){
	int  sd, sdd; //Creación del socket descriptor
	char *ip=argv[1];
	char *puerto= argv[2];
	char *quitr="221 Goodbye";
	char filename [30];
	int dataFlag=0; 
	char puerto_l [10]; // Variable para mi puerto (string)
	char ipread[16]; // Guardo la ip de mi estructura
	int max=0, min=0;
	long int bytesz=0;
	

	valida_argumentos(argc);

	sd = crea_socket();

	datos_cliente(ip, puerto);

	conexion(sd);

	datos_local(sd);

	int puerto_local = ntohs(localDir.sin_port);

    sprintf(buffer_cliente,"Codigo %s. Cliente %s. Version %s.\n", CODIGO, CLIENTE, VERSION);
	
    vaciar(buffer_cliente);
	
    sprintf(buffer_cliente,"Codigo %s. Cliente %s. Version %s.\n", CODIGO, CLIENTE, VERSION);

	escribir(sd, 0);

	leer(sd);
	
    imprimir(servidor_data);

	vaciar(buffer_cliente);

	logueo (sd);

	
	while(estado==0){
		pide_input(operacion);

		if (strncmp(operacion, "QUIT", strlen(operacion)) == 0) {
			escribir(sd, 1);
			leer(sd);
			
            if (strncmp(servidor_data, quitr, strlen(quitr)) == 0) {
				imprimir(servidor_data);
				printf("cerrando..\n");
				
                close(sd);
                break;
			}
		} else if ((strncmp(operacion, GET, strlen(GET)) == 0)) {
			tomar_parametro(operacion, filename);
			vaciar(buffer_cliente);
			sprintf(buffer_cliente,"%s %s\r\n", RETR, filename);
			escribir(sd, 0);
			leer(sd);
			
			if (strncmp(servidor_data, CODIGO550, strlen(CODIGO550)) == 0) {
				imprimir(servidor_data);
			} else if (strncmp(servidor_data, CODIGO229, strlen(CODIGO229)) == 0) {
				dataFlag = 1; // Flag para iniciar canal de datos
				imprimir(servidor_data);
				bytesz = obtener_tamanio(servidor_data);
			}
		} else {
			printf("ups! comando equivocado\n");
		}
		
		if (dataFlag == 1) {
			sdd = crea_socket();// Creo el socket para datos
			puerto_local = puerto_local +1;
			sprintf(puerto_l, "%d", puerto_local);
			setea_estructura(ip, puerto_l);
			socklen_t data_size = sizeof(dataDir);

			if(bind (sdd,(struct sockaddr *)&dataDir, data_size) < 0) {
				printf("ups! fallo el bind\n");
				setea_estructura(ip,"0");
				
                if(bind (sdd, (struct sockaddr *)&dataDir, data_size) < 0) {
					printf("ups! fallo el bind\n");
					exit(-1);// Falla del sistema
				} else {
					getsockname(sdd, (struct sockaddr *)&dataDir, &data_size);
					inet_ntop(AF_INET, &(dataDir.sin_addr), ipread, INET_ADDRSTRLEN);
					#ifdef DEBUG
					printf("bind exitoso!!\n");
					printf("la ip es %s y el puerto es %d\n", ipread, ntohs(dataDir.sin_port));
					#endif
				}
			} else {
				getsockname(sdd, (struct sockaddr *)&dataDir, &data_size);
				inet_ntop(AF_INET, &(dataDir.sin_addr), ipread, INET_ADDRSTRLEN);
				
				#ifdef DEBUG
				printf("bind exitoso!!\n");
				printf("la ip es %s y el puerto %d\n",ipread,ntohs(dataDir.sin_port));
				#endif
			}
			
            if (listen(sdd,1) < 0) {
				printf("ups! fallo el listen\n");
				exit(-1);
			}
			
            puerto_local = ntohs(dataDir.sin_port);
			convertir_ip(ipread, '.', ','); 
            
			#if DEBUG
			printf("la ip formateada es: %s\n", ipread);
			#endif
			convertir_puerto(puerto_local,&max,&min);
			sprintf(buffer_cliente, "%s %s,%d,%d\r\n", PUERTO, ipread, max, min);

			escribir(sd, 0);
			leer(sd);

			if (strncmp(servidor_data,CODIGO200, 3) == 0) {
				int sdds = exito_conexion(sdd, data_size); 
				
                leer_archivo(sdds, filename, bytesz);
				leer(sd); 

				if (strncmp(servidor_data, CODIGO226, strlen(CODIGO226)) == 0) {
					imprimir(servidor_data);
				} else {
					printf("ups! transferencia fallida\n");
				}
			}
		}
	}
	
	return 0;
}