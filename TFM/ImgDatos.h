#pragma once
#include<opencv2/core/core.hpp> // Librerías necesarias para usar OpenCV
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>


// Esta clase va a almacenar datos de la imagen utilizada en una matriz. Se construye la clase con un constructor que utiliza un imagen.
class imgDatos
{
public:
	imgDatos(cv::Mat imagenRef);
public:
	// Estos miembros de la clase han de ser inicilizados con datos de la matriz imagenRef que se le pasa al constructor. 
	// En el .cpp se inicializan
	cv::Mat conect; // Matriz que refleja el valor 0 si es un píxel fuera del umbral de Otsu y 255 si es un píxel seleccionado
	cv::Mat numObj; // Matriz que marca el número de objeto en el que está cada píxel
	std::vector<int> tamObj; // cada posición del vector identifica el tamaño de los objetos en número de píxeles
	int contObj=0;
public:
	void formaObjetos(int i, int j, cv::Mat* imagenUmbral);

};