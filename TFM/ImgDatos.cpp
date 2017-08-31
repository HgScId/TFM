#include"ImgDatos.h"
#include<opencv2/core/core.hpp> // Librerías necesarias para usar OpenCV
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
#include<assert.h>
#include<queue>
#include"EstructurasUsadas.h"

#define PI 3.141592653589793238463

imgDatos::imgDatos(cv::Mat& imagenRef, bool comp)
	:
	conect(imagenRef.size(), CV_8UC1, cv::Scalar(0)), // Los miembros de la clase usan su constructor aquí. Usarlo en el cuerpo
	contorno(imagenRef.size(), CV_8UC3, cv::Scalar({ 0,0,0 })) // crea la matriz de contornos del objeto más grande.
{
}

imgDatos::imgDatos(cv::Mat& imagenRef)
	:
	conect(imagenRef.size(), CV_8UC1, cv::Scalar(0)), // Los miembros de la clase usan su constructor aquí. Usarlo en el cuerpo
	contorno(imagenRef.size(), CV_8UC3, cv::Scalar({ 0,0,0 })) // crea la matriz de contornos del objeto más grande.
{

	/// FORMACIÓN DE OBJETOS Y SELECCIÓN DEL MAYOR (INSECTO)
	for (int i = 0; i < imagenRef.rows; i++)
	{
		for (int j = 0; j < imagenRef.cols; j++)
		{
			if (imagenRef.at<unsigned char>(i, j) == 255 && conect.at<unsigned char>(i, j) != 255)
			{
				int tamObjtemp = 1;// Introduce un nuevo elemento al vector de tamaño de objetos

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
								conect.at<unsigned char>(fil, col) = 255; // marcamos elemento como visto
								posPixTemp.push_back({ fil,col }); // cada píxel del objeto añade sus coordenadas al vector temporal
							}
						}
					}
					conector.pop(); // elimina el primer elemento
				}

				// Se comprueba si el vector de posiciones temporal es mayor que el vector de posiciones del objeto más grande.
				if (posPixTemp.size() > posPix.size())
				{
					posPix.clear(); // Se borra el vector de la clase si es más pequeño
					tamObj = tamObjtemp; // Se cambia el tamaño del objeto más grande.
					for (int i = 0; i < posPixTemp.size(); i++)
					{
						posPix.push_back(posPixTemp[i]); // Se introducen todos los valores
					}
				}
			}
		}
	}
	
	/// CONTORNO EXTERIOR E INTERIOR
	for (int k = 0; k < posPix.size(); k++) // bucle por cada posición del vector de coordenadas del insecto
	{
		int i = posPix[k].val[0]; // tomo posición de fila
		int j = posPix[k].val[1]; // tomo posición de columna
		contorno.at<cv::Vec3b>(i, j)[0] = 255; // Insecto en azul
		contorno.at<cv::Vec3b>(i, j)[1] = 0; // Le quitas el verde de contornos de otra iteración

		// Si alrededor hay espacio (no te sales de la imagen) y si no valen ya 255 las bandas de azul, dale el verde al contorno.
		if (i - 1 >= 0 && contorno.at<cv::Vec3b>(i - 1, j)[0] != 255)
		{
			contorno.at<cv::Vec3b>(i - 1, j)[1] = 255;
		}
		if (i + 1 <= contorno.rows && contorno.at<cv::Vec3b>(i + 1, j)[0] != 255)
		{
			contorno.at<cv::Vec3b>(i + 1, j)[1] = 255;
		}

		if (j - 1 >= 0 && contorno.at<cv::Vec3b>(i, j - 1)[0] != 255) 
		{
			contorno.at<cv::Vec3b>(i, j - 1)[1] = 255;
		}
		if (j + 1 <= contorno.cols && contorno.at<cv::Vec3b>(i, j + 1)[0] != 255)
		{
			contorno.at<cv::Vec3b>(i, j + 1)[1] = 255;
		}
	}

	/// DIFERENCIAR CONTORNO EXTERNO DE INTERNO: ROJO EXTERNO, VERDE INTERNO, INSECTO AZUL
	std::queue<cv::Vec2i> vecRellenaCont;
	vecRellenaCont.push({ posPix[0].val[0] - 1,posPix[0].val[1] }); // se introduce la posición inicial del contorno
	posPixcontornoext.push_back({ posPix[0].val[0] - 1,posPix[0].val[1] });
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
					posPixcontornoext.push_back({ fil, col });

				}
			}
		}
		vecRellenaCont.pop();
	}

	/// PERÍMETRO DEL OBJETO

	for (int i = 0; i < contorno.rows; i++)
	{
		for (int j = 0; j < contorno.cols; j++)
		{
			if (contorno.at<cv::Vec3b>(i, j)[2] == 255)
			{	// Comprobación para evitar salidas de la imagen
				int testfil1 = 1, testfil2 = 1, testcol1 = 1, testcol2 = 1;
				if (i - testfil1 < 0) testfil1 = 0;
				if (j - testcol1 < 0) testcol1 = 0;
				if (i + testfil2 >= contorno.rows) testfil2 = 0;
				if (j + testcol2 >= contorno.cols) testcol2 = 0;

				// En el caso de que cualquiera de los puntos del test salga 0, se buscará un punto insecto (banda del azul) 
				// en la posición (i,j) que es contorno (banda del rojo) por lo que no sumará al perímetro
				if (contorno.at<cv::Vec3b>(i - testfil1, j)[0] == 255) perObj++;
				if (contorno.at<cv::Vec3b>(i + testfil2, j)[0] == 255) perObj++;
				if (contorno.at<cv::Vec3b>(i, j - testcol1)[0] == 255) perObj++;
				if (contorno.at<cv::Vec3b>(i, j + testcol2)[0] == 255) perObj++;
			}
		}
	}

	/// CALCULO MOMENTOS INERCIA, EJES DE INERCIA, EXCENTRICIDAD, ELIPSE EQUIVALENTE
	geometriaImagen =CalculaMomentos(posPix,perObj);


	
}

