#include<opencv2/core/core.hpp> // Librerías necesarias para usar OpenCV
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<conio.h>     
#include<vector>
#include"FuncionesVarias.h"
#include"ImgDatos.h"
#include<math.h>
#include<queue>


int main() 
{

	cv::Mat imagen= cv::imread("Fotos\\Tv1.bmp", CV_LOAD_IMAGE_COLOR);
	cv::Mat imagenGris;
	cv::Mat imagenUmbral;


	for (int imgarchivo = 1; imgarchivo <= 20; imgarchivo++)
	{
		// Cargamos imagen
		imagen = cv::imread("Fotos\\Tv"+std::to_string(imgarchivo)+".bmp");
		if (imagen.dims == 0) continue; // Saltarse la foto 6 que no está. Si no carga foto, omites el contador.
		
	
		// Transformación a escala de grises
		cv::cvtColor(imagen, imagenGris, CV_BGR2GRAY);
		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_1.bmp", imagenGris);


		// Umbralización con Otsu
		cv::threshold(imagenGris, imagenUmbral, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
		//cv::imwrite("ImagenSalida\\prueba"+ std::to_string(imgarchivo) +"_2.bmp", imagenUmbral);

		// Invertir umbralización 255 -> 0 y 0 -> 255
		for (int i = 0; i <= imagenUmbral.u->size; i++) // imagenUmbral.u->size elementos de la matriz
		{
			if (imagenUmbral.data[i] == 0)
			{
				imagenUmbral.data[i] = 255;
			}
			else
			{
				imagenUmbral.data[i] = 0;
			}
		}
		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_3.bmp", imagenUmbral);
		int prueba= imagen.channels();
		
		
		///  FORMACIÓN DE OBJETOS 
		imgDatos datos(&imagenUmbral);
		for (int i = 0; i < imagenUmbral.rows; i++)
		{
			for (int j = 0; j < imagenUmbral.cols; j++)
			{
				if (imagenUmbral.at<unsigned char>(i, j)==255 && datos.conect.at<unsigned char>(i,j)!=255) 
				// Si encuentras un píxel con contenido y no ha sido analizado previamente
				{	
					datos.contObj++; // aumenta en uno el contador de los objetos
					datos.tamObj.push_back(1);// Introduce un nuevo elemento al vector de tamaño de objetos
					datos.centroide.push_back({ i + 0.5f,j + 0.5f });
					// y dile que, en principio, abarca 1 píxel de la imagen umbralizada.
					datos.formaObjetos(i, j, &imagenUmbral);// extiéndete en los elementos conexos y forma el objeto
				}
			}
		}
		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_4.bmp", datos.conect);
		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_5.bmp", datos.numObj*255/datos.contObj);
		
		/// DIBUJAR CENTROIDES DE LOS OBJETOS UMBRALIZADOS
		// Imagen negra donde se albergan los centroides. Será una imagen BGR de ceros donde los centroides se dibujarán en rojo.
		cv::Mat imgCentroides(imagen.size(), CV_8UC3, cv::Scalar({ 0,0,0 }));
		cv::Mat imgCentroidesSep[3]; //se almacenan por separado las bandas aquí.
		cv::split(imgCentroides, imgCentroidesSep); //se dividen las bandas para poder trabajar con una de ellas.
		
		for (int i = 0; i < datos.centroide.size(); i++) //desde cero hasta el último centroide detectado
		{	// banda del rojo
			imgCentroidesSep[2].at<unsigned char>(int(datos.centroide[i].val[0]), int(datos.centroide[i].val[1])) =255;
		}
		cv::merge(imgCentroidesSep, 3, imgCentroides); // se vuelven a unir modificadas
		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_6.bmp", imgCentroides);

		/// MOMENTOS DE INERCIA DEL INSECTO
		// Localización del objeto más grande
		int n = 0; // localizador del objeto más grande. Se podría evitar si borrase el resto de objetos.
		for (int i = 0; i < datos.contObj; i++)
		{
			if (datos.tamObj[i] > datos.tamObj[n])
			{
				n = i;
			}
		}

		// Cálculo de momentos de inercia centrales en el objeto más grande
		for (int i = 0; i < datos.posPix.size(); i++)
		{
			datos.InerciaX += (datos.posPix[i].val[1]+0.5f - datos.centroide[n].val[1])*(datos.posPix[i].val[1]+0.5f - datos.centroide[n].val[1]);
			datos.InerciaY += (datos.posPix[i].val[0]+0.5f - datos.centroide[n].val[0])*(datos.posPix[i].val[0]+0.5f - datos.centroide[n].val[0]);
			datos.InerciaXY += (datos.posPix[i].val[0]+0.5f - datos.centroide[n].val[0])*(datos.posPix[i].val[1]+0.5f - datos.centroide[n].val[1]);
		}

		/// ANGULO PARA EJES PRINCIPALES
		// El símbolo está configurado para que el giro de los ejes en sentido de las agujas del reloj siempre sea
		// negativo y giro antihorario positivo. InerciasX > InerciasY suponen que el insecto está en horizontal y es el eje Y el que se alinea
		// con el eje principal del insecto. Al contrario, el eje X (vertical descendente) sería la dirección principal que se alinea con el insecto
		// Se podría sumar 90º para corregir en el caso de requerirlo y así sería siempre el eje X el alineado con el insecto.
		if (datos.InerciaX > datos.InerciaY)
		{
			datos.AngGiro = (3.141593f)/2.0f-std::atan((2.0f * datos.InerciaXY) / (datos.InerciaX - datos.InerciaY)) / 2.0f; //ángulo en radianes
		}
		else
		{
			datos.AngGiro = std::atan((2 * datos.InerciaXY) / (datos.InerciaY - datos.InerciaX)) / 2; //ángulo en radianes

		}
		{
			float AngGiroGrad = 360.0f * datos.AngGiro / (2.0f * 3.141593f);
			int a=0;
		}

		/// DIBUJAR CONTORNO
		cv::Mat contornoSep[3];
		cv::split(datos.contorno, contornoSep);

		for (int k = 0; k < datos.posPix.size();k++) // bucle por cada posición del vector de coordenadas del insecto
		{
			int i = datos.posPix[k].val[0]; // tomo posición de fila
			int j = datos.posPix[k].val[1]; // tomo posición de columna
			contornoSep[0].at<unsigned char>(i, j) = 255; // Insecto en azul
			contornoSep[1].at<unsigned char>(i, j) = 0; // Le quitas el verde de contornos de otra iteración

			// Si alrededor hay espacio (no te sales de la imagen) y si no valen ya 255 las bandas de azul, dale el verde al contorno.
			if (i - 1 >= 0 && contornoSep[0].at<unsigned char>(i-1, j) != 255) contornoSep[1].at<unsigned char>(i - 1, j) = 255;
			if (i + 1 <= contornoSep[1].rows && contornoSep[0].at<unsigned char>(i + 1, j) != 255) contornoSep[1].at<unsigned char>(i + 1, j) = 255;
			if (j - 1 >= 0 && contornoSep[0].at<unsigned char>(i, j-1) != 255) contornoSep[1].at<unsigned char>(i, j - 1) = 255;
			if (j + 1 <= contornoSep[1].cols && contornoSep[0].at<unsigned char>(i, j+1) != 255)contornoSep[1].at<unsigned char>(i, j + 1) = 255;
		}
		
		
		/// DIFERENCIAR CONTORNO EXTERNO DE INTERNO: ROJO EXTERNO, VERDE INTERNO, INSECTO AZUL
		assert(datos.posPix[0].val[0] - 1 > 0); // Si falla esto significa que la figura está pegada en el extremo y el borde se corta
		// en la foto
		std::queue<cv::Vec2i> vecRellenaCont;
		vecRellenaCont.push({ datos.posPix[0].val[0] - 1,datos.posPix[0].val[1] }); // se introduce la posición inicial del contorno
		contornoSep[1].at<unsigned char>(datos.posPix[0].val[0] - 1, datos.posPix[0].val[1]) = 0; // le quito el verde
		contornoSep[2].at<unsigned char>(datos.posPix[0].val[0] - 1, datos.posPix[0].val[1]) = 255; // cambiamos el color del contorno externo a rojo. Sirve para diferenciar.

		while (!vecRellenaCont.empty())
		{
			int testfil1 = 1, testfil2 = 1, testcol1 = 1, testcol2 = 1;
			if (vecRellenaCont.front().val[0] == 0) testfil1 = 0;
			if (vecRellenaCont.front().val[0] == imagen.rows) testfil1 = 0;
			if (vecRellenaCont.front().val[1] == 0) testcol1 = 0;
			if (vecRellenaCont.front().val[1] == imagen.cols) testcol2 = 0;
			assert(vecRellenaCont.front().val[0] - testfil1 >= 0); // En el caso de que la resta sea un valor menor que cero, salta un error.
			assert(vecRellenaCont.front().val[1] - testcol1 >= 0); // En el caso de que la resta sea un valor menor que cero, salta un error.
			assert(vecRellenaCont.front().val[0] + testfil2 <= imagen.rows); // En el caso de que la resta sea un valor mayor que el núm de filas, error.
			assert(vecRellenaCont.front().val[1] + testcol2 <= imagen.cols); // En el caso de que la resta sea un valor mayor que el núm de colmns, error.
			
			for (int fil = vecRellenaCont.front().val[0] - testfil1; fil <= vecRellenaCont.front().val[0] + testfil2; fil++)
			{
				for (int col = vecRellenaCont.front().val[1] - testcol1; col <= vecRellenaCont.front().val[1] + testcol2; col++)
				{
					if (contornoSep[1].at<unsigned char>(fil, col) == 255)
					{
						vecRellenaCont.push({ fil,col });
						contornoSep[1].at<unsigned char>(fil, col) = 0;
						contornoSep[2].at<unsigned char>(fil, col) = 255;
					}
				}
			}
			vecRellenaCont.pop();

		}
		
		cv::merge(contornoSep, 3, datos.contorno);
		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_7.bmp", datos.contorno);


		/// DIBUJA EJE PRINCIPAL

		cv::Mat Ejes(imagen.size(), CV_8UC1, cv::Scalar(0)); // Imagen matriz que mostrará los ejes principales de inercia
		DibujaEje(&Ejes, &datos.centroide[n], datos.AngGiro);
		DibujaEjeSec(&Ejes, &datos.centroide[n], datos.AngGiro);
		cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_8.bmp", Ejes);
	}	




	return(0);
}



