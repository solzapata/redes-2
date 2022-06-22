void valida_argumentos (int num);

int crea_socket (void);

void datos_cliente (char*ip, char* puerto); 

void conexion (int sockd);

void datos_local (int sd);

void vaciar (char *buff);

void escribir (int s, int n);

void leer (int s);

void imprimir (char *buff);

void logueo (int s);


void pide_input(char *operacion);

char* retorna_string(char * buffer);
void tomar_parametro(char * buff, char *param); 
void operacion_string( char *operacion, char o, char d);
void convertir_ip( char * txt, char replace, char new);
void convertir_puerto (int puerto, int *max, int * min);
void setea_estructura(char* ip,char* puerto);
int exito_conexion(int sd,socklen_t sockt);
int obtener_tamanio(char *buff);
void leer_archivo(int sdds, char *filename,long int bsize);