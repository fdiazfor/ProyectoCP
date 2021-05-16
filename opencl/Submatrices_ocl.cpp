#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include <string.h>
#include <CL/cl.h>

typedef struct {
	int x,  // Coordenada x de la submatriz
		y, // Coordenada y de la submatriz
		t; // Tamaño de la submatriz
} terna_t;

void CodigoError(cl_int err) {
	switch (err) {
		case CL_SUCCESS: printf("Ejecución correcta\n"); break;
		case CL_BUILD_PROGRAM_FAILURE: printf("CL_BUILD_PROGRAM_FAILURE\n"); break;
		case CL_COMPILER_NOT_AVAILABLE: printf("CL_COMPILER_NOT_AVAILABLE\n"); break;
		case CL_DEVICE_NOT_AVAILABLE: printf("CL_DEVICE_NOT_AVAILABLE \n"); break;
		case CL_DEVICE_NOT_FOUND: printf("CL_DEVICE_NOT_FOUND\n"); break;
		case CL_INVALID_ARG_INDEX : printf("CL_INVALID_ARG_INDEX \n"); break;
		case CL_INVALID_ARG_SIZE : printf("CL_INVALID_ARG_SIZE \n"); break;
		case CL_INVALID_ARG_VALUE: printf("CL_INVALID_ARG_VALUE\n"); break;
		case CL_INVALID_BUFFER_SIZE : printf("CL_INVALID_BUFFER_SIZE \n"); break;
		case CL_INVALID_BUILD_OPTIONS: printf("CL_INVALID_BUILD_OPTIONS\n"); break;
		case CL_INVALID_COMMAND_QUEUE : printf("CL_INVALID_COMMAND_QUEUE \n"); break;
		case CL_INVALID_CONTEXT: printf("CL_INVALID_CONTEXT\n"); break;
		case CL_INVALID_DEVICE_TYPE: printf("CL_INVALID_DEVICE_TYPE\n"); break;
		case CL_INVALID_EVENT: printf("CL_INVALID_EVENT\n"); break;
		case CL_INVALID_EVENT_WAIT_LIST : printf("CL_INVALID_EVENT_WAIT_LIST \n"); break;
		case CL_INVALID_GLOBAL_WORK_SIZE : printf("CL_INVALID_GLOBAL_WORK_SIZE \n"); break;
		case CL_INVALID_HOST_PTR : printf("CL_INVALID_HOST_PTR \n"); break;
		case CL_INVALID_KERNEL: printf("CL_INVALID_KERNEL \n"); break;
		case CL_INVALID_KERNEL_ARGS : printf("CL_INVALID_KERNEL_ARGS \n"); break;
		case CL_INVALID_KERNEL_NAME: printf("CL_INVALID_KERNEL_NAME\n"); break;
		case CL_INVALID_MEM_OBJECT : printf("CL_INVALID_MEM_OBJECT \n"); break;
		case CL_INVALID_OPERATION : printf("\n"); break;
		case CL_INVALID_PLATFORM: printf("CL_INVALID_PLATFORM\n"); break;
		case CL_INVALID_PROGRAM : printf("CL_INVALID_PROGRAM \n"); break;
		case CL_INVALID_PROGRAM_EXECUTABLE : printf("CL_INVALID_PROGRAM_EXECUTABLE\n"); break;
		case CL_INVALID_QUEUE_PROPERTIES: printf("CL_INVALID_QUEUE_PROPERTIES\n"); break;
		case CL_INVALID_VALUE: printf("CL_INVALID_VALUE\n"); break;
		case CL_INVALID_WORK_DIMENSION : printf("CL_INVALID_WORK_DIMENSION \n"); break;
		case CL_INVALID_WORK_GROUP_SIZE : printf("CL_INVALID_WORK_GROUP_SIZE \n"); break;
		case CL_INVALID_WORK_ITEM_SIZE : printf("CL_INVALID_WORK_ITEM_SIZE \n"); break;
		case CL_MEM_OBJECT_ALLOCATION_FAILURE : printf("CL_MEM_OBJECT_ALLOCATION_FAILURE \n"); break;
		case CL_OUT_OF_HOST_MEMORY: printf("CL_OUT_OF_HOST_MEMORY\n"); break;
		case CL_OUT_OF_RESOURCES: printf("CL_OUT_OF_RESOURCES\n"); break;
		case CL_PROFILING_INFO_NOT_AVAILABLE : printf("CL_PROFILING_INFO_NOT_AVAILABLE \n"); break;
		default: printf("Código de error no contemplado\n"); break;
	}
}

