/*
 * Paulina Vara Figueroa
 * 2019
 * Algoritmos de interpolación iterada
 */

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
//Variables globales

float *untouchedPuntos = NULL;
float *puntos = NULL; //Guarda los puntos conocidos que ingresa el usuario
float *derivadas = NULL;//Guarda los valores de las derivadas utilizados en Hermite
float **interpSpace = NULL; //Genera una matriz en la que se irá realizando el algoritmo
                        //Esta matriz debe ser de n x n por eso se genera en cuanto se conoce n
float *untouchedSpace = NULL;//Guarda la primera columna de interpSpace pero nunca se modifica 
                            //(en caso de que se quieran usar los mismos valores con un método diferente)
int n; //Número de puntos que ingresa el usuario
float searched_x; //Valor x cuya evaluacion f(x) es buscada
int hermiteValid = 0; //Variable que valida si se está usando el método Hermite, es global ya que afecta el área de trabajo de todas las funciones.
//Métodos de apoyo
void imprimePuntos()
{
    int i, size;
    size=n;
    if(hermiteValid==3)
        size *= 2;
    for (i=0; i<size; i++)
        printf("x%d = %10f || f(x%d) = %10f\n",i, puntos[i], i, interpSpace[i][0]);
}

void print_interSpace()
{
    int i, j, size;
    size = n;
    if (hermiteValid == 3)//Si se está usando Hermite
        size *= 2; //Se debe considerar el tamaño doble por las 'z'
    printf("\n");
    for (i=0; i<size; i++){
        for(j=0; j<size; j++){
            printf("%12.5f",interpSpace[i][j]);
        }
        printf("\n");
    }
}

void createInterpSpace ()//Crea el espacio para aplicar el método de interpolación
{
    int i, j;
    int size;
    size = n;
    if (hermiteValid == 3)//Si se pidió usar el método de Hermite
    {
        derivadas = (float*)malloc(size*sizeof(float));
        size *= 2;//Necesitamos el doble de espacio para interpSpace en el método Hermite
    }
    interpSpace = (float**)malloc(size*sizeof(float*));
    for (i=0; i<size; i++)
        interpSpace[i] = (float*)malloc(size*sizeof(float));
    for (i=0; i<size; i++){
        for(j=0; j<size; j++)
            interpSpace[i][j] = 0;
    }
}

void createUntouchedSpace ()
{
    int i, j;
    untouchedSpace = (float*)malloc(n*sizeof(float));
    for (i=0; i<n; i++)
        untouchedSpace[i] = 0;
}

void askPointsAmount()//Pide el número de puntos y llama función que crea el espacio para aplicar el método de intepolación
{
    n = -1;
    int i;
    while(n<=0)
    {
        printf("\nIndique la cantidad de puntos que ingresara para la aproximacion:  ");
        scanf("%d",&n);
        if(n<=0)
        {
            system("cls");
            printf("----------  Cantidad invalida de puntos ----------\n\n");
        }
    }
    createInterpSpace();
    createUntouchedSpace();
}

