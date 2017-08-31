#pragma once
#include<opencv2/core/core.hpp> // Librerías necesarias para usar OpenCV
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

class Eje
{/* Esta clase recoge todo el trabajo con los ejes de inercia y rectas sobre una imagen. Se construye mediante las dimensiones de la imagen
 que se va a trabajar. Para dibujer el eje principal de inercia utiliza las funciones DibujaEje o por el contrario si el objeto de la clase
 Eje es el eje secundario de inercia, se dibuja con DibujaEjeSec. Si el objeto representa ambos ejes, se utilizan ambas funciones de la
 clase consecutivamente.*/

public:
	Eje(cv::Mat& imagenRef);

public:
	cv::Mat mat; // Matriz que almacena el/los ejes que se dibujen.
	std::vector<cv::Vec2i> pos; // Vector de coordenadas de los ejes dibujados.

public:
	
	/// DIBUJA EJE PRINCIPAL DE INERCIA Y LO DEVUELVE COMO IMAGEN
	void DibujaEje(Eje& eje, cv::Vec2d& centroide, double angulo);

	/// DIBUJA EJE SECUNDARIO
	void DibujaEjeSec(Eje& eje, cv::Vec2d& centroide, double angulo);

	/// INTERSECCIÓN EJES - CONTORNO
	double Intersecc(Eje& eje, cv::Mat& contorno); // Intersección eje longitudinal
	cv::Vec2d InterseccSec(const cv::Vec2f& centroide, const double angulo, const double longPrin); // Intersección transversal
};