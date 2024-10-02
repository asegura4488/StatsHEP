#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>

void Leer(){

	std::vector<double> n_values, b_values, s_values;

	std::ifstream infile("datos.txt"); // Archivo de entrada
	if (!infile.is_open()){
		std::cerr << " Error al abrir el archivo de datos " << std::endl;
		return; 
	}

	std::string line;
	char comma;

	// Leer el archivo linea por linea saltando encabezado
	bool first_line = true;
	while (std::getline(infile,line)){


		if (first_line){
			first_line = false; 
			continue;
		}


		// Variables para almacenar temporalemente n,b,s
		double n,b,s;

		// Crear un stream a partir de la linea para leer n,b,s

		std::istringstream ss(line);
		if (!(ss >> n >> comma >> b >> comma >> s)){
			std::cerr << "Error al leer los valores en la linea: " << line << std::endl;
			continue;
		}

		n_values.push_back(n);
		b_values.push_back(b);
		s_values.push_back(s);

	}

	infile.close(); // Cerrar archivo de datos


	// Para verificar iteramos los vectores
	for ( size_t i = 0; i < n_values.size(); ++i ){
		std::cout << "Channel: " << i << std::endl;
		std::cout << n_values[i] << std:: endl;
		std::cout << b_values[i] << std:: endl;
		std::cout << s_values[i] << std:: endl;
	}

}




int main(){


	Leer();
	return 0;
}