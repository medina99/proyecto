#include <iostream> //libreria para control de ingreso y salida estandar
#include <cmath>  //libreria de funciones matematicas, usamos hypot (hipotenuza) para hallar las distancias
#include <fstream> //libreria de manejo de archivos
#include <sstream> //libreria para manejo de flujos (usada en la funcion "cargar_archivo")
#include <vector>  //vector dinamico para guardar los puntos
#include <assert.h> //libreria usada para verificar que los ficheros se abran correctamente
using namespace std;

class Puntos{

	private:
		ofstream Archivo; //Variable que maneja el fichero de salida
		string nombre_archivo; //nombre del archivo .csv
		vector<string> Nombre; //lista de los nombres de los puntos
		vector<float> X, Y; //lista de las coordenadas (X, Y)
		vector<char> Estado; //variable de estado A, 
		
	public:

		Puntos(const string archivo = "Puntos.csv"): nombre_archivo(archivo){} //Inicializacion del nombre del archivo, se puede cambiar
		void cargar_archivo();                                            //Funcion para cargar lo que ya habia en el archivo con nombre: "nombre_archivo"
		bool Eliminar_nombre(const string);                               //Opcion 2. Eliminar nombre
		void Listar_100_primeros() const;	                              //Opcion 3. Listar los 100 primeros puntos (suponiendo lo 100 primeros ingresados).
		void Listar_10_metros(float, float) const;                        //Opcion 4. Listar los puntos en un radio de 10 metros de un punto especificado por entrada
		float distancia_promedio() const;                                 //Opcion 5. Distancia promedio, es la suma de distancias sobre la cantidad de pares C(n, 2) "combinatoria de n en 2"	
		float distancia_mas_lejana() const;                               //Opcion 6. Distancia mas lejana, se itera sobre todas las posiblidades hasta hallar la maxima
		float distancia_mas_corta() const;                                //Opcion 7. Distancia mas corta, se itera sobre todas las posibilidades hasta hallar la minima
		void ruta_mas_corta(float, float, float, float);                  //Opcion 8. Ruta mas corta. Se considera la ruta con menor distancia solo pasando por puntos ya ingresados
		void ruta(vector<int>&, int);	                                  //Permite generar la ruta mas corta
		void contorno();                                                 /*Opcion 9. funcion para calcular las ciudades que estan al borde
		
		                                                                           /.\________
		                                                                          /            .\
		                                                                        /     . .       .\____   
	                                                        	              /.                     .|
		                                                                      |___   .   .  . .     ./
		                                                                          \___             /
		                                                                              \.         ./
		                                                                               ----------*/
		
		friend istream& operator>> (istream&, Puntos&);                  //Sobrecarga del operador >> para la lectura de elementos (Opcion 1)
		int MENU() const;                                                //Funcion para mostrar el menu

} control;      //Variable de la clase creada


/* INT MENU()   
    funcion para desplegar el menu.
    ingreso: ~
    salida: opcion elegida
*/

int Puntos::MENU() const{
	cout << "MENU\n" 
	  		"~~~~\n"
			"1. Agregar una nueva posicon.\n"
			"2. Eliminar una posicion por nombre.\n"
			"3. Listar 100 primeros puntos.\n"
			"4. Listar los puntos ubicados a menos de 10 metros.\n"
			"5. Calcular la distancia promedio entre cada punto.\n"
			"6. Determinar la distancia entre los puntos mas lejano\n"
			"7. Determinar la distancia mas corta entre los puntos\n"
			"8. Hallar camino mas corto entre dos puntos a traves de otros puntos\n"
			"9. Hallar el contorno de los puntos\n"
			"0. Para salir\n"
			">? ";

	return cin.get();
}


