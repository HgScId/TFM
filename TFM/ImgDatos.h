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
	std::vector<cv::Vec2f> centroide; // Se trata de un vector de dos componenetes de floats que va a detectar el centroide de 
	/*cada objeto. Cada píxel es un cuadrado de coordenadas conocidas. El píxel (0,0) se identifica a través de su esquina superior 
	izquierda. Las coordenadas del centroide serán (0.5f,0.5f). El tamaño unitario del píxel hace que el problema se reduzca a 
	hallar la suma de todos los centros de gravedad de los píxeles (componente X e Y por separado) y dividir entre el número de 
	píxeles (área total del objeto).*/
	int contObj=0;
	std::vector<cv::Vec2i> posPix; // almacena las posiciones de los píxeles del objeto más grande (insecto)
	float InerciaX, InerciaY, InerciaXY, AngGiro; // Momento de inercia central (pasa por el centroide) del eje X (eje vertical descendente), 
	// del eje Y (eje horizontal hacia la derecha) y ángulo de rotación de los ejes para transformar en ejes principales de inercia.
	cv::Mat contorno; // Matriz que identificará el controno externo e interno del objeto principal de cada imagen

public:
	void formaObjetos(int i, int j, cv::Mat* imagenUmbral);

};