void imgDatos::perfilamiento(cv::Mat& imagencontornoentrada, std::vector<cv::Vec2i>& posPixentrada)
{
	int Areacuad = 100, Percuad = 40;
	double kcuad = double(Percuad*Percuad) / double(Areacuad);
	//int iteraciones = 0;

	// Localizar la forma para minimizar iteraciones. Encuadrar el insecto.
	int filmin= imagencontornoentrada.rows, colmin= imagencontornoentrada.cols, filmax=0, colmax=0;
	for (int i = 0; i < posPixentrada.size(); i++)
	{
		filmin = std::min(filmin, posPixentrada[i].val[0]);
		colmin = std::min(colmin, posPixentrada[i].val[1]);
		filmax = std::max(filmax, posPixentrada[i].val[0]);
		colmax = std::max(colmax, posPixentrada[i].val[1]);
	}

	for (int i = 0; i <= int((filmax-filmin) / 10); i++)
	{
		for (int j = 0; j <= int((colmax-colmin) / 10); j++)
		{
			imgDatos segmento(imagencontornoentrada, true); // trozo extraído de la figura
			
			for (int fil = filmin + i * 10; fil < filmin+10 + i * 10; fil++)
			{
				for (int col = colmin + j * 10; col < colmin+10 + j * 10; col++)
				{
					if (imagencontornoentrada.at<cv::Vec3b>(fil, col)[0] == 255)
					{
						segmento.contorno.at<cv::Vec3b>(fil, col)[0] = 255; // dibuja el insecto
						segmento.tamObj++; // contar tamaño del segmento
						segmento.posPix.push_back({ fil,col }); // almacena los puntos del segmento
					}
				}
			}	

			// Se cierra toda la figura azul con contornos rojos
			if (segmento.tamObj > 0) // Si hay objeto en el extracto, opera!
			{
				//iteraciones++;
				//cv::imwrite("ImagenSalida\\"+std::to_string(iteraciones)+"cosa1.bmp", segmento.contorno); // segmento azul extraído
				for (int l = 0; l < segmento.posPix.size(); l++)
				{
					int testfil1 = 1, testfil2 = 1, testcol1 = 1, testcol2 = 1;
					if (segmento.posPix[l].val[0] - testfil1 < 0) testfil1 = 0;
					if (segmento.posPix[l].val[1] - testcol1 < 0) testcol1 = 0;
					if (segmento.posPix[l].val[0] + testfil2 >= segmento.contorno.rows) testfil2 = 0;
					if (segmento.posPix[l].val[1] + testcol2 >= segmento.contorno.cols) testcol2 = 0;

					if (segmento.contorno.at<cv::Vec3b>(segmento.posPix[l].val[0], segmento.posPix[l].val[1])[0] == 255)
					{
						if (segmento.contorno.at<cv::Vec3b>(segmento.posPix[l].val[0] - testfil1, segmento.posPix[l].val[1])[2] == 0
							&& segmento.contorno.at<cv::Vec3b>(segmento.posPix[l].val[0] - testfil1, segmento.posPix[l].val[1])[0] == 0)
						{
							segmento.posPixcontornoext.push_back({ segmento.posPix[l].val[0] - testfil1, segmento.posPix[l].val[1] });
							segmento.contorno.at<cv::Vec3b>(segmento.posPix[l].val[0] - testfil1, segmento.posPix[l].val[1])[2] = 255;
							segmento.perObj++;
						}
						if (segmento.contorno.at<cv::Vec3b>(segmento.posPix[l].val[0] + testfil2, segmento.posPix[l].val[1])[2] == 0
							&& segmento.contorno.at<cv::Vec3b>(segmento.posPix[l].val[0] + testfil2, segmento.posPix[l].val[1])[0] == 0)
						{
							segmento.posPixcontornoext.push_back({ segmento.posPix[l].val[0] + testfil2, segmento.posPix[l].val[1] });
							segmento.contorno.at<cv::Vec3b>(segmento.posPix[l].val[0] + testfil2, segmento.posPix[l].val[1])[2] = 255;
							segmento.perObj++;
						}
						if (segmento.contorno.at<cv::Vec3b>(segmento.posPix[l].val[0], segmento.posPix[l].val[1] - testcol1)[2] == 0
							&& segmento.contorno.at<cv::Vec3b>(segmento.posPix[l].val[0], segmento.posPix[l].val[1] - testcol1)[0] == 0)
						{
							segmento.posPixcontornoext.push_back({ segmento.posPix[l].val[0] , segmento.posPix[l].val[1] - testcol1 });
							segmento.contorno.at<cv::Vec3b>(segmento.posPix[l].val[0], segmento.posPix[l].val[1] - testcol1)[2] = 255;
							segmento.perObj++;
						}
						if (segmento.contorno.at<cv::Vec3b>(segmento.posPix[l].val[0], segmento.posPix[l].val[1] + testcol2)[2] == 0
							&& segmento.contorno.at<cv::Vec3b>(segmento.posPix[l].val[0], segmento.posPix[l].val[1] + testcol2)[0] == 0)
						{
							segmento.posPixcontornoext.push_back({ segmento.posPix[l].val[0], segmento.posPix[l].val[1] + testcol2 });
							segmento.contorno.at<cv::Vec3b>(segmento.posPix[l].val[0], segmento.posPix[l].val[1] + testcol2)[2] = 255;
							segmento.perObj++;
						}
					}

				}
				double ffseg = double(kcuad*segmento.tamObj) / double(segmento.perObj*segmento.perObj);
				//cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa2.bmp", segmento.contorno);

				if (segmento.tamObj != 100)
				{
					cv::Mat contornoTemp=imagencontornoentrada.clone();
					std::vector<cv::Vec2i> posPixTemp = posPixentrada;
					std::vector<cv::Vec2i> posPixexteriorTemp = posPixcontornoext;
					int perObjTemp = perObj;
					int tamObjTemp = tamObj;
					
					// Necesitamos el nuevo perimetro
					// Borrado de azul del contorno temporal
					for (int l = 0; l < segmento.posPix.size(); l++)
					{
						if (contornoTemp.at<cv::Vec3b>(segmento.posPix[l].val[0], segmento.posPix[l].val[1])[0] == 255)
						{
							contornoTemp.at<cv::Vec3b>(segmento.posPix[l].val[0], segmento.posPix[l].val[1])[0] = 0;
							tamObjTemp--;
						}
					}
					//cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa3.bmp", contornoTemp); // azul borrado de contorno Temporal

					// Borrado de píxeles insecto azules del vector de almacenamiento temporal.
					for (int k = 0; k < segmento.posPix.size(); k++)
					{
						for (int p = int(posPixTemp.size()) - 1; p >= 0; p--) // se empieza desde el final de posPixTemp para que no haya problema al borrar dos elementos consecutivos
						{
							if (posPixTemp[p].val[0] == segmento.posPix[k].val[0] && posPixTemp[p].val[1] == segmento.posPix[k].val[1])
							{
								posPixTemp.erase(posPixTemp.begin() + p);
								break;
							}
						}
					}

					// Borrado de rojos aislados
					for (int l = 0; l < posPixcontornoext.size(); l++)
					{
						int testfil1 = 1, testfil2 = 1, testcol1 = 1, testcol2 = 1;
						if (posPixcontornoext[l].val[0] - testfil1 < 0) testfil1 = 0;
						if (posPixcontornoext[l].val[1] - testcol1 < 0) testcol1 = 0;
						if (posPixcontornoext[l].val[0] + testfil2 >= contornoTemp.rows) testfil2 = 0;
						if (posPixcontornoext[l].val[1] + testcol2 >= contornoTemp.cols) testcol2 = 0;
						
						if (contornoTemp.at<cv::Vec3b>(posPixcontornoext[l].val[0] - testfil1, posPixcontornoext[l].val[1])[0] == 0
							&& contornoTemp.at<cv::Vec3b>(posPixcontornoext[l].val[0] + testfil2, posPixcontornoext[l].val[1])[0] == 0
							&& contornoTemp.at<cv::Vec3b>(posPixcontornoext[l].val[0], posPixcontornoext[l].val[1] - testcol1)[0] == 0
							&& contornoTemp.at<cv::Vec3b>(posPixcontornoext[l].val[0], posPixcontornoext[l].val[1] + testcol2)[0] == 0)
						{
							contornoTemp.at<cv::Vec3b>(posPixcontornoext[l].val[0], posPixcontornoext[l].val[1])[2] = 0;
							perObjTemp--;
						}
						
					}
					//cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa4.bmp", contornoTemp); // borrado de rojos aislados
					
					// Borrado de píxeles contorno rojo del vector de almacenamiento temporal.
					for (int k = 0; k < segmento.posPixcontornoext.size(); k++)
					{
						for (int p = static_cast<int>(posPixexteriorTemp.size()) - 1; p >= 0; p--) // se empieza desde el final de posPixTemp para que no haya problema al borrar dos elementos consecutivos
						{
							if (posPixexteriorTemp[p].val[0] == segmento.posPixcontornoext[k].val[0] && posPixexteriorTemp[p].val[1] == segmento.posPixcontornoext[k].val[1])
							{
								int testfil1 = 1, testfil2 = 1, testcol1 = 1, testcol2 = 1;
								if (posPixexteriorTemp[p].val[0] - testfil1 < 0) testfil1 = 0;
								if (posPixexteriorTemp[p].val[1] - testcol1 < 0) testcol1 = 0;
								if (posPixexteriorTemp[p].val[0] + testfil2 >= contornoTemp.rows) testfil2 = 0;
								if (posPixexteriorTemp[p].val[1] + testcol2 >= contornoTemp.cols) testcol2 = 0;
								
								if (contornoTemp.at<cv::Vec3b>(posPixexteriorTemp[p].val[0] - testfil1, posPixexteriorTemp[p].val[1])[0] != 255
									&& contornoTemp.at<cv::Vec3b>(posPixexteriorTemp[p].val[0] + testfil2, posPixexteriorTemp[p].val[1])[0] != 255
									&& contornoTemp.at<cv::Vec3b>(posPixexteriorTemp[p].val[0], posPixexteriorTemp[p].val[1] - testcol1)[0] != 255
									&& contornoTemp.at<cv::Vec3b>(posPixexteriorTemp[p].val[0], posPixexteriorTemp[p].val[1] + testcol2)[0] != 255)
								{
									posPixexteriorTemp.erase(posPixexteriorTemp.begin() + p);
									break;
								}
							}
						}
					}


					// Colocación de rojo en azules exteriores
					for (int l = 0; l < posPixTemp.size(); l++)
					{
						int testfil1 = 1, testfil2 = 1, testcol1 = 1, testcol2 = 1;
						if (posPixTemp[l].val[0] - testfil1 < 0) testfil1 = 0;
						if (posPixTemp[l].val[1] - testcol1 < 0) testcol1 = 0;
						if (posPixTemp[l].val[0] + testfil2 >= contornoTemp.rows) testfil2 = 0;
						if (posPixTemp[l].val[1] + testcol2 >= contornoTemp.cols) testcol2 = 0;

						if (contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0] - testfil1, posPixTemp[l].val[1])[0] == 0
							&& contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0] - testfil1, posPixTemp[l].val[1])[2] == 0
							&& contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0] - testfil1, posPixTemp[l].val[1])[1] == 0)
						{
							contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0] - testfil1, posPixTemp[l].val[1])[2] = 255;
							perObjTemp++;
							posPixexteriorTemp.push_back({ posPixTemp[l].val[0] - testfil1, posPixTemp[l].val[1] });
						}
						if (contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0] + testfil2, posPixTemp[l].val[1])[0] == 0
							&& contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0] + testfil2, posPixTemp[l].val[1])[2] == 0
							&& contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0] + testfil2, posPixTemp[l].val[1])[1] == 0)
						{
							contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0] + testfil2, posPixTemp[l].val[1])[2] = 255;
							perObjTemp++;
							posPixexteriorTemp.push_back({ posPixTemp[l].val[0] + testfil2, posPixTemp[l].val[1] });
						}
						if (contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0], posPixTemp[l].val[1] - testcol1)[0] == 0
							&& contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0], posPixTemp[l].val[1] - testcol1)[2] == 0
							&& contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0], posPixTemp[l].val[1] - testcol1)[1] == 0)
						{
							contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0], posPixTemp[l].val[1] - testcol1)[2] = 255;
							perObjTemp++;
							posPixexteriorTemp.push_back({ posPixTemp[l].val[0], posPixTemp[l].val[1] - testcol1 });
						}
						if (contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0], posPixTemp[l].val[1] + testcol2)[0] == 0
							&& contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0], posPixTemp[l].val[1] + testcol2)[2] == 0
							&& contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0], posPixTemp[l].val[1] + testcol2)[1] == 0)
						{
							contornoTemp.at<cv::Vec3b>(posPixTemp[l].val[0], posPixTemp[l].val[1] + testcol2)[2] = 255;
							perObjTemp++;
							posPixexteriorTemp.push_back({ posPixTemp[l].val[0], posPixTemp[l].val[1] + testcol2 });
						}
					}
					//cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa5.bmp", contornoTemp); // nuevos rojos exteriores colocados

					GeometriaObj nuevaGeo = CalculaMomentos(posPixTemp, perObjTemp);
					if (nuevaGeo.factformaelipse > geometriaImagen.factformaelipse)
					{
						//cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa6.bmp", imagencontornoentrada);

						contornoTemp.copyTo(imagencontornoentrada);
						posPixcontornoext = posPixexteriorTemp;
						posPixentrada = posPixTemp;
						tamObj = tamObjTemp;
						geometriaImagen=nuevaGeo;
						perObj=perObjTemp;

						//cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa7.bmp", imagencontornoentrada); // borrado de rojos aislados
						//cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa8.bmp", contorno); // borrado de rojos aislados
					}
					else
					{
						//cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa6.bmp", imagencontornoentrada);

						imagencontornoentrada.copyTo(contornoTemp);
						posPixexteriorTemp = posPixcontornoext;
						posPixTemp = posPixentrada;
						tamObjTemp= tamObj;
						nuevaGeo= geometriaImagen;
						perObjTemp = perObj;

						//cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa7.bmp", imagencontornoentrada); // borrado de rojos aislados
						//cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa8.bmp", contorno); // borrado de rojos aislados
					}

					
				}

			}
			






		}
	}

}
	
	
	
	

