struct userdata { 
    char *usuario;
    char *contrasenia;
};

void valida_argumentos(int num);

int crea_socket(void);

void estructura(char* puerto);

void binding(int sd);

void escucha(int sd);

int exito_conexion(int sd, socklen_t sockt);

void vacia(char *buff);

void escribe_comando(int s);

void lee_comando(int s);

void estructura_usuario(char *usuario, char *contrasenia, struct userdata *udata);

int compara_data_ingresada(struct userdata *ud);

void loguea( struct userdata *ud);

FILE *abre_archivo(char *p);

void cierra_archivo(FILE* file);

char * obtener_contrasenia(void);

int valida_archivo(char *p);

void tomar_parametro(char * buff, char *param);

long int pide_tamanio (char *fname);

void convertir_ipuerto(char *ip, int *puerto, char* buffer);

void setea_estructura(char* ip, int puerto); 

int obtener_tamanio(char *buff);

void escribe_archivo(int sddc , FILE *puntero_archivo, long int sz);