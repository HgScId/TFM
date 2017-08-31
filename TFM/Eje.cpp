#include "Eje.h"

#define PI 3.141592653589793238463


Eje::Eje(cv::Mat& imagenRef)
	:
	mat(imagenRef.size(), CV_8UC1, cv::Scalar(0)) // Los miembros de la clase usan su constructor aquí. Usarlo en el cuerpo
{
}


void Eje::DibujaEje(Eje& eje, cv::Vec2d& centroide, double angulo)
{	
	if (std::tan(angulo) > 0) // recta creciente (decreciente desde el punto de vista, la fila 0 corta por la izquierda)
	{
		int ValColIni = int(std::tan(angulo)*(0 - centroide.val[0]) + centroide.val[1]); // Comprobación de pos columna en fila 0.
		if (ValColIni < 0) ValColIni = 0; // si en fila 0 la columna se sale por la izq, la primera pos columna será 0.

		// If para evitar problema de fallos en el redondeo en el cálculo de la pos fila para ValColIni. Cuando la primera fila es 0 puede
		// quedar un entero negativo.
		if (int((ValColIni - centroide.val[1]) / std::tan(angulo) + centroide.val[0]) < 0) // si es menor que 0
		{
			eje.mat.at<unsigned char>(0, ValColIni) = 255;
			pos.push_back({ 0, ValColIni });
		}
		else // si el entero redondeado es positivo o 0, dale su propio valor.
		{
			eje.mat.at<unsigned char>(int((ValColIni - centroide.val[1]) / std::tan(angulo) + centroide.val[0]), ValColIni) = 255;
			pos.push_back({ int((ValColIni - centroide.val[1]) / std::tan(angulo) + centroide.val[0]), ValColIni });
		}

		// Análisis por filas de la imagen
		for (int i = 0; i < eje.mat.rows; i++ )
		{
			int ValCol = int(std::tan(angulo)*(i - centroide.val[0]) + centroide.val[1]); // Valor columna para cada fila de la imagen
			
			if (ValCol >= 0) // Si el valor es mayor que 0 (está dentro de la imagen por el límite izquierdo)
			{
				if (ValCol - ValColIni > 1) // Comprobación de los saltos en el dibujo de los puntos (posibles saltos de múltiples columnas)
				{
					for (int j = 1; j < ValCol - ValColIni; j++)
					{
						if (ValColIni + j < eje.mat.cols) // Comprobación de que ValColIni + j no pueda salirse de la imagen por la derecha
						{	// Introduce los valores en la matriz y en el vector de posiciones de la recta
							eje.mat.at<unsigned char>(int((ValColIni + j - centroide.val[1]) / std::tan(angulo) + centroide.val[0]), ValColIni + j) = 255;
							pos.push_back({ int((ValColIni + j - centroide.val[1]) / std::tan(angulo) + centroide.val[0]), ValColIni + j });
						}
					}
				}
				
				if (ValCol < eje.mat.cols) 
				{	// Si el ValCol no se sale por la derecha (estamos también dentro de un if que evita la salida por la izquierda)
					eje.mat.at<unsigned char>(i, ValCol) = 255;
					// Se dan casos en los que el ValColIni se introduce en el vector de posiciones de la recta y el primer ValCol calculado
					// es equivalente a ValColIni. Este if comprueba que el último valor no se repita antes de añadir coordenadas al vector.
					if (ValCol != pos.back().val[1] || i != pos.back().val[0]) pos.push_back({ i, ValCol }); 
					ValColIni = ValCol; // el ValColIni toma el ValCol para comprobar los siguientes saltos de columna.
				}
				else // En el caso de que el ValCol sobrepase la derecha, rompe el bucle que analiza la imagen por filas.
				{
					break;
				}
			}		
		}
	}
	else if (std::tan(angulo) < 0) // recta decreciente (creciente desde el punto de vista, la fila 0 corta por la derecha)
	{
		int ValColIni = int(std::tan(angulo)*(0 - centroide.val[0]) + centroide.val[1]);
		if (ValColIni >= eje.mat.cols) ValColIni = eje.mat.cols - 1;
		
		if (int((ValColIni - centroide.val[1]) / std::tan(angulo) + centroide.val[0]) < 0)
		{
			eje.mat.at<unsigned char>(0, ValColIni) = 255;
			pos.push_back({ 0, ValColIni });
		}
		else
		{
			eje.mat.at<unsigned char>(int((ValColIni - centroide.val[1]) / std::tan(angulo) + centroide.val[0]), ValColIni) = 255;
			pos.push_back({ int((ValColIni - centroide.val[1]) / std::tan(angulo) + centroide.val[0]), ValColIni });
		}

		for (int i = 0; i < eje.mat.rows; i++)
		{
			int ValCol = int(std::tan(angulo)*(i - centroide.val[0]) + centroide.val[1]);
			if (ValCol < eje.mat.cols)
			{
				if (ValColIni - ValCol> 1)
				{
					for (int j = 1; j < ValColIni-ValCol; j++)
					{
						if (ValColIni - j >=0)
						{
							eje.mat.at<unsigned char>(int((ValColIni - j - centroide.val[1]) / std::tan(angulo) + centroide.val[0]), ValColIni - j) = 255;
							pos.push_back({ int((ValColIni - j - centroide.val[1]) / std::tan(angulo) + centroide.val[0]), ValColIni - j });
						}
					}
				}
				if (ValCol >=0)
				{
					eje.mat.at<unsigned char>(i, ValCol) = 255;
					if(ValCol!=pos.back().val[1] || i!= pos.back().val[0]) pos.push_back({ i, ValCol });
					ValColIni = ValCol;
				}
				else
				{
					break;
				}
			}
		}
	}
	else
	{
		assert(1 == 0); // Caso raro tangente 0, el eje de inercia principal X es una recta vertical completamente.
	}
}

