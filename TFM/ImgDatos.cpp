#include"ImgDatos.h"
#include<opencv2/core/core.hpp> // Librerías necesarias para usar OpenCV
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<assert.h>
#include<queue>

imgDatos::imgDatos(cv::Mat& imagenRef)
	:
	conect(imagenRef.size(), CV_8UC1, cv::Scalar(0)), // Los miembros de la clase usan su constructor aquí. Usarlo en el cuerpo
	contorno(imagenRef.size(), CV_8UC3, cv::Scalar({ 0,0,0 })) // crea la matriz de contornos del objeto más grande.
{
	/// FORMACIÓN DE OBJETOS Y SELECCIÓN DEL MAYOR
	for (int i = 0; i < imagenRef.rows; i++)
	{
		for (int j = 0; j < imagenRef.cols; j++)
		{
			if (imagenRef.at<unsigned char>(i, j) == 255 && conect.at<unsigned char>(i, j) != 255)
			{
				int tamObjtemp = 1;// Introduce un nuevo elemento al vector de tamaño de objetos
				cv::Vec2f centroidetemp = { i + 0.5f,j + 0.5f }; // Centroide del primer píxel del objeto

				std::queue<cv::Vec2i> conector; // el vector (vector 2D de enteros) que almacenará coordenadas de los píxeles a conectar
												/* El formato contenedor queue almacena colocando en cola (en última posición) los elementos entrantres y extrayendo la primera posición
												cada vez que se quita un elemento (FIFO, First In First Out, el primero en entrar el primero en salir). Otra clase de la librería estándar
												es stack que introduce elementos en primera posición y quita también la primera posición (LIFO, Last In First Out, el último en entrar el
												primero en salir) */
				conector.push({ i, j }); // introducimos el primer contenido detectado como último elemento (está vacío, se coloca el primero)
				conect.at<unsigned char>(i, j) = 255; // marcamos primer elemento como visto

				std::vector<cv::Vec2i> posPixTemp; // vector temporal de posiciones del objeto más grande
				posPixTemp.push_back({ i,j });

				while (!conector.empty())
				{	// Comprobación para que la iteración de búsqueda se haga siempre en los límites de la imagen
					int testfil1 = 1, testfil2 = 1, testcol1 = 1, testcol2 = 1;
					if (conector.front().val[0] == 0) testfil1 = 0;
					if (conector.front().val[0] == conect.rows) testfil1 = 0;
					if (conector.front().val[1] == 0) testcol1 = 0;
					if (conector.front().val[1] == conect.cols) testcol2 = 0;
					assert(conector.front().val[0] - testfil1 >= 0); // En el caso de que la resta sea un valor menor que cero, salta un error.
					assert(conector.front().val[1] - testcol1 >= 0); // En el caso de que la resta sea un valor menor que cero, salta un error.
					assert(conector.front().val[0] + testfil2 <= conect.rows); // En el caso de que la resta sea un valor mayor que el núm de filas, error.
					assert(conector.front().val[1] + testcol2 <= conect.cols); // En el caso de que la resta sea un valor mayor que el núm de colmns, error.

					for (int fil = conector.front().val[0] - testfil1; fil <= conector.front().val[0] + testfil2; fil++)
					{
						for (int col = conector.front().val[1] - testcol1; col <= conector.front().val[1] + testcol2; col++)
						{
							if (conect.at<unsigned char>(fil, col) != 255 && imagenRef.at<unsigned char>(fil, col) == 255)
							{// Si no está conectado y hay contenido: mételo en el vector queue
								conector.push({ fil,col });
								tamObjtemp++; // dale al último elemento del vector(valor utilizado) un píxel más de tamaño.
								centroidetemp.val[0] += fil + 0.5f;
								centroidetemp.val[1] += col + 0.5f;
								conect.at<unsigned char>(fil, col) = 255; // marcamos elemento como visto
								posPixTemp.push_back({ fil,col }); // cada píxel del objeto añade sus coordenadas al vector temporal
							}
						}
					}
					conector.pop(); // elimina el primer elemento
				}

				// Sin más puntos en el objeto, se divide entre el número de píxeles total cada componente del centroide
				centroidetemp.val[0] /= tamObjtemp;
				centroidetemp.val[1] /= tamObjtemp;

				// Se comprueba si el vector de posiciones temporal es mayor que el vector de posiciones del objeto más grande.
				if (posPixTemp.size() > posPix.size())
				{
					posPix.clear(); // Se borra el vector de la clase si es más pequeño
					centroide.val[0] = centroidetemp.val[0]; // Se cambian los valores del centroide al del objeto más grande
					centroide.val[1] = centroidetemp.val[1];
					tamObj = tamObjtemp; // Se cambia el tamaño del objeto más grande.
					for (int i = 0; i < posPixTemp.size(); i++)
					{
						posPix.push_back(posPixTemp[i]); // Se introducen todos los valores
					}
				}
			}
		}
	}
	
	/// MOMENTOS DE INERCIA CENTRALES (respecto del centroide)
	for (int i = 0; i < posPix.size(); i++)
	{
		InerciaX += (posPix[i].val[1] + 0.5f - centroide.val[1])*(posPix[i].val[1] + 0.5f - centroide.val[1]);
		InerciaY += (posPix[i].val[0] + 0.5f - centroide.val[0])*(posPix[i].val[0] + 0.5f - centroide.val[0]);
		InerciaXY += (posPix[i].val[0] + 0.5f - centroide.val[0])*(posPix[i].val[1] + 0.5f - centroide.val[1]);
	}

	/// ANGULO PARA EJES PRINCIPALES
	// El símbolo está configurado para que el giro de los ejes en sentido de las agujas del reloj siempre sea negativo y giro antihorario 
	// positivo. Se ha configurado para que el eje fila (eje X) coincida con la dirección principal del insecto.
	
	if (InerciaX > InerciaY)
	{
		AngGiro = (3.141593f) / 2.0f - std::atan((2.0f * InerciaXY) / (InerciaX - InerciaY)) / 2.0f; //ángulo en radianes
	}
	else
	{
		AngGiro = std::atan((2 * InerciaXY) / (InerciaY - InerciaX)) / 2; //ángulo en radianes
	}

	
	/// CONTORNO EXTERIOR E INTERIOR
	for (int k = 0; k < posPix.size(); k++) // bucle por cada posición del vector de coordenadas del insecto
	{
		int i = posPix[k].val[0]; // tomo posición de fila
		int j = posPix[k].val[1]; // tomo posición de columna
		contorno.at<cv::Vec3b>(i, j)[0] = 255; // Insecto en azul
		contorno.at<cv::Vec3b>(i, j)[1] = 0; // Le quitas el verde de contornos de otra iteración

		// Si alrededor hay espacio (no te sales de la imagen) y si no valen ya 255 las bandas de azul, dale el verde al contorno.
		if (i - 1 >= 0 && contorno.at<cv::Vec3b>(i - 1, j)[0] != 255) contorno.at<cv::Vec3b>(i - 1, j)[1] = 255;
		if (i + 1 <= contorno.rows && contorno.at<cv::Vec3b>(i + 1, j)[0] != 255) contorno.at<cv::Vec3b>(i + 1, j)[1] = 255;
		if (j - 1 >= 0 && contorno.at<cv::Vec3b>(i, j - 1)[0] != 255) contorno.at<cv::Vec3b>(i, j - 1)[1] = 255;
		if (j + 1 <= contorno.cols && contorno.at<cv::Vec3b>(i, j + 1)[0] != 255)contorno.at<cv::Vec3b>(i, j + 1)[1] = 255;
	}

	/// DIFERENCIAR CONTORNO EXTERNO DE INTERNO: ROJO EXTERNO, VERDE INTERNO, INSECTO AZUL
	std::queue<cv::Vec2i> vecRellenaCont;
	vecRellenaCont.push({ posPix[0].val[0] - 1,posPix[0].val[1] }); // se introduce la posición inicial del contorno
	contorno.at<cv::Vec3b>(posPix[0].val[0] - 1, posPix[0].val[1])[1] = 0; // le quito el verde
	contorno.at<cv::Vec3b>(posPix[0].val[0] - 1, posPix[0].val[1])[2] = 255; // cambiamos el color del contorno externo a rojo. Sirve para diferenciar.

	while (!vecRellenaCont.empty())
	{
		int testfil1 = 1, testfil2 = 1, testcol1 = 1, testcol2 = 1;
		if (vecRellenaCont.front().val[0] == 0) testfil1 = 0;
		if (vecRellenaCont.front().val[0] == imagenRef.rows) testfil1 = 0;
		if (vecRellenaCont.front().val[1] == 0) testcol1 = 0;
		if (vecRellenaCont.front().val[1] == imagenRef.cols) testcol2 = 0;
		assert(vecRellenaCont.front().val[0] - testfil1 >= 0); // En el caso de que la resta sea un valor menor que cero, salta un error.
		assert(vecRellenaCont.front().val[1] - testcol1 >= 0); // En el caso de que la resta sea un valor menor que cero, salta un error.
		assert(vecRellenaCont.front().val[0] + testfil2 <= imagenRef.rows); // En el caso de que la resta sea un valor mayor que el núm de filas, error.
		assert(vecRellenaCont.front().val[1] + testcol2 <= imagenRef.cols); // En el caso de que la resta sea un valor mayor que el núm de colmns, error.

		for (int fil = vecRellenaCont.front().val[0] - testfil1; fil <= vecRellenaCont.front().val[0] + testfil2; fil++)
		{
			for (int col = vecRellenaCont.front().val[1] - testcol1; col <= vecRellenaCont.front().val[1] + testcol2; col++)
			{
				if (contorno.at<cv::Vec3b>(fil, col)[1] == 255)
				{
					vecRellenaCont.push({ fil,col });
					contorno.at<cv::Vec3b>(fil, col)[1] = 0;
					contorno.at<cv::Vec3b>(fil, col)[2] = 255;
				}
			}
		}
		vecRellenaCont.pop();

	}
}



