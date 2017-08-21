#pragma once
#include<opencv2/core/core.hpp> // Librerías necesarias para usar OpenCV
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>


// Esta clase va a almacenar datos de la imagen utilizada en una matriz. Se construye la clase con un constructor que utiliza un imagen.
class imgDatos
{
public:
	imgDatos(cv::Mat& imagenRef);
	/* Dentro del constructor de la clase se añaden todos los procedimientos para la construcción de los miembros de la clase: se conectan
	los objetos de la imagen umbralizada, se cuentan el número de píxeles del objeto más grande, su centroide, el valor de todos sus píxeles,
	sus momentos de inercia y el ángulo de giro para ejes principales y su contorno.*/
public:
	// Estos miembros de la clase han de ser inicilizados con datos de la matriz imagenRef que se le pasa al constructor. 
	// En el .cpp se inicializan
	cv::Mat conect; // Matriz que refleja el valor 0 si es un píxel fuera del umbral de Otsu y 255 si es un píxel seleccionado
	int tamObj; // identifica el tamaño en píxeles del objeto más grande
	cv::Vec2f centroide; // Se trata de una pareja de floats que localizan la posición del centroide del objeto más grande de la imagen
	std::vector<cv::Vec2i> posPix; // almacena las posiciones de los píxeles del objeto más grande.
	float InerciaX, InerciaY, InerciaXY, AngGiro; // Momento de inercia central (pasa por el centroide) del eje X (eje vertical descendente), 
	// del eje Y (eje horizontal hacia la derecha) y ángulo de rotación de los ejes para transformar en ejes principales de inercia.
	cv::Mat contorno; // Matriz que identificará el controno externo e interno del objeto principal de cada imagen
};