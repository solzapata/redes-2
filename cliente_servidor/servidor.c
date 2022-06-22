#include <stdio.h>
#include <stdlib.h>
#include <ctype.h> 
#include <arpa/inet.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h> 

#include "servidor.h"

#define VERSION "1.0"
#define SERVIDOR "srvFtp" 
#define CLIENTE "cltFtp" 
#define LOCALHOST "127.0.0.1"
#define COLA   5      
#define ADIOS "Goodbye"

char * cquit="QUIT";
char * cusuario="USUARIO";
char * cpuerto="PUERTO";
char * ccontrasenia="CONTRASENIA";
char * cretr="RETR";
char * puertook="Port received";
char * codigo550="550";
char * codigo530="530";
char * codigo230="230";
char * codigo229="229";
char * codigo226="226";
char * codigo221="221";
char * codigo200="200";
char *logueado="logueado";
char * no_logueado= "logueo incorrecto";
char * xversion="220 cltFtp 1.0";
char * codigo331="331";
char * rq= "contrasenia requerida";
char * no_encontrado=": no existe";
char * transfer= "Transferencia completa";
char * bytes= "bytes";
char * nfile= "archivo";
char * size ="tamanio";

char buffer_servidor[256];
char buffer_cliente[256];
char *operacions [20];
char contra[50];
char espacio_usuario [50];
char ip_client[16];
char filename [50];
int  puerto_client=0;
struct sockaddr_in serverAddr;
struct sockaddr_in clienteDir;
struct sockaddr_in dataDir;
bool cconnect= true;
FILE *puntero_archivo;

void valida_argumentos(int num){
    if(num != 2) {
        printf("ups! argumentos invalidos\n");
        exit(-1);
    }
}

int crea_socket(void) {
	int sd = socket(AF_INET,SOCK_STREAM, 0);

	if(sd < 0) {
		printf("ups! no se creo el socket\n");
		exit(-1);
	} else {
        #ifdef DEBUG
		printf("socket creado!!\n");
        #endif
	}

	return sd;
}

void estructura(char* puerto) {
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(atoi(puerto));
    serverAddr.sin_addr.s_addr = inet_addr(LOCALHOST);
}

void binding(int sd) {
    int estado = bind(sd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
   
    if(estado < 0) {
        printf("ups! no se pudo hacer el bind\n");
        exit(-1);
    }
    #ifdef DEBUG
    printf("bind exitoso!!\n");
    #endif
}

void escucha(int sd){
    int estado = listen (sd,COLA);
   
    if(estado < 0) {
		printf("ups! error en listen\n");
		exit(-1);
	} else { 
        #ifdef DEBUG
        printf("listen exitoso!!\n");
        #endif
    }
}

int exito_conexion(int sd,socklen_t sockt){
    printf("[+]Esperando conexión...\n");

    int csd = accept(sd, (struct sockaddr *)&clienteDir, &sockt);
    
    if(csd < 0){
        printf("ups! error en el accept\n");
        exit(-1);
    } else {
        #ifdef DEBUG
        printf("accept con exito!!\n");
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET,&(serverAddr.sin_addr),ip,INET_ADDRSTRLEN);
        printf("conexion con ip %s y puerto %d!!\n", ip, ntohs(serverAddr.sin_port));
        #endif
    }

    return csd;
}

void vacia(char * buff){
	memset(buff, 0, sizeof(*buff));
}

void lee_comando(int s){
	int lee = read(s, buffer_cliente, sizeof(buffer_cliente));

	if(lee == -1) {
		printf("ups! no pudo recibir el mensaje\n");
		exit(-1);
	} else{
        #ifdef DEBUG
		printf("lectura con exito!!\n");
        printf("recibio < %s\n", buffer_cliente);
        #endif 
	}
}

void escribe_comando(int s){
	int estado = write(s, buffer_servidor, sizeof(buffer_servidor));

	if(estado == -1){
		printf("ups! no pudo escribir\n");
		exit(-1);
	} else{
        #ifdef DEBUG
		printf("escritura con exito!!\n");
        #endif
	}
}

int compara_data_ingresada(struct userdata *ud){
    char auxiliar_buff[256];

    sprintf(auxiliar_buff, "%s", buffer_cliente);
    char * espacio=strtok(auxiliar_buff, " ");

    int compara = strncmp(espacio, cquit,strlen(cquit)); 
		
    if (compara == 0) {
        return 0;
    } else if(strncmp(espacio, xversion,3) == 0) {
        return 2;
    } else if(strncmp(espacio, cusuario, strlen(cusuario)) == 0) {
        return 3;
        #ifdef DEBUG
        printf("recibi comando %s\n",cusuario);
        #endif
    } else if(strncmp(espacio, ccontrasenia, strlen(ccontrasenia)) ==0 ){
        return 4;
    } else if(strncmp(espacio,cretr,strlen(cretr))==0){       
        return 5;
    } else if(strncmp(espacio,cpuerto,strlen(cpuerto))==0){ 
        return 6;
    } else{
        printf("este comando %s es erroneo\n", buffer_cliente);
        return 1;
    }
}