/*for (int fil = i - testfil1; fil <= i + testfil2; fil++)
{
	for (int col = j - testcol1; col <= j + testcol2; col++)
	{
		if ((*imagenUmbral).at<unsigned char>(fil, col) == 255 && conect.at<unsigned char>(fil, col) != 255)
			// Si la imagen del umbral tiene contenido y el punto no ha sido conectado
		{
			conect.at<unsigned char>(fil, col) = 255; // conecta el punto
			numObj.at<unsigned char>(fil, col) = contObj; // le pones el valor del objeto
			manchaAceite(fil, col, imagenUmbral); // y busca en su entorno
		}
	}
}


	// Comprobación de valores del umbral en el entorno de conexión.
	// Si encuentra un píxel del umbral en su entorno, el píxel no está aislado -> aislado devuelve false.
	// Si no encuentra un píxel del umbral en su entorno, el píxel está aislado -> aislado devuelve true.

	
	/*for (int col = j - testcol1; col <= j + testcol2; col++) // Comprobación fila superior.
	{
		if (conect.at<unsigned char>(i-testfil1, col) == 255 && testfil1==1) //Si hay pixel en el umbral arriba con comprobación de que
		{																	 //haya fila superior
			conect.at<unsigned char>(i, j) = 255; // conecta el píxel
			numObj.at<unsigned char>(i, j) = numObj.at<unsigned char>(i-testfil1, col); // y dale número de objeto que al de la fila anterior
		}
	}
	if (conect.at<unsigned char>(i, j) == 255) // Ya está conectado
	{	}
	
	// Si salta el bucle anterior sin resultado hay que comprobar la opción de que sea un píxel conectado por abajo o por los lados
	
	// Comprobación fila media. 
	if (conect.at<unsigned char>(i, j - testcol1) == 255 && testcol1==1) // Posición izquierda está conectada (y comprobación de columna
	{																	 // anterior existente
		conect.at<unsigned char>(i, j) = 255; // se conecta posición
		numObj.at<unsigned char>(i, j) = numObj.at<unsigned char>(i, j-testcol1); // y se añade el número de objeto del píxel
	}
	
	// Volvemos a comprobar conexión
	if (conect.at<unsigned char>(i, j) == 255) // Ya está conectado
	{	}

	// Ahora trabajamos en el centro-derecha y abajo. Son posiciones que pueden estar conectadas, pero que no han sido comprobadas en el bucle.
	// Hay que usar ahora la referencia a la imagen original para ver si estan en el umbral.
	
	// Este if vale para salir de la recursión del if que mira la imagen original. Cuando se llega a un píxel en la derecha 
	if (conect.at<unsigned char>(i, j + testcol2) == 255 && testcol2 == 1)
	{
		conect.at<unsigned char>(i, j) = 255;
		numObj.at<unsigned char>(i, j) = numObj.at<unsigned char>(i, j + testcol2);
		return false; // Salir de la función
	}
	
	if ((*imagentrabajada).at<unsigned char>(i, j + testcol2) == 255 && testcol2 == 1) // Posición derecha está en umbral (y comprobación de 												
	{																				   // columna
		int fila = i, columna = j + 1;
		manchaAceite(fila, columna, imagentrabajada); // Creo que desplaza hacia la derecha y hace comprobación de aislado
	}
	
	for (int fil = i - testfil1; fil <= i + testfil2; fil++)
	{
		for (int col = j - testcol1; col <= j + testcol2; col++)
		{
			if (col == j && fil == i) continue;
			if (conect.at<unsigned char>(fil, col)==255)
			return false;
		}
	}
	return true;
		
}

/*if (*j == 0) //Borde izquierdo
{
	if ((*imagenRef).at<unsigned char>(*(i + 1), *(j + 1)) == 255)
	{
	}
}
else if (*j == 0 && *i == conect.rows) // Esquina inferior izquierda
{

}
else if (*i == conect.rows) // Borde inferior
{

}
else if (*i == conect.rows && *j == conect.cols) // Esquina inferior derecha
{

}
else if (*j == conect.cols) // Borde derecho
{

}
else // Puntos dentro de la imagen
{

}*/