char leerFuentes(char *&src, const char *fileName)
{
    FILE *file = fopen(fileName, "rb");
    if (!file) {
        printf("Error al abrir el fichero '%s'\n", fileName);
        return 0;
    }

	if (fseek(file, 0, SEEK_END))  {
		printf("Error de posicionamiento en el '%s'\n", fileName);
		fclose(file);
		return 0;
    }

    long size = ftell(file);
    if (size == 0)  {
        printf("Error al calcular el tamaño del fichero '%s'\n", fileName);
        fclose(file);
        return 0;
    }

	rewind(file);
    src = (char *)malloc(sizeof(char) * size + 1);
    size_t res = fread(src, 1, sizeof(char) * size, file);
    if (res != sizeof(char) * size)   {
        printf("Error de lectura del fichero '%s'\n", fileName);
        fclose(file);
        free(src);
        return 0;
	}
    src[size] = '\0';
    fclose(file);
	return 1;
}

cl_int ObtenerPlataformas(cl_platform_id *&platforms, cl_uint &num_platforms){
	cl_int error;
	int i = 0;
	char Info[500];
	
	error = clGetPlatformIDs(0, NULL, &num_platforms);
	if (error != CL_SUCCESS) {
		CodigoError(error);
		return error;
	}
	
	platforms = new cl_platform_id[num_platforms];
	
	error = clGetPlatformIDs(num_platforms, platforms, &num_platforms);
	if (error != CL_SUCCESS) {
		CodigoError(error);
		return error;
	}
	
	for (i = 0 ; i < num_platforms ; i++){
		error = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, 500, Info, NULL);
		if (error != CL_SUCCESS) {
			CodigoError(error);
			return error;
		}
	printf("%s\n",Info);
	}
	return CL_SUCCESS;
}

cl_int ObtenerDispositivos(cl_platform_id platform, cl_device_type device_type, cl_device_id *&devices, cl_uint &num_devices){
	cl_int error;
	int i; 
	char buffer[500];
	
	error = clGetDeviceIDs(platform, device_type, 0, NULL, &num_devices);
	if (error != CL_SUCCESS) {
		CodigoError(error);
		return error;
	}
	
	devices = new cl_device_id [num_devices];
	error = clGetDeviceIDs(platform, device_type, num_devices, devices, NULL);
	if (error != CL_SUCCESS) {
		CodigoError(error);
		return error;
	}
	
	for(i = 0 ; i < num_devices ; i++){
		error = clGetDeviceInfo(devices[i], CL_DEVICE_NAME, 500, buffer, NULL);
		if (error != CL_SUCCESS) {
			CodigoError(error);
			return error;
		}	
		printf("\t%s\n", buffer);
	}
	return CL_SUCCESS;
}

cl_int CrearContexto(cl_platform_id platform, cl_device_id *devices, cl_uint num_devices, cl_context &contexto ){
	cl_int error;
	cl_context_properties prop[3] = {CL_CONTEXT_PLATFORM, (cl_context_properties) platform, 0};
	contexto = clCreateContext(prop, num_devices, devices, NULL, NULL, &error);
	if (error != CL_SUCCESS) {
		CodigoError(error);
		return error;
	}
	return CL_SUCCESS;
}

cl_int CrearPrograma(cl_program &program, cl_context context, cl_uint num_devices, const cl_device_id *device_list, const char *options, const char *fichero){
	cl_int error;
	char CompLog[20000], *fuente;
	
	leerFuentes(fuente, fichero);
	
	program = clCreateProgramWithSource (context, 1, (const char **)&fuente, NULL, &error);
	if (error != CL_SUCCESS) {
		CodigoError(error);
		return error;
	}
	
	error = clBuildProgram (program, num_devices, device_list, options, NULL, NULL);
	if (error != CL_SUCCESS) {
		if(error == CL_BUILD_PROGRAM_FAILURE){
			error = clGetProgramBuildInfo(program, device_list[0], CL_PROGRAM_BUILD_LOG, 20000, CompLog, NULL);
			if (error != CL_SUCCESS) {
				CodigoError(error);
				return error;
			}
			printf("Errores de compilación: \n%s\n", CompLog);
		} else {
			CodigoError(error);
			return error;
		}
	}
	return CL_SUCCESS;
}


void initializedouble(int t,double *a,double lv,double uv)
{
  int i;

	for(i=0;i<t;i++)
		//Valores generados entre lv y uv con 2 decimales
		a[i]=((int) ((((1.*rand())/RAND_MAX)*(uv-lv)+lv)*100.))/100.;
}

