#include<opencv2/core/core.hpp> // Librerías necesarias para usar OpenCV
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

struct GeometriaObj
{
	cv::Vec2d centroide; // Se trata de una pareja de floats que localizan la posición del centroide del objeto más grande de la imagen
	double InerciaX, InerciaY, InerciaXY; // Momento de inercia central (pasa por el centroide) del eje X (eje vertical descendente), 
										  // del eje Y (eje horizontal hacia la derecha)
	double AngGiro; // Ángulo de rotación de los ejes para transformar en ejes principales de inercia.
	double factforma; // factor de forma = kIdeal* Área objeto / Perímetro^2 objeto
	double kIdeal; // Kideal hace factor de forma =1 para la elipse equivalente.
};