FILE * abre_archivo( char* path){
    puntero_archivo=fopen(path, "r");
    return puntero_archivo;
}

void cierra_archivo(FILE* file){
    fclose(file);
}

void loguea(struct userdata * ud){
    char linea[200];
    char * espacio_usuario;
    char * espacio_contrasenia;
    char * aux_puntero;
    int x = 100, flag_cierra = 0, var = 0;

    while(!feof(puntero_archivo)) {
        fgets(linea, 200, puntero_archivo);
        espacio_usuario = strtok(linea,":");
        espacio_contrasenia = strtok(NULL,"");

        if(var == 0) {
            aux_puntero = strtok(buffer_cliente," ");
            aux_puntero = strtok(NULL, " ");
            var++;
        }

        strncpy(espacio_usuario, aux_puntero, strlen(espacio_usuario));
        x=strncmp(espacio_usuario, espacio_usuario, strlen(espacio_usuario)-2);

        if(x == 0) {
            estructura_usuario(espacio_usuario, contra, ud);
            sprintf(contra, "%s", espacio_contrasenia);
            cierra_archivo(puntero_archivo);
            flag_cierra++;

            break;
        } else {
            estructura_usuario(NULL, NULL, ud); 
        }
    }
   
    if (flag_cierra == 0) {
        cierra_archivo(puntero_archivo);
    }
}

void estructura_usuario(char *usr, char *pass, struct userdata *udata){
    (*udata).usuario = usr;
    (*udata).contrasenia=pass;
}

char * obtener_contrasenia(void){
    char auxiliar_buff[256];

    sprintf(auxiliar_buff,"%s",buffer_cliente);
    strtok(auxiliar_buff, " ");
    char * f =strtok(NULL, " ");
    char * espacio = malloc(strlen(f));
    strncpy(espacio,f,strlen(f)-1);

    return espacio;
}

void tomar_parametro(char * buff, char *param){ 
	char *auxcommand= buff;
	char *aux2;
    memset(param,0,256);
	strtok(auxcommand, " ");
	aux2=strtok (NULL,"\0");
	strncpy(param,aux2,strlen(aux2)-2);
}

int valida_archivo(char *p){
    return (access(p, F_OK));
}

long int pide_tamanio(char *fname){
    struct stat fstatus;

    if(stat(fname, &fstatus)<0){
        return -1;
    }

    return fstatus.st_size;
}

void convertir_ipuerto(char *ip, int *puerto, char* buffer){
    buffer=buffer+5;
    char puertomax[5];
    char puertomin [5];
    int max;
    int min;
    int i=0;

    memset(puertomax,0,sizeof(puertomax));
    memset(puertomin,0,sizeof(puertomin));

    int comnumb=0;

    while(comnumb < 4) {
        if(*buffer != ',') {
            *ip=*buffer;
        } else {
            comnumb++;
            *ip='.';
        }
       
        ip++;
        buffer++;
    }

    *(ip -1)='\0';
    comnumb=0;

    while(*buffer != '\r') {
        if(*buffer != ',') {
            if (comnumb==0) {
                puertomax[i]=*buffer;
                i++;
                buffer++;
            } else { 
                puertomin[i]=*buffer;
                i++;
                buffer++;
            }
        } else {
            buffer++;
            comnumb++;
            i=0;
        }

    }

    max=atoi(puertomax);
    min=atoi(puertomin);
    max= max<<8;
    (*puerto)=max + min;
}

void setea_estructura(char* ip, int puerto){
    dataDir.sin_family=AF_INET;
    dataDir.sin_port= htons(puerto);
    dataDir.sin_addr.s_addr = inet_addr(ip);
}

void escribe_archivo(int sddc , FILE *puntero_archivo, long int sz){
    char data[512]={0};

    while(sz >= 0) {
        if(sz <= 512){
            while(fread(data,1,sz,puntero_archivo)!=NULL) {
                if (write(sddc,data,sz)<0){
                    printf("ups! no pudo escribir en archivo\n");
                    exit(-1);
                }
                memset(data,0,sizeof(data));
            }
            sz=-1;
        } else {
            while(fread(data,1,512,puntero_archivo)!=NULL){
                if (write(sddc,data,sizeof(data))<0){
                    printf("ups! no pudo escribir en archivo\n");
                    exit(-1);
                }
                memset(data,0,sizeof(data));
            }
            sz=sz -512;
            
            if(sz<0){
                sz=512+sz;
            }
        }
    }
}