void initialize(int t,double *a,terna_t *ternas,int r)
{
	int i;

	initializedouble(t*t,a,-10.,10.);

	for(i=0;i<r;i++)
	{
		ternas[i].x=(int) (((1.*rand())/RAND_MAX)*t);
		ternas[i].y=(int) (((1.*rand())/RAND_MAX)*t);
		ternas[i].t=(int) (((1.*rand())/RAND_MAX)*(t-2)+2);
	}
}

void escribir(int t,double *a){
  int i, j;

  for (i = 0; i < t; i++)
  {
    for (j = 0; j < t; j++)
    {  
      printf("%.2f ",a[i*t+j]);
    }
    printf("\n");
  }
  printf("\n");
}

void escribirt(terna_t *a,int t)
{
	int i;

	for(i=0;i<t;i++)
		printf("%d ",a[i].x);
	printf("\n");
	for(i=0;i<t;i++)
		printf("%d ",a[i].y);
	printf("\n");
	for(i=0;i<t;i++)
		printf("%d ",a[i].t);
	printf("\n");
}

/*
c
c     mseconds - returns elapsed milliseconds since Jan 1st, 1970.
c
*/
long long mseconds(){
  struct timeval t;
  gettimeofday(&t, NULL);
  return t.tv_sec*1000 + t.tv_usec/1000;
}

int ObtenerParametros(int argc, char *argv[], int *debug, int *num_workitems, int *workitems_por_workgroups) {
	int i;
	*debug=0;
	*num_workitems=0;
	*workitems_por_workgroups=0;
	if (argc<2)
		return 0;
	for (i=2; i<argc;) {
		if (strcmp(argv[i], "-d")==0) {
			*debug=1;
			i++;
		}
		else if (strcmp(argv[i], "-wi")==0) {
			i++;
			if (i==argc)
				return 0;
			*num_workitems=atoi(argv[i]);
			i++;
			if (*num_workitems<=0)
				return 0;
		}
		else if (strcmp(argv[i], "-wi_wg")==0) {
			i++;
			if (i==argc)
				return 0;
			*workitems_por_workgroups=atoi(argv[i]);
			i++;
			if (*workitems_por_workgroups<=0)
				return 0;
		}
		else
			return 0;
	}
	return 1;
}

typedef struct {
	cl_platform_id *plataformas;
	cl_device_id *dispositivos;
	cl_context contexto;
	cl_command_queue cola;
	cl_program programa;
	cl_kernel kernel1;
	cl_kernel kernel2;
} EntornoOCL_t;

// **************************************************************************
// ***************************** IMPLEMENTACIÓN *****************************
// **************************************************************************
cl_int InicializarEntornoOCL(EntornoOCL_t *entorno) {

	cl_uint num_plataformas, num_dispositivos;
	cl_int error;

	ObtenerPlataformas(entorno->plataformas, num_plataformas);
	ObtenerDispositivos(entorno->plataformas[0], CL_DEVICE_TYPE_ALL, entorno->dispositivos, num_dispositivos);
	CrearContexto(entorno->plataformas[0], entorno->dispositivos, num_dispositivos, entorno->contexto);
	
	entorno->cola = clCreateCommandQueue(entorno->contexto, entorno->dispositivos[0], CL_QUEUE_PROFILING_ENABLE, &error);
	if (error != CL_SUCCESS) { CodigoError(error); return error;}
	
	CrearPrograma(entorno->programa, entorno->contexto, num_dispositivos, entorno->dispositivos, "", "programa.cl");
	
	entorno->kernel1 = clCreateKernel(entorno->programa, "matrices", &error);
	if (error != CL_SUCCESS) { CodigoError(error); return error;}
	
	entorno->kernel2 = clCreateKernel(entorno->programa, "suma", &error);
	if (error != CL_SUCCESS) { CodigoError(error); return error;}
	
	return CL_SUCCESS;

}

cl_int LiberarEntornoOCL(EntornoOCL_t *entorno) {

	cl_int error;

	error = clReleaseContext(entorno->contexto);
	if (error != CL_SUCCESS) { CodigoError(error); return error;}
	
	error = clReleaseCommandQueue(entorno->cola);
	if (error != CL_SUCCESS) { CodigoError(error); return error;}
	
	error = clReleaseProgram(entorno->programa);
	if (error != CL_SUCCESS) { CodigoError(error); return error;}
	
	error = clReleaseKernel(entorno->kernel1);
	if (error != CL_SUCCESS) { CodigoError(error); return error;}
	
	error = clReleaseKernel(entorno->kernel2);
	if (error != CL_SUCCESS) { CodigoError(error); return error;}
	
	error = clReleaseDevice(entorno->dispositivos[0]);
	if (error != CL_SUCCESS) { CodigoError(error); return error;}
	
	return CL_SUCCESS;
}

