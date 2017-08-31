#pragma once
#include<opencv2/core/core.hpp> // Librerías necesarias para usar OpenCV
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include"EstructurasUsadas.h"


// Esta clase va a almacenar datos de la imagen utilizada en una matriz. Se construye la clase con un constructor que utiliza un imagen.
class imgDatos
{
public:
	imgDatos(cv::Mat& imagenRef, bool comp);
	imgDatos(cv::Mat& imagenRef);
	/* Dentro del constructor de la clase se añaden todos los procedimientos para la construcción de los miembros de la clase: se conectan
	los objetos de la imagen umbralizada, se cuentan el número de píxeles del objeto más grande, su centroide, el valor de todos sus píxeles,
	sus momentos de inercia y el ángulo de giro para ejes principales y su contorno.*/
public:
	// Estos miembros de la clase han de ser inicilizados con datos de la matriz imagenRef que se le pasa al constructor. 
	// En el .cpp se inicializan
	cv::Mat conect; // Matriz que refleja el valor 0 si es un píxel fuera del umbral de Otsu y 255 si es un píxel seleccionado. // ESTA MATRIZ INTRODUCIR COMO VARIABLE LOCAL EN CONTRUCTOR
	int tamObj; // identifica el tamaño en píxeles del objeto más grande (equivalente al área del objeto)
	int perObj; // Perímetro del objeto. Número de lados de píxeles contorno que están en contacto con píxeles objeto.
	std::vector<cv::Vec2i> posPix; // almacena las posiciones de los píxeles del objeto más grande. NO CONTIENE LAS POSICONES DEL CONTORNO DE LA FIGURA.
	std::vector<cv::Vec2i> posPixcontornoext; // almacena el contorno exterior de la figura más grande
	cv::Mat contorno; // Matriz que identificará el controno externo e interno del objeto principal de cada imagen
	GeometriaObj geometriaImagen; // La estructura GeometriaObj alberga Momentos de Inercia, valor del centroide, ángulo de giro para la obtención de ejes
	//principales y el k para elipse ideal y su factor de forma (excentricidad de la figura y elipse equivalente). Para obtener esta estructura se utiliza la
	//función CalculaMomentos con un vector de píxeles del objeto de entrada junto con su perímetro.
public:
	void perfilamiento(cv::Mat& imagencontorno, std::vector<cv::Vec2i>& posPixentrada);
private:
	GeometriaObj CalculaMomentos(std::vector<cv::Vec2i> posPixentrada, int perObjentrada);
};