void llenado_Puntos ()//Crea y llena la matriz de puntos que ingresa el usuario
                        //También guarda la x que está buscando
{
    int i, size, i_deriv, i_untouched;
    size = n;
    if (hermiteValid == 3)//Si se esta utilizando Hermite se usa el doble de estacio por las z's
        size *= 2;
    puntos = (float*)malloc(size*sizeof(float));
    if (hermiteValid == 3){
        printf("\nA continuacion indique cuales son dichos puntos, sus respectivas evaluaciones y valores de derivacion:\n");
        untouchedPuntos = (float*)malloc(size*sizeof(float));
    }
    else
        printf("\nA continuacion indique cuales son dichos puntos y sus respectivas evaluaciones:\n");
    i_deriv = 0;
    i_untouched = 0;
    for (i=0;i<size;i++)
    {
        printf("Punto x%d:  ", i_untouched);
        scanf("%f", &puntos[i]);
        if (hermiteValid == 3){//Si se está trabajando hermite se guarda doble (por la generación de z's)
            puntos[i+1] = puntos[i];
            untouchedPuntos[i_untouched] = puntos[i];
        }
        printf("Evaluacion f(x%d):  ", i_untouched);
        scanf("%f",&interpSpace[i][0]);
        untouchedSpace[i_untouched] = interpSpace[i][0];
        i_untouched++;//Usa otro índice porque el respaldo "untouched" no guarda doble aunque sea Hermite
        if(hermiteValid == 3){//Si se está trabajando hermite se guarda doble (por la generación de z's)
            interpSpace[i+1][0] = interpSpace[i][0];
            printf("Evaluacion f'(x%d): ", i_deriv);
            scanf("%f",&derivadas[i_deriv]);
            i_deriv++;//Es el índice para los valores de derivación, esos no se guardarían dobles
            i++;
        }
            //Si se está trabajando hermite se guarda doble (por la generación de z's)  
    }
    printf("\nIndique cual es el dato \"x\" cuya evaluacion f(x) se desea aproximar:  ");
    scanf("%f", &searched_x);
}

void liberaInterpSpace ()
{
    int i, size;
    size = n;
    if(hermiteValid==3)
        size *= 2;
    for (i=0; i<size; i++)
        free(interpSpace[i]);
    free(interpSpace);
}

//Métodos de interpolación
void iterada_Neville ()// Interpolación iterada de Neville
{
    int i, j;
    printf("INTERPOLACION ITERADA DE NEVILLE\n\n");
    printf("Para aproximar x = %f se utilizan los puntos:\n\n", searched_x);
    imprimePuntos();
    for(i=1; i<n; i++)
    {
		for(j=i; j<n; j++)
			interpSpace[j][i] = ((searched_x-puntos[j-i]) * interpSpace[j][i-1] - (searched_x-puntos[j]) * interpSpace[j-1][i-1]) / (puntos[j]-puntos[j-i]);
    }
    print_interSpace();
    printf("\nEl valor f(%f) aproximado es: %f",searched_x, interpSpace[n-1][n-1]);
    printf("\n");
}

void diferencia_dividida_Newton (int alredyDone)//Diferencia dividida interpolante de Newton
    //alreadyDone indica si ya hizo el algoritmo y solo se va a repetir para evaluar otro dato (caso 2)
    //o si es un proceso con puntos nuevos (caso 1)
{
    int i, j;
    float result, tempo;
    printf("DIFERENCIAS DIVIDIDAS DE NEWTON\n\n");
    printf("Para aproximar x = %f se utilizan los puntos:\n\n", searched_x);
    imprimePuntos();
    if(alredyDone == 1)//Si es un proceso con datos nuevos
    {
        for(i=1; i<n; i++)
        {
			for(j=i; j<n; j++)
				interpSpace[j][i] = (interpSpace[j][i-1] - interpSpace[j-1][i-1]) / (puntos[j] - puntos[j-i]); 
		}
    }
    result = interpSpace[0][0];
	for(i=1; i<n; i++)
    {
		tempo = interpSpace[i][i];
		for(j=0; j<i; j++)
			tempo *= (searched_x - puntos[j]);
		result += tempo;
	}
    print_interSpace();
    printf("\nEl valor f(%f) aproximado es: %f",searched_x, result);
    printf("\n");
} 

