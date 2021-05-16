typedef struct {
	int x,  // Coordenada x de la submatriz
		y, // Coordenada y de la submatriz
		t; // Tamaño de la submatriz
} terna_t;

__kernel void matrices(__global double *inM, __global terna_t *ternas, __global double *Bin, __global double *Bout, int N){
	size_t idG = get_group_id(0);
	size_t id = get_local_id(0);
	size_t NumWorkItems = get_local_size(0);
	int siz = 0, inX = 0, inY = 0;
	int posVec = 0;
	
	for(int i = 0; i < idG ; i++){
		posVec += ternas[i].t * ternas[i].t;
	}

 	siz = ternas[idG].t;
	inY = ternas[idG].y; 
	inX = ternas[idG].x;
	
	//Cargar terna
	int iter = siz/NumWorkItems;
	for(int y = id*iter + inY ; y < (id+1)*iter + inY ; y++){
		for(int x = inX ; x < (siz + inX) ; x++){
			Bin[(y - inY)*siz + x - inX + posVec] = inM[(y%N)*N + (x%N)];
		}
	}
	
	barrier(CLK_GLOBAL_MEM_FENCE);
	
	//Calcular cuadrado
	double sum;		
	iter =  siz/NumWorkItems;
	for(int i = id*iter; i < (id+1)*iter ;  i++ ){
		for(int j = 0 ; j < siz ; j++){
			sum = 0;
			for(int k = 0 ; k < siz ; k++){
				sum += Bin[i*siz + k + posVec] * Bin[k*siz + j + posVec];
			} 
			Bout[i*siz + j + posVec] = sum;
		}
	}

	//barrier(CLK_GLOBAL_MEM_FENCE);
}


__kernel void suma(__global double *A, __global double *B, __global terna_t *ternas, int inPos, int idT, int N){
	int siz = ternas[idT].t;
	int inY = ternas[idT].y; 
	int inX = ternas[idT].x;
	int id = get_global_id(0);
	int iter = siz/get_global_size(0);
	
	for(int y = get_global_id(0)*iter + inY ; y < (iter*(id+1)) + inY ; y++){
		for(int x = inX ; x < (siz + inX) ; x++){
			A[(y%N)*N + (x%N)] += B[(y - inY)*siz + (x - inX) + inPos];
		}
	}
	
	
	if(id == 0){
		printf("1___1\n");
		for(int y = 0 ; y < N ; y++){
			for(int x = 0 ; x < N ; x++)
				printf("%lf ", A[y*siz + x]);
			printf("\n");
		}
		
	}
	
}