/*sobrecara del operacor >>
    Funcion de sobrecarga del operador de corrimiento de bits para hacer la lectura del punto
    ingreso: flujo de entrada, clase Puntos
    salida: flujo de salida para no evitar que se interrumpa (se puede prescindir de ello)
*/
istream& operator>>(istream& In, Puntos& C){
	
	//variables para guaradar la informacion del punto
	string nombre;
	float  x, y;
	char estado;
	
	
	//Peticion de ingreso de las variables
	cout << "Ingrese:" << endl;
	cout << "Nombre: ";
	In >> nombre;
	cout << "Coordenada: ";
	In>> x >> y;
	cout << "Estado: ";
	In.ignore(); //ignorando espacio en blanco
	In >> estado;
	In.ignore(); //ignorando fin de linea
	
	
	

	if(nombre.find(",") != string::npos) //verificando que no haya comas
		cout << "ERROR!, nombre no permite comas" << endl;
		
	for(int i = 0; i < C.Nombre.size(); ++i) //verificando que no existe el nuevo nombre
		if(C.Nombre[i].find(nombre) != string::npos){
			cout << nombre << " ya esta en uso" << endl;
			return In;
		}			


    //ingresando los elementos en las listas
	C.Nombre.push_back(nombre); 
	C.X.push_back(x);
	C.Y.push_back(y);
	C.Estado.push_back(estado);
			
			
    //abriendo los archivos para la escritura al final
	C.Archivo.open(C.nombre_archivo.c_str(), std::ofstream::out | std::ofstream::app);
	assert(C.Archivo);		

    //escribiendo los datos sin espacios y separados por comas
	C.Archivo << nombre << "," << x << "," << y << "," << estado << endl;
	//cerrando el archivo
	C.Archivo.close();

    //retornando el flujo de entrada (se puede obviar y hacer que la funcion devuelva void)
	return In;
}


/*Funcion para hallar el contorno o capsula convexa de los puntos
    entrada: ~
    salida: ~, la funcion despliega el contorno de los puntos ya ingresados.
*/
void Puntos::contorno(){
    
	if(X.size() < 2){ //virificar si es que hay menos de dos puntos
		if(X.empty()) cout << "Se necesita al menos un punto" << endl; //se evita cuando no hay puntos almacenados
		else cout << "Contorno\n" << Nombre[0] << " " << X[0] << " " << Y[0] << endl; //se imprime el unico punto
		return;
	}

	vector<int> next(X.size()); //se toma el vector next (siguiente) para mostrar el contorno en orden
	int init = -1, t = 0;
	for(int i = 0; i < X.size(); ++i) //se elije el punto i, j para ver si forman parte del contorno		
		for(int j = 0; j < X.size(); ++j)
			if(i != j){
			        
			    //se procede a verificar si forman parte del contorno
				int m = 0;
				for(int k = 0; k < X.size(); ++k) 
					if((X[j] - X[i])*(Y[k] - Y[i]) + (X[k] - X[i])*(Y[j] - Y[i]) >= 0) //se verifica que un semi-espacio que pasa por (x_i, y_i), (x_j, y_j) contenga todos los puntos  
						m++;
					
				if(m == X.size()){ //si es que el semi-espacio contiene todos los puntos, decimos que es parte del contorno
					next[i] = j;
						t++;
						init = i; //guardamos cualquier punto del contorno para comenzar a imprimir desde ahi
				}
			}				

	cout << "Contorno" << endl; //imprimimos el contorno con el vector next
	while(t--){
		cout << Nombre[init] << " " << X[init] << " " << Y[init] << endl;
		init = next[init];	//pasamos al siguiente punto
	}
}