/*
N -> Tamaño de la matriz (NxN)
A -> Matriz
ternas -> Vector de ternas con los tamaños y las coordenadas de las submatrices
num_sb -> Número de submatrices
num_workitems -> Número de work items que se usarán para lanzar el kernel. Es opcional, se puede usar o no dentro de la función
workitems_por_workgroups -> Número de work items que se lanzarán en cada work group. Es opcional, se puede usar o no dentro de la función
*/
void ocl(int N,double *A,terna_t *ternas, int num_sb, EntornoOCL_t entorno, int num_workitems, int workitems_por_workgroups) {

	cl_int error;
	
	//cl_double *matSal = new cl_double[N*N];
	int tBuff = 0;
	int maxWI = 0;
	for(int i = 0 ;  i < num_sb ; i++){
		tBuff += ternas[i].t * ternas[i].t;
		if(maxWI  <= ternas[i].t){
			maxWI = ternas[i].t;
		}
	}
	
	size_t Wi = maxWI*num_sb , Wi_g = maxWI;
	
	cl_double *terEnt = new cl_double[tBuff];
	cl_double *terSal = new cl_double[tBuff];
		
	cl_mem bTernas, bInMatriz, bInTernas, bOutTernas;	
	
	bTernas = clCreateBuffer (entorno.contexto, CL_MEM_USE_HOST_PTR, num_sb*sizeof(terna_t), ternas, &error);
	if (error != CL_SUCCESS) { CodigoError(error); return;}
	
	bInMatriz = clCreateBuffer (entorno.contexto, CL_MEM_USE_HOST_PTR, N*N*sizeof(cl_double), A, &error);
	if (error != CL_SUCCESS) { CodigoError(error); return;}

	bInTernas = clCreateBuffer (entorno.contexto, CL_MEM_USE_HOST_PTR, tBuff*sizeof(cl_double), terEnt, &error);
	if (error != CL_SUCCESS) { CodigoError(error); return;}
	
	//bOutMatriz = clCreateBuffer (entorno.contexto, CL_MEM_USE_HOST_PTR, N*N*sizeof(cl_double), matSal, &error);
	//if (error != CL_SUCCESS) { CodigoError(error); return;}
	
	bOutTernas = clCreateBuffer (entorno.contexto, CL_MEM_USE_HOST_PTR, tBuff*sizeof(cl_double), terSal, &error);
	if (error != CL_SUCCESS) { CodigoError(error); return;}
	
	error = clSetKernelArg (entorno.kernel1, 0, sizeof(cl_mem), &bInMatriz);
	if (error != CL_SUCCESS) { CodigoError(error); return;}
	
	error = clSetKernelArg (entorno.kernel1, 1, sizeof(cl_mem), &bTernas);
	if (error != CL_SUCCESS) { CodigoError(error); return;}
	
	error = clSetKernelArg (entorno.kernel1, 2, sizeof(cl_mem), &bInTernas);
	if (error != CL_SUCCESS) { CodigoError(error); return;}
	
	error = clSetKernelArg (entorno.kernel1, 3, sizeof(cl_mem), &bOutTernas);
	if (error != CL_SUCCESS) { CodigoError(error); return;}
	
	error = clSetKernelArg (entorno.kernel1, 4, sizeof(int), &N); 
	if (error != CL_SUCCESS) { CodigoError(error); return;}
	
	error = clEnqueueNDRangeKernel(entorno.cola, entorno.kernel1, 1, NULL, &Wi, &Wi_g, 0, NULL, NULL);
	if (error != CL_SUCCESS) { CodigoError(error); return;}
	
	error = clFinish(entorno.cola);
	
	int posVec = 0;
	for(int i = 0 ; i < num_sb ; i++){
		if(i > 0)
			posVec += ternas[i - 1].t * ternas[i - 1].t;
			
		error = clSetKernelArg (entorno.kernel2, 0, sizeof(cl_mem), &bInMatriz);
		if (error != CL_SUCCESS) { CodigoError(error); return;}
		
		error = clSetKernelArg (entorno.kernel2, 1, sizeof(cl_mem), &bOutTernas);
		if (error != CL_SUCCESS) { CodigoError(error); return;}
		
		error = clSetKernelArg (entorno.kernel2, 2, sizeof(cl_mem), &bTernas);
		if (error != CL_SUCCESS) { CodigoError(error); return;}	
		
		error = clSetKernelArg (entorno.kernel2, 3, sizeof(int), &posVec); 
		if (error != CL_SUCCESS) { CodigoError(error); return;}
		
		error = clSetKernelArg (entorno.kernel2, 4, sizeof(int), &i); 
		if (error != CL_SUCCESS) { CodigoError(error); return;}
		
		error = clSetKernelArg (entorno.kernel2, 5, sizeof(int), &N); 
		if (error != CL_SUCCESS) { CodigoError(error); return;}
		
		error = clEnqueueNDRangeKernel(entorno.cola, entorno.kernel2, 1, NULL, &Wi, NULL, 0, NULL, NULL);
		if (error != CL_SUCCESS) { CodigoError(error); return;}
	}
	
	error = clFinish(entorno.cola);
	if (error != CL_SUCCESS) { CodigoError(error); return;}
}
// **************************************************************************
// *************************** FIN IMPLEMENTACIÓN ***************************
// **************************************************************************