void inter_hermite (int alredyDone)//Interpolación de Hermite
    //alreadyDone indica si ya hizo el algoritmo y solo se va a repetir para evaluar otro dato (caso 2)
    //o si es un proceso con puntos nuevos (caso 1)
{
    int i, j, size, i_deriv;
    float result, tempo;
    size = n*2;
    printf("INTERPOLACION DE HERMITE\n\n");
    printf("Para aproximar x = %f se utilizan los puntos:\n\n", searched_x);
    imprimePuntos();
    if(alredyDone == 1)//Si se hace el proceso por primera vez para esos datos
    {
        i_deriv = 0;
        for(i=1;i<size; i++)
        {
            interpSpace[i][1] = derivadas[i_deriv];
            i_deriv++;
            i++;
            if(i<size)
                interpSpace[i][1] = (interpSpace[i][0] - interpSpace[i-1][0]) / (puntos[i] - puntos[i-1]);
        }//Hace la primera iteración considerando las derivadas
        
        for(i=2; i<size; i++)
        {
            for(j=i; j<size; j++)
                interpSpace[j][i] = (interpSpace[j][i-1] - interpSpace[j-1][i-1]) / (puntos[j] - puntos[j-i]); 	
        }//Se hace el proceso de diferencias divididas en adelante
    }
    result = interpSpace[0][0];
	for(i=1; i<size; i++){
		tempo = interpSpace[i][i];
		for(j=0; j<i; j++){
			tempo *= (searched_x - puntos[j]);
		}
		result += tempo;
	}
    print_interSpace();
    printf("\nEl valor f(%f) aproximado es: %f",searched_x, result);
    printf("\n");
}

//Funciones menú
int menu_metodos ()
{
    int opc = -1;
    printf("PROGRAMA DE INTERPOLACION\n");
    while(opc<0 || opc>3)
    {
        printf("\nQue metodo desea utilizar para encontrar el valor del punto dado?\n");
        printf("1.  Interpolacion iterada de Neville\n");
        printf("2.  Diferencias divididas de Newton\n");
        printf("3.  Interpolacion de Hermite\n");
        printf("\n0.  Salir\n");
        printf("Opcion:    ");
        scanf("%d",&opc);
        if(opc<0 || opc>3)
            printf("\n---Por favor elija otra opcion---\n");
    }
    return opc;
}

int menu_metodos_noHermite ()//No incluye al método de Hermite si los puntos ya fueron creados sin derivadas
{
    int opc = -1;
    printf("PROGRAMA DE INTERPOLACION\n");
    while(opc<0 || opc>2)
    {
        printf("\nQue metodo desea utilizar para encontrar el valor del punto dado?\n");
        printf("1.  Interpolacion iterada de Neville\n");
        printf("2.  Diferencias divididas de Newton\n");
        printf("\n0.  Salir\n");
        printf("Opcion:    ");
        scanf("%d",&opc);
        if(opc<0 || opc>2)
            printf("\n---Por favor elija otra opcion---\n");
    }
    return opc;
}

int menu_afterMethod()
{
    int opc_after = -1;
    while (opc_after < 1 || opc_after > 4)
    {
        printf("\nDesea:\n1.   Aproximar otro valor con los mismos datos\n");
        printf("2.   Cambiar el metodo de interpolacion para los mismos datos\n");
        printf("3.   Comenzar un nuevo caso de interpolacion\n");
        printf("4.   Salir del programa\nOpcion:   ");
        scanf("%d",&opc_after);
        if(opc_after < 1 || opc_after > 4)
            printf("\n---Por favor vuelva a ingresar la opcion---\n");
    }
}