int main(int argc, char* argv[]){
    valida_argumentos(argc);

    int sd;
    int sddc;
    char p[256];
    char * puerto=argv[1];
    int csd;
	operacions[0]="220";

    socklen_t size_addr;
    size_addr=sizeof(serverAddr);

    sd=crea_socket();

    estructura(puerto);

    binding(sd); 

    escucha(sd);

    struct userdata ud;
    csd=exito_conexion(sd, size_addr);

    vacia(buffer_cliente);

    lee_comando(csd);

    vacia(buffer_servidor);

    sprintf(buffer_servidor, "%s %s %s\r\n", operacions[0], SERVIDOR, VERSION);

    escribe_comando(csd);

    #ifdef DEBUG
    printf("escribiendo %s!!",buffer_servidor);
    #endif
    estructura_usuario("","",&ud);
   
    while(1){
        vacia(buffer_cliente);

        lee_comando(csd);

        int nvar=compara_data_ingresada(&ud);

        vacia(buffer_servidor);

        if(nvar == 0) { 
            vacia(buffer_servidor);

            printf("desconectando...\n");
            sprintf(buffer_servidor, "%s %s\r\n",codigo221,ADIOS);
            escribe_comando(csd);
            sleep(1);
            shutdown(csd,SHUT_RDWR);
            close(csd);
            csd=exito_conexion(sd, size_addr);
        } else if (nvar==1){
            char *error="Código de Error";
            vacia(buffer_servidor);
            sprintf(buffer_servidor, "%s\r\n",error);
            escribe_comando(csd);
            
        } else if (nvar==2){
            vacia(buffer_servidor);
            sprintf(buffer_servidor, "%s %s %s\r\n", operacions[0], SERVIDOR, VERSION);
            escribe_comando(csd);
        } else if (nvar==3){
            vacia(buffer_servidor);
            abre_archivo("ftpusers");
            loguea(&ud);
            if(ud.usuario!=NULL) {
                vacia(buffer_servidor);
                sprintf(buffer_servidor, "%s %s %s\r\n",codigo331,rq,ud.usuario);
                escribe_comando(csd);
                
            } else{
                vacia(buffer_servidor);
                sprintf(buffer_servidor, "%s %s\r\n",codigo530,no_logueado);
                escribe_comando(csd);
                printf("desconectando...\n");
                close(csd);
                break;
            }
        } else if (nvar==4) {   
            if(ud.contrasenia!=NULL){
                char * aux=obtener_contrasenia();
                if(strncmp(aux,ud.contrasenia,strlen(ud.contrasenia)-1)==0){
                    vacia(buffer_servidor);
                    sprintf(buffer_servidor,"%s %s %s %s\r\n",codigo230,"Usuario",ud.usuario,logueado);
                    escribe_comando(csd);
                }
                free(aux);// Libero el espacio del heap
            } else {
                vacia(buffer_servidor);
                sprintf(buffer_servidor, "%s %s\r\n",codigo530,no_logueado);
                escribe_comando(csd);
                printf("desconectando...\n");
                close(csd);
                
                break; // Finalizo mi bucle
            }
        } else if (nvar==5){
            
            vacia(p);
            tomar_parametro(buffer_cliente,p);
            valida_archivo(p);
            
            if (valida_archivo(p)==0) {
                vacia(buffer_servidor);
                long int sizef=pide_tamanio(p);
                char *bf [10];
                
                memset(filename,0,strlen(filename));
                sprintf(filename,"%s",p);
                sprintf(bf,"%ld",sizef);
                sprintf(buffer_servidor,"%s %s %s %s %s %s\r\n",codigo229,nfile,p,size,bf,bytes);
                escribe_comando(csd);

            } else{
                vacia(buffer_servidor);
                sprintf(buffer_servidor, "%s %s %s\r\n",codigo550,p, no_encontrado);
                escribe_comando(csd);
            }
        } else if (nvar==6){
            vacia(buffer_servidor);
            sprintf(buffer_servidor, "%s %s\r\n",codigo200,puertook);
            escribe_comando(csd);
            convertir_ipuerto(ip_client,&puerto_client,buffer_cliente);
            setea_estructura(ip_client,puerto_client);
            sddc=crea_socket();
            if(connect(sddc,(struct sockaddr *)&dataDir,sizeof(dataDir))<0){
                printf("ups! conexion fracasada\n");
                exit(-1);
            }
            #ifdef DEBUG
            printf("conectado para transferencia!!\n");
            #endif
            puntero_archivo=fopen(p,"rb");
            long int sizef=pide_tamanio(p);
            escribe_archivo(sddc,puntero_archivo,sizef);
            memset(buffer_servidor,0, sizeof(buffer_servidor));
            sprintf(buffer_servidor, "%s %s\r\n",codigo226,transfer);
            escribe_comando(csd);
        }
        
    }

    close(sd);

    return 0;
}