/*
Recibirá los siguientes parámetros (los parámetros entre corchetes son opcionales): fichEntrada [-d]
fichEntrada -> Obligatorio. Fichero de entrada con los parámetros de lanzamiento de los experimentos
-d -> Opcional. Si se indica, se mostrarán por pantalla los valores iniciales, finales y tiempo de cada experimento
-wi work_items -> Opcional. Si se indica, se lanzarán tantos work items como se indique en work_items (para OpenCL)
-wi_wg workitems_por_workgroup -> Opcional. Si se indica, se lanzarán tantos work items en cada work group como se indique en WorkItems_por_WorkGroup (para OpenCL)
*/
int main(int argc,char *argv[]) {
	int i,
		debug=0,				   // Indica si se desean mostrar los tiempos y resultados parciales de los experimentos
		num_workitems=0, 		   // Número de work items que se utilizarán
		workitems_por_workgroups=0, // Número de work items por cada work group que se utilizarán
		num_problems, 		 	   // Número de experimentos
		matrix_size, 		 	   // Tamaño de la matriz
		seed, 			 	   // Semilla  
		num_random;		 	   // Número de submatrices
	double *A; 			 	   // Matriz de datos. Se representa en forma de vector. Para acceder a la fila f y la columna c: A[f*N+c]
	terna_t *ternas;			   // Vector de ternas con los tamaños y las coordenadas de las submatrices
	long long ti,			 	   // Tiempo inicial
			tf,			 	   // Tiempo final
			tt=0; 		 	   // Tiempo acumulado de los tiempos parciales de todos los experimentos realizados
	FILE *f;				 	   // Fichero con los datos de entrada
	EntornoOCL_t entorno; 	 	   //Entorno para el control de OpenCL

	if (!ObtenerParametros(argc, argv, &debug, &num_workitems, &workitems_por_workgroups)) {
		printf("Ejecución incorrecta\nEl formato correcto es %s fichEntrada [-d] [-wi work_items] [-wi_wg workitems_por_workgroup]\n", argv[0]);
		return 0;
	}
	
	InicializarEntornoOCL(&entorno);

	// Se leen el número de experimentos a realizar
	f=fopen(argv[1],"r");
	fscanf(f, "%d",&num_problems);
	
	for(i=0;i<num_problems;i++) {
		//Por cada experimento se leen
		fscanf(f, "%d",&matrix_size);	//Tamaño de la matriz (cuadrada)
		fscanf(f, "%d",&seed);		//Semilla para la inicialización de números aleatorios
		fscanf(f, "%d",&num_random);	//Número de submatrices a generar
		//Reserva de memoria para la matriz de datos y las ternas de las submatrices
		A = (double *) malloc(sizeof(double)*matrix_size*matrix_size);
		ternas = (terna_t *) malloc(sizeof(terna_t)*num_random);
		
		srand(seed);
		initialize(matrix_size,A,ternas,num_random);
		
		if (debug) {
			printf("Matriz original del experimento %d:\n", i); escribir(matrix_size, A);
			printf("Submatrices del experimento %d:\n", i); escribirt(ternas, num_random);
			
			printf("Numero de ternas %d \n", num_random);
		}

		ti=mseconds(); 
		ocl(matrix_size, A, ternas, num_random, entorno, num_workitems, workitems_por_workgroups);
		tf=mseconds(); 
		tt+=tf-ti;
		
		if (debug) {
			printf("Tiempo del experimento %d: %Ld ms\n", i, tf-ti);
			printf("Matriz resultado del experimento %d:\n", i); escribir(matrix_size, A);
		}
		free(A);
		free(ternas);
	}
  
	LiberarEntornoOCL(&entorno);
	printf("Tiempo total de %d experimentos: %Ld ms\n", num_problems, tt);
	fclose(f);
	return 0;
}