/*
    Eliminar_nombre: permite borrar un punto dado el nombre
    entrada: nombre del punto a eliminar
    salida: una variable booleana (false, true) que te confirme que se elimino un punto con ese nombre
*/
bool Puntos::Eliminar_nombre(string nombre){
	
	//declaracion de variables
	ifstream aux; 
	bool ok = false;		
	int pos = 0;	

    //abriendo el archivo de los puntos, para lectura
	aux.open(nombre_archivo.c_str(), ifstream::in);
	//se verifica que se abra correctamente el archivo
    assert(aux);
	//abriendo un archivo temporal para escritura
	Archivo.open("temp.csv", ofstream::out);
	//se verifica que se abra bien el archivo
    assert(Archivo);
    //variable para leer las lineas que hay en nuestro archivo csv
	string line;
			
	while(getline(aux, line)){ //leyendo de nuestro arvhico
		if(line.find(nombre) == string::npos) //si el nombre no coincide lo pasamos a temp.csv
			Archivo << line << endl; 
		else{ //en caso de que coincida lo eliminamos de nuestras listas
			Nombre.erase(Nombre.begin()+pos); 
			X.erase(X.begin()+pos);
			Y.erase(Y.begin()+pos);
			Estado.erase(Estado.begin()+pos);
			ok = true; //quiere decir que se ha eliminado
		}
		pos++; //voy contando en que linea estoy para saber cual voy a eliminar
	}

	if(ok) //imprimo si he eliminado o no el elemento
		cout << "Punto eliminado exitosamente." << endl;
    else
        cout << "No se encontro el nombre especificado." << endl;
	
	//elimino mi archivo de puntos
	remove(nombre_archivo.c_str());
	//cambio el nombre de mi archivo temp.csv para el nombre de mi archivo de puntos
	rename("temp.csv", nombre_archivo.c_str());
	
	//cierro los archivos
	aux.close();
	Archivo.close();
	
	//retono si ha sido exitosa la eliminacion
	return ok;
}


/* 
    Listar los 100 primeros nombres, se listan los 100 primeros asi no haya elementos, se rellena de ~ hasta llegar a 100
    entrada: ~
    salida: ~
*/

void Puntos::Listar_100_primeros() const{
	for(int i = 0; i < 100; ++i) //irtero 100 veces para imprimir los 100 primeros, pero respetando de no imprimir mas de los que he guardado
		if(i < X.size())
			cout << i+1 << ". " << Nombre[i] << " " <<  X[i] << " " << Y[i] << " " << Estado[i] << endl;
		else
			cout << i+1 << ". ~" << endl;
}


/*
    listar 10 metros, permite listar los elementos que se encuentra hasta 10 metros de un punto ingresado
    entrada: punto de referencia
    salida: ~

*/
void Puntos::Listar_10_metros(float x, float y) const{
	int k = 0;
	for(int i = 0; i < X.size(); ++i) //busco entre todos los puntos, los que estas a una distancia euclidea menor o igual a 10.0
		if(hypot(X[i] - x, Y[i] - y) <= 10.0f){
			cout << k+1 << ". " << Nombre[i] << " " <<  X[i] << " " << Y[i] << " " << Estado[i] << endl;
			k++;
		}

	if(k == 0) //en caso de que no haya encontrado nada, imprimo que no se han encontrado
		cout << "Puntos no encontrados." << endl; 
}


/*
    distancia promedio: se saca un promedio ponderado de todos los pares de puntos que son n*(n-1)/2
    entrada: ~
    salida: promedio de las distancias de los puntos que se tienen almacenados
*/
float Puntos::distancia_promedio() const{
    
    if(X.size() <= 1) //si es que solo hay un elemento o menos no puedo medir distancias
        return cout << "Se requiere almenos dos puntos" << endl, -1.0f;
    
	float E = 0.0f; //aqui guardo las distancias
	int n = X.size();
	for(int i = 0; i < n; ++i) //itero sobre todos los elemntos
		for(int j = i+1; j < n; ++j) //y sobre todos los j que forman pares con i tal que no se repitan
			E += hypot(X[i] - X[j], Y[i] - Y[j]); //calculo la distancia euclidea y la sumo		
		
	return 2.0f*E/(n*(n-1.0f)); //retorno E / (n*(n-1)/2)
}			


/*
    distancia mas lejana: halla la distancia mas lejana entre los puntos
    entrada: ~
    salida: distancia mas lejanda entre los puntos
*/		
float Puntos::distancia_mas_lejana() const{
	float mas_lejana = 0.0; //comienzo con la distancia mas pequegna, que es 0
			
	//si hay menos de dos puntos no puedo obtener distacnias
	if(X.size() <= 1) return cout << "Se requiere al menos dos ciudades." << endl, -1.0f;
	
	
	//itero sobre todos los pares y maximizo las distancias
	for(int i = 0; i < X.size(); ++i)
		for(int j = i+1; j < X.size(); ++j)
			mas_lejana = max(mas_lejana, hypot(X[i] - X[j], Y[i] - Y[j]));


	return mas_lejana;
}