void Eje::DibujaEjeSec(Eje& eje, cv::Vec2d& centroide, double angulo)
{
	DibujaEje(eje, centroide, angulo + PI/2);
}

double Eje::Intersecc(Eje& eje, cv::Mat& contorno)
{
	std::vector<cv::Vec2i> intersecc;

	for (int i = 0; i < eje.mat.rows; i++)
	{
		for (int j = 0; j < eje.mat.cols; j++)
		{
			// Comprobación para que no se pueda salir de la imagen el chequeo de columna.
			int testcol1 = 1, testcol2 = 1;
			if (j - testcol1 < 0) testcol1 = 0;
			if (j + testcol2 >= eje.mat.cols) testcol2 = 0;
			if (contorno.at<cv::Vec3b>(i, j)[2] == 255 && eje.mat.at<unsigned char>(i, j) == 255 ||
				contorno.at<cv::Vec3b>(i, j + testcol2)[2] == 255 && eje.mat.at<unsigned char>(i, j) == 255 || //Estas dos comprobaciones son para 
				contorno.at<cv::Vec3b>(i, j - testcol1)[2] == 255 && eje.mat.at<unsigned char>(i, j) == 255) //casos en los que se cruzan. Ejemplo imagen 3
			{
				if (intersecc.size() >= 2)	intersecc.pop_back();
				intersecc.push_back({ i,j });
			}
		}
	}
	double dist = std::sqrt(std::pow((intersecc[0].val[0] - intersecc[1].val[0]), 2) + std::pow((intersecc[0].val[1] - intersecc[1].val[1] - 2), 2));
	// Ese -2 sirve para eliminar las dos unidades columnas añadidas en cada medición para evitar los cruces de líneas.
	return dist;
}

cv::Vec2d Eje::InterseccSec(const cv::Vec2f & centroide, const double angulo, const double longPrin)
{
	double angTrans = angulo + 3.141593f / 2.0f; // ángulo de la recta del eje transversal.
	cv::Vec2d longitudes = { 0.0,0.0 };


	return longitudes;
}