/*	
	/// ALGORITMO DE COMPROBACIÓN DE FORMA
	imgDatos segmento(imagencontorno, true); // trozo extraído de la figura
	std::queue<cv::Vec2i> conector; // Vector cola de la extensión
	std::vector<cv::Vec2i> almacen; // almacén de píxeles del segmento extraído

	cv::Mat contornoTemp(imagencontorno); // copia temporal de la imagencontorno original.
	int tamObjTemp = tamObj;
	int perObjTemp = perObj;
	std::vector<cv::Vec2i> posPixTemp=posPix;
	int iteraciones = 0;
	
	// Caracterización de una expansión cuadrada libre por la figura
	int n = 7;
	int Percuad = 4 * (n + 1); // perímetro ideal de 36 para figura cuadrada con n=9, restriccion izquierda
	int Areacuad = (n + 1)*(n + 1);// 81 píxeles de superficie que se alcanzará
	double kcuadrado = double(Percuad*Percuad) / double(Areacuad); // k=16 para cualquier valor de n en cuadrado
	
	
	for (int i = 0; i < imagencontorno.rows; i++)
	{
		for (int j = 0; j < imagencontorno.cols; j++)
		{
			if (imagencontorno.at<cv::Vec3b>(i, j)[0] == 255 && segmento.conect.at<unsigned char>(i, j) != 255)
			{
				conector.push({ i,j });
				almacen.push_back({ i,j });
				segmento.conect.at<unsigned char>(i, j) = 255;
				segmento.contorno.at<cv::Vec3b>(i, j)[0] = 255;
				segmento.tamObj = 1; // tamaño de objeto en comprobación mide los píxeles de insecto analizados
				
				while (segmento.tamObj<Areacuad) // mientras esta comprobación sea menor que el área de 169 píxeles para n=12 iteraciones
				{
					int testfil1 = 1, testfil2 = 1, testcol1 = 1, testcol2 = 1;
					if (i - testfil1 < 0) testfil1 = 0;
					if (j - testcol1 < 0) testcol1 = 0;
					if (i + testfil2 >= contornoTemp.rows) testfil2 = 0;
					if (j + testcol2 >= contornoTemp.cols) testcol2 = 0;

					for (int fil = conector.front().val[0] - testfil1; fil <= conector.front().val[0] + testfil2; fil++)
					{
						for (int col = conector.front().val[1] - testcol1; col <= conector.front().val[1] + testcol2; col++)
						{
							if (imagencontorno.at<cv::Vec3b>(fil, col)[2] == 255 && segmento.conect.at<unsigned char>(fil, col) != 255)
							{
								segmento.contorno.at<cv::Vec3b>(fil, col)[2] = 255;
								segmento.conect.at<unsigned char>(fil, col) = 255;
								almacen.push_back({ fil,col });
							}
							if (imagencontorno.at<cv::Vec3b>(fil, col)[0] == 255 && segmento.conect.at<unsigned char>(fil, col) != 255)
							{
								segmento.contorno.at<cv::Vec3b>(fil, col)[0] = 255;
								segmento.conect.at<unsigned char>(fil, col) = 255;
								conector.push({ fil,col });
								almacen.push_back({ fil,col });
								segmento.tamObj++;
							}
						}
					}

					conector.pop();
				}
				iteraciones++;
				cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa.bmp", segmento.contorno);
				cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa2.bmp", segmento.conect);
				cv::imwrite("ImagenSalida\\muestra1.bmp", imagencontorno);
				cv::imwrite("ImagenSalida\\muestra2.bmp", contornoTemp);

				// Cerrar contornos de la figura con lo que queda dentro del queue. Son puntos que se pasan en el segmento, pero que no han sido comprobados
				while (!conector.empty())
				{
					int testfil1 = 1, testfil2 = 1, testcol1 = 1, testcol2 = 1;
					if (i - testfil1 < 0) testfil1 = 0;
					if (j - testcol1 < 0) testcol1 = 0;
					if (i + testfil2 >= contorno.rows) testfil2 = 0;
					if (j + testcol2 >= contorno.cols) testcol2 = 0;

					// Vale para comprobar que todos los píxeles azules extraídos a su alrededor tienen contorno rojo. Comprueba izq, der, arr y abj
					// que no sean azules los píxeles que les rodean y de no serlos les da valor rojo ya que sería el hipotético contorno.
					// Los añade al almacén de píxeles. El almacén se convierte en la suma de píxeles de contorno (perímetro) más área de la figura.
					if (segmento.contorno.at<cv::Vec3b>(conector.front().val[0] - testfil1, conector.front().val[1])[0] != 255)
					{
						segmento.contorno.at<cv::Vec3b>(conector.front().val[0] - testfil1, conector.front().val[1])[2] = 255;
						almacen.push_back({ conector.front().val[0] - testfil1, conector.front().val[1] });
					}
					if (segmento.contorno.at<cv::Vec3b>(conector.front().val[0] + testfil2, conector.front().val[1])[0] != 255)
					{
						segmento.contorno.at<cv::Vec3b>(conector.front().val[0] + testfil2, conector.front().val[1])[2] = 255;
						almacen.push_back({ conector.front().val[0] + testfil2, conector.front().val[1] });
					}
					if (segmento.contorno.at<cv::Vec3b>(conector.front().val[0], conector.front().val[1] - testcol1)[0] != 255)
					{
						segmento.contorno.at<cv::Vec3b>(conector.front().val[0], conector.front().val[1] - testcol1)[2] = 255;
						almacen.push_back({ conector.front().val[0], conector.front().val[1] - testcol1 });

					}
					if (segmento.contorno.at<cv::Vec3b>(conector.front().val[0], conector.front().val[1] + testcol2)[0] != 255)
					{
						segmento.contorno.at<cv::Vec3b>(conector.front().val[0], conector.front().val[1] + testcol2)[2] = 255;
						almacen.push_back({ conector.front().val[0], conector.front().val[1] + testcol2 });

					}
					conector.pop();
				}
				cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa3.bmp", segmento.contorno);
				
				// Aquí se comprueba la forma cuadrada de la figura extraída
				// Los puntos del almacén menos los puntos del tamaño de objeto del segmento serán los puntos del perímetro.
				double funcformacuad = kcuadrado*segmento.tamObj / ((almacen.size() - segmento.tamObj)*(almacen.size() - segmento.tamObj));
				
				// En el caso de que no se cumpla la función de forma, hacer esto de abajo. BORRAR EL EXTRACTO Y PONER NUEVO BORDE
				for (int l = 0; l < almacen.size(); l++)
				{
					// Para cada píxel del almacén comprueba si es azul y de serlo, lo borra.
					if (segmento.contorno.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1])[0] == 255)
					{
						contornoTemp.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1])[0] = 0;
						tamObjTemp--;
					}
				}
				cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa4.bmp", contornoTemp);

				for (int l = 0; l < almacen.size(); l++)
				{
					// Comprueba los contornos almacenados. Si estos no tienen a su alrededor ningún píxel azul que siga usándolo en la figura, lo
					// puede borrar.
					if (segmento.contorno.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1])[2] == 255)
					{
						int testfil1 = 1, testfil2 = 1, testcol1 = 1, testcol2 = 1;
						if (almacen[l].val[0] - testfil1 < 0) testfil1 = 0;
						if (almacen[l].val[1] - testcol1 < 0) testcol1 = 0;
						if (almacen[l].val[0] + testfil2 >= contornoTemp.rows) testfil2 = 0;
						if (almacen[l].val[1] + testcol2 >= contornoTemp.cols) testcol2 = 0;

						if (contornoTemp.at<cv::Vec3b>(almacen[l].val[0] - testfil1, almacen[l].val[1])[0] == 0 &&
							contornoTemp.at<cv::Vec3b>(almacen[l].val[0] + testfil2, almacen[l].val[1])[0] == 0 &&
							contornoTemp.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1] - testcol1)[0] == 0 &&
							contornoTemp.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1] + testcol2)[0] == 0)
						{
							contornoTemp.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1])[2] = 0;
							perObjTemp--;
						}
					}
				}
				cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa5.bmp", contornoTemp);

				for (int l = 0; l < almacen.size(); l++)
				{
					// En el caso de almacenarse un contorno rojo en la forma extraída donde en la figura real hay un puntos azules
					if (segmento.contorno.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1])[2] == 255 &&
						contornoTemp.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1])[0] == 255)
					{
						int testfil1 = 1, testfil2 = 1, testcol1 = 1, testcol2 = 1;
						if (almacen[l].val[0] - testfil1 < 0) testfil1 = 0;
						if (almacen[l].val[1] - testcol1 < 0) testcol1 = 0;
						if (almacen[l].val[0] + testfil2 >= contorno.rows) testfil2 = 0;
						if (almacen[l].val[1] + testcol2 >= contorno.cols) testcol2 = 0;

						// Si ese punto azul de la figura real tiene algún espacio vacío de azul, ponle contorno rojo. Si tenía contorno rojo
						// previamente lo vuelve a colocar, pero si era un vacío de los que se quedaban al quitar la otra figura, ponle un nuevo
						// borde. Además se le quita a ese punto la conexión de la matriz de comprobación para que esos puntos contorno vuelvan a 
						// ser utilizados en las siguientes segmentoes y no se queden contornos abiertos.
						if (contornoTemp.at<cv::Vec3b>(almacen[l].val[0] - testfil1, almacen[l].val[1])[0] == 0)
						{
							contornoTemp.at<cv::Vec3b>(almacen[l].val[0] - testfil1, almacen[l].val[1])[2] = 255;
							segmento.conect.at<unsigned char>(almacen[l].val[0] - testfil1, almacen[l].val[1]) = 0;
							perObjTemp++;
						}
						if (contornoTemp.at<cv::Vec3b>(almacen[l].val[0] + testfil2, almacen[l].val[1])[0] == 0)
						{
							contornoTemp.at<cv::Vec3b>(almacen[l].val[0] + testfil2, almacen[l].val[1])[2] = 255;
							segmento.conect.at<unsigned char>(almacen[l].val[0] + testfil2, almacen[l].val[1]) = 0;
							perObjTemp++;
						}
						if (contornoTemp.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1] - testcol1)[0] == 0)
						{
							contornoTemp.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1] - testcol1)[2] = 255;
							segmento.conect.at<unsigned char>(almacen[l].val[0], almacen[l].val[1] - testcol1) = 0;
							perObjTemp++;
						}
						if (contornoTemp.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1] + testcol2)[0] == 0)
						{
							contornoTemp.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1] + testcol2)[2] = 255;
							segmento.conect.at<unsigned char>(almacen[l].val[0], almacen[l].val[1] + testcol2) = 0;
							perObjTemp++;
						}
					}
				}
				cv::imwrite("ImagenSalida\\" + std::to_string(iteraciones) + "cosa6.bmp", contornoTemp);
				

				// Elimina los puntos de posPixtemporal para recalcular la geometría de la figura.
				for (int k = 0; k < almacen.size(); k++)
				{
					for (int p = static_cast<int>(posPixTemp.size()) - 1; p >= 0; p--) // se empieza desde el final de posPixTemp para que no haya problema al borrar dos elementos consecutivos
					{
						if (posPixTemp[p].val[0] == almacen[k].val[0] && posPixTemp[p].val[1] == almacen[k].val[1])
						{
							posPixTemp.erase(posPixTemp.begin() + p);
						}
					}
				}

				GeometriaObj nuevaGeom= CalculaMomentos(posPixTemp, perObjTemp);

				double factformaelipseNuevo = nuevaGeom.kelipseIdeal*tamObjTemp/(perObjTemp*perObjTemp);
				if (factformaelipseNuevo > geometriaImagen.factformaelipse)
				{
					imagencontorno = contornoTemp; // copia temporal de la imagencontorno original.
					tamObj = tamObjTemp;
					perObj = perObjTemp;
					posPix = posPixTemp;
					geometriaImagen.factformaelipse = factformaelipseNuevo;
					cv::imwrite("ImagenSalida\\muestra1.bmp", imagencontorno);
					cv::imwrite("ImagenSalida\\muestra2.bmp", contornoTemp);
				}
				else
				{
					tamObjTemp = tamObj;
					perObjTemp = perObj;
					posPixTemp = posPix;
					contornoTemp = imagencontorno;
					cv::imwrite("ImagenSalida\\muestra1.bmp", imagencontorno);
					cv::imwrite("ImagenSalida\\muestra2.bmp", contornoTemp);
				}

				// Restituye los puntos de la matriz de contorno a 0 para siguientes iteraciones.
				for (int l = 0; l < almacen.size(); l++)
				{
					segmento.contorno.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1])[0] = 0;
					segmento.contorno.at<cv::Vec3b>(almacen[l].val[0], almacen[l].val[1])[2] = 0;
				}
				// Borra el almacén.
				almacen.clear();

			}
		}
	}


}
*/
GeometriaObj imgDatos::CalculaMomentos(std::vector<cv::Vec2i> posPixentrada, int perObjentrada)
{
	GeometriaObj nuevaGeom;
	
	/// CALCULO CENTROIDE (CENTRO DE GRAVEDAD)

	for (int i = 0; i < posPixentrada.size(); i++)
	{
		nuevaGeom.centroide.val[0] += posPixentrada[i].val[0] + 0.5;
		nuevaGeom.centroide.val[1] += posPixentrada[i].val[1] + 0.5;
	}
	nuevaGeom.centroide.val[0] /= posPixentrada.size();
	nuevaGeom.centroide.val[1] /= posPixentrada.size();


	/// MOMENTOS DE INERCIA CENTRALES (respecto del centroide)

	for (int i = 0; i < posPixentrada.size(); i++)
	{
		nuevaGeom.InerciaX += (posPixentrada[i].val[1] + 0.5 - nuevaGeom.centroide.val[1])*(posPixentrada[i].val[1] + 0.5 - nuevaGeom.centroide.val[1]);
		nuevaGeom.InerciaY += (posPixentrada[i].val[0] + 0.5 - nuevaGeom.centroide.val[0])*(posPixentrada[i].val[0] + 0.5 - nuevaGeom.centroide.val[0]);
		nuevaGeom.InerciaXY += (posPixentrada[i].val[0] + 0.5 - nuevaGeom.centroide.val[0])*(posPixentrada[i].val[1] + 0.5 - nuevaGeom.centroide.val[1]);
	}


	/// ANGULO PARA EJES PRINCIPALES

	// El angulo realiza el menor giro de los ejes X e Y para alcanzar la posición de ejes principales de inercia.
	// Sentido horario - y sentido antihorario +.
	nuevaGeom.AngGiro = -std::atan((-2 * nuevaGeom.InerciaXY) / (nuevaGeom.InerciaY - nuevaGeom.InerciaX)) / 2;

	// Se ajusta el angulo de giro para que el eje X (eje fila) siempre tengo el momento principal de inercia
	// más pequeño (alineado con el insecto). Para el otro eje principal habría que añadirle PI/2 más.
	if (nuevaGeom.InerciaX > nuevaGeom.InerciaY)
	{
		nuevaGeom.AngGiro += (PI / 2);
	}


	/// EXCENTRICIDAD DEL A FORMA. EJES DE LA ELIPSE EQUIVALENTE COLOCADA EN EL CENTRO DE GRAVEDAD Y ORIENTADA SEGÚN EL ÁNGULO DE ROTACIÓN.

	double a1 = nuevaGeom.InerciaX + nuevaGeom.InerciaY + std::sqrt((nuevaGeom.InerciaX - nuevaGeom.InerciaY)*(nuevaGeom.InerciaX - nuevaGeom.InerciaY) + 4 * nuevaGeom.InerciaXY*nuevaGeom.InerciaXY);
	double a2 = nuevaGeom.InerciaX + nuevaGeom.InerciaY - std::sqrt((nuevaGeom.InerciaX - nuevaGeom.InerciaY)*(nuevaGeom.InerciaX - nuevaGeom.InerciaY) + 4 * nuevaGeom.InerciaXY*nuevaGeom.InerciaXY);
	double exc = a1 / a2;
	double ra = std::sqrt(2 * a1 / posPixentrada.size());
	double rb = std::sqrt(2 * a2 / posPixentrada.size());
	double PerIdeal = PI*(3 * (ra + rb) - std::sqrt((3 * ra + rb)*(ra + 3 * rb)));
	double AreaIdeal = PI*ra*rb;
	nuevaGeom.kelipseIdeal = PerIdeal*PerIdeal / AreaIdeal;
	/*
	// SI SE QUIERE DEIBUJAR LA ELIPSE!!!
	cv::Mat Elipse(contorno.size(), CV_8UC1, cv::Scalar(0));
	for (double i = 0; i < 2 * PI; i = i + 0.001)
	{
	Elipse.at<unsigned char>(int(nuevaGeom.centroide.val[0]+std::cos(nuevaGeom.AngGiro)*ra*std::cos(i)-std::sin(nuevaGeom.AngGiro)*rb*std::sin(i)), int(nuevaGeom.centroide.val[1] + std::sin(nuevaGeom.AngGiro)*ra*std::cos(i) - std::cos(nuevaGeom.AngGiro)*rb*std::sin(i))) = 255;
	}
	cv::imwrite("ImagenSalida\\elipse.bmp", Elipse);
	*/

	/// FACTOR DE FORMA PARA LA ELIPSE EQUIVALENTE
	nuevaGeom.factformaelipse = nuevaGeom.kelipseIdeal*posPixentrada.size() / (perObjentrada*perObjentrada);
	return nuevaGeom;
}