/*
    distancia mas corta: halla la distancia mas corta entre los puntos
    entrada: ~
    salida: distancia mas corta entre los puntos.
*/
float Puntos::distancia_mas_corta() const{
	float mas_corta = 1000000000.0f; //inicio para una distancio muy grande que signifique infinito

    //si hay menos de dos elementos no puedo obtener distancias			
	if(X.size() <= 1) return cout << "Se requiere al menos dos ciudades." << endl, -1.0f;
	
	
	//itero sobre todos los pares yminimizo las distancias encontradas
	for(int i = 0; i < X.size(); ++i)
		for(int j = i+1; j < X.size(); ++j)
			mas_corta = min(mas_corta, hypot(X[i] - X[j], Y[i] - Y[j]));

	return mas_corta;
}


/*
    ruta: funcion para imrprimir la ruta mas corta, generada en la funcion siguiente
    entrada: vector de padres o de antecesores, posicion actual a imprimir
    salida: ~
*/
void Puntos::ruta(vector<int>& p, int pos){
	if(pos == -1) return; //en caso de que ya no haya un punto atras en el camino retorno
	ruta(p, p[pos]); //voy al punto de atras en mi camino
	cout << Nombre[pos] << " " << X[pos] << " " << Y[pos] << endl; //imprimo el punto actual	
}



/*
    ruta mas corta: algoritmo para hallar la ruta mas corta entre dos puntos usando algun punto 
    ya ingresado.
    entrada: punto de entrada, punto de salida
    salida: ~
*/
void Puntos::ruta_mas_corta(float partida_x, float partida_y, float llegada_x, float llegada_y){
	if(X.empty()){ //si no hay puntos no se puede proceder
		cout << "Se necesita al menos un punto" << endl;
		return;
	}

	vector<float> d(X.size()); //vector de distancias desde el punto de llegada
	vector<bool> vis(X.size(), false); //vector que nos indica si un nodo ya ha sido usado
	vector<int> p(X.size()); //vector de antecesores en el camino
	
	
	//Se inicializa las distancias con las distancias desde el punto de partida hacia los puntos alamaenados
	//y se marca a todos con antecesor -1 para indicar que el anterior nodo es el punto de partida
	for(int i = 0; i < X.size(); ++i){
		d[i] = hypot(partida_x - X[i], partida_y - Y[i]); 
		p[i] = -1;
	}


    //controlando el numero de iteraciones
	int m = 0;
	while(m < X.size()){
	    
	    //variables para elegir el nodo con menor distancia
		float dist = 1000000000.0f;
		int v = -1;

        //elegimos el nodo con menor distancia desde el punto de partida
		for(int i = 0; i < X.size(); ++i)
			if(not vis[i] and dist >= d[i]){
				dist = d[i];
				v = i;
			}
		if(v == -1) break;
        //visitamos ese nodo para ya no tocarlo en la siguiente iteracion
		vis[v] = true;
		
		//actualizando todos los puntos a aprtir del punto escogido
		for(int i = 0; i < X.size(); ++i)
			if(not vis[i] and i != v and d[i] > d[v] + hypot(X[v] - X[i], Y[v] - Y[i])){
				d[i] = d[v] + hypot(X[v] - X[i], Y[v] - Y[i]);
				p[i] = v;	
			}  m += 1;
	}


    //variables para elegir el punto mas cerca de la llegada
	int pre_terminal = -1;
	float pre_dist = 1000000000.0f;
	
	
	//elegimos el punto con menor distancia desde la partida hasta la llegada pasando por los puntos
	//que ya tengo guardados
	for(int i = 0; i < X.size(); ++i)
		if(pre_dist > d[i] + hypot(X[i] - llegada_x, Y[i] - llegada_y)){
			pre_dist = d[i] + hypot(X[i] - llegada_x, Y[i] - llegada_y);
			pre_terminal = i;
		}

    //imptimiendo el camino minimo
	cout << "Distancia de la ruta mas corta: " << pre_dist << endl;
	cout << "Inicio " << partida_x << " " << partida_y << endl;
	ruta(p, pre_terminal);
	cout << "Final " << llegada_x << " " << llegada_y << endl;
}