int main ()
{
    int metodo, i, j;
    metodo = menu_metodos();
    if (metodo == 3)//Si se eligió el método de interpolación de Hermite se activa su bandera
        hermiteValid = 3;
    askPointsAmount();
    llenado_Puntos();
    while (metodo != 0)
    {
        system("cls");
        switch (metodo)
        {
        case 1:
            iterada_Neville();
            switch (menu_afterMethod())
            {
            case 1://Buscar otra x, mismos datos
                liberaInterpSpace();
                createInterpSpace();
                for (i=0; i<n; i++)
                    interpSpace[i][0] = untouchedSpace[i];
                system("cls");
                printf("PROGRAMA DE INTERPOLACION\n");
                printf("\nIndique cual es el dato \"x\" cuya evaluacion f(x) se desea aproximar:  ");
                scanf("%f", &searched_x);
                system("cls");
                iterada_Neville();
                break;
            case 2://Misma busqueda, cambio de método
                liberaInterpSpace();
                createInterpSpace();
                for (i=0; i<n; i++)
                    interpSpace[i][0] = untouchedSpace[i];
                system("cls");
                printf("PROGRAMA DE INTERPOLACION\n");
                metodo = menu_metodos_noHermite();//No permite Hermite porque ya se tienen los datos (sin derivadas)
                break;
            case 3://Vuelve a iniciar TODO
                liberaInterpSpace();
                free(puntos);
                free(untouchedSpace);
                system("cls");
                metodo = menu_metodos();
                if (metodo != 0)
                {
                    if(metodo == 3)
                        hermiteValid = 3;
                    askPointsAmount();
                    llenado_Puntos();
                }
                break;
            case 4://Termina el programa
                liberaInterpSpace();
                free(puntos);
                free(untouchedSpace);
                metodo = 0;
                break;
            }
            break;
        case 2:
            diferencia_dividida_Newton(1);//Se manda un 1 pues es la primera vez que se hace y así valida que haga todo el método
            switch (menu_afterMethod())
            {
            case 1://Buscar otra x, mismos datos
                system("cls");
                printf("PROGRAMA DE INTERPOLACION\n");
                printf("\nIndique cual es el dato \"x\" cuya evaluacion f(x) se desea aproximar:  ");
                scanf("%f", &searched_x);
                diferencia_dividida_Newton(2);//Mandar un dos para indicar que ha se hizo el proceso 
                                    //y solo se va a evaluar otro punto (para reutilizar lo que ya calculó)
                break;
            case 2://Misma busqueda, cambio de método
                liberaInterpSpace();
                createInterpSpace();
                for (i=0; i<n; i++)
                    interpSpace[i][0] = untouchedSpace[i];
                system("cls");
                printf("PROGRAMA DE INTERPOLACION\n");
                metodo = menu_metodos_noHermite();//No permite Hermite porque ya se tienen los datos (sin derivadas)
                break;
            case 3://Vuelve a iniciar TODO
                liberaInterpSpace();
                free(puntos);
                free(untouchedSpace);
                system("cls");
                metodo = menu_metodos();
                if (metodo != 0)
                {
                    if(metodo == 3)
                        hermiteValid = 3;
                    askPointsAmount();
                    llenado_Puntos();
                }
                break;
            case 4://Termina el programa
                liberaInterpSpace();
                free(puntos);
                free(untouchedSpace);
                metodo = 0;
                break;
            }
            break;
        case 3:
            hermiteValid = 3;
            inter_hermite(1);
            switch (menu_afterMethod())
            {
            case 1://Buscar otra x, mismos datos
                system("cls");
                printf("PROGRAMA DE INTERPOLACION\n");
                printf("\nIndique cual es el dato \"x\" cuya evaluacion f(x) se desea aproximar:  ");
                scanf("%f", &searched_x);
                inter_hermite(2);
                break;
            case 2://Misma busqueda, cambio de método
                liberaInterpSpace();
                hermiteValid = 0;
                createInterpSpace();
                for (i=0; i<n; i++)
                    interpSpace[i][0] = untouchedSpace[i];
                free(puntos);
                puntos = (float*)malloc(n*sizeof(float));
                for (i=0; i<n; i++)
                    puntos[i] = untouchedPuntos[i];
                free(derivadas);
                system("cls");
                printf("PROGRAMA DE INTERPOLACION\n");
                metodo = menu_metodos_noHermite();
                break;
            case 3://Vuelve a iniciar TODO
                liberaInterpSpace();
                free(puntos);
                free(derivadas);
                free(untouchedSpace);
                free(untouchedPuntos);
                hermiteValid = 0;
                system("cls");
                metodo = menu_metodos();
                if (metodo != 0)
                {
                    if(metodo == 3)
                        hermiteValid = 3;
                    askPointsAmount();
                    llenado_Puntos();
                }
                break;
            case 4://Termina el programa
                liberaInterpSpace();
                free(puntos);
                free(derivadas);
                free(untouchedSpace);
                free(untouchedPuntos);
                metodo = 0;
                break;
            }
            break;
        case 0:
            break;
        }
    }
    system("cls");
    printf("FINALIZA PROGRAMA DE INTERPOLACION\n\n");
    system("pause");
    return 0;
}