/*
    cargar archivos: funcion para cargar los puntos ya ingresados antes de empezar a correr el programa
    entrada: ~
    salida: ~
*/
void Puntos::cargar_archivo(){
	
	ifstream aux;
	//abriendo el archivo de puntos para lectura
	aux.open(nombre_archivo.c_str(), ifstream::in);
    //se verifica que se abre correctamente el archivo
    assert(aux);

    //variable para leer todas las lineas de mi archivo csv
	string line;
	//variables para leer y guardar en mi lista
	string nombre;
	float x, y;
	char c;
	
	
	//leemos todas las lineas
	while(getline(aux, line)){
	    
	    //quitamos las comas del archivo csv y le ponemos espacio
		for(int i = 0; i < line.size(); ++i)
			if(line[i] == ',')
				line[i] = ' ';
	
	    //creamos un frujo de entrada a partir de la linea que tenia
		istringstream L(line);
		   
		//leemos nuestras variables (se lee dos veces c para evitar el espacio)
		L >> nombre >> x >> y >> c >> c;
		
		//guardamos en las listas
		Nombre.push_back(nombre);
		X.push_back(x);
		Y.push_back(y);
		Estado.push_back(c);
	}
	
	//cerramos el archivo csv
	aux.close();
}
		

int main(){
	
	control.cargar_archivo(); //cargando los archivos

	bool nueva_opcion = true; //variable que indica si hay una nueva opcion
	string aux; //string auxiliar para lectura
	float x, y, w, z; //variables auxiliares para leer puntos
	while(nueva_opcion){
		int op = control.MENU(); //despliego de menu 
		cin.ignore(); //ignorar la siguiente tecla (enter)
		
		//clasificar las opciones
		switch(op){	
			
			case '1':
			    //usamos el operador sobrecargado para leer los elementos
				cin >> control;
				break;
			case '2':
			    //leemos el nombre para borrar y lo pasamos a la clase 
				cout << "Que nombre desea borrar? >";
				getline(cin, aux);			
				control.Eliminar_nombre(aux);
				break;
			case '3':
			    //impresion de los 100 primeros
				cout << "Lista de los 100 primeros puntos:" << endl;
				control.Listar_100_primeros();
				break;
			case '4':
			    //Se ingtresa punto de referencia para luego imprimir los puntos que estan a lo mas 10.0 de distancia
				cout << "Ingrese el punto referencia: >";
				cin >> x >> y; cin.ignore();
				control.Listar_10_metros(x, y);
				break;
			case '5':
				cout << "Distancia promedio entre puntos: " << control.distancia_promedio() << endl;
				break;
			case '6':
				cout << "La distancia mas lejana es: " << control.distancia_mas_lejana() << endl;
				break;
			case '7':
				cout << "La distancia mas cercana es: " << control.distancia_mas_corta() << endl;
				break;
			case '8':
			    //se indica punto de partida y de llegada para pasarlo a la funcion
				cout << "Indique punto de partida: >";
				cin >> x >> y;
				cout << "Indique punto de llegada: >";
				cin >> w >> z; cin.ignore();
				control.ruta_mas_corta(x, y, w, z);
				break;
			case '9':
			    //se imrprimre el contorno de los dator que tenemos
				control.contorno();		
				break;
			case '0':
			    //se dice que ya no haga mas iteraciones y sale del programa
			    cout << "Usted ha seleccionado la opcion de salir." << endl;
				nueva_opcion = false;
				break;
		}
	}


	return 0;
}