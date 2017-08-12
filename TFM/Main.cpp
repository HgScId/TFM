#include<opencv2/core/core.hpp> // Librerías necesarias para usar OpenCV
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

#include<iostream>
#include<conio.h>     
#include<vector>
#include"FuncionesVarias.h"
#include"ImgDatos.h"


int main() 
{

	cv::Mat imagen= cv::imread("Fotos\\Tv1.bmp", CV_LOAD_IMAGE_COLOR);
	cv::Mat imagenGris;
	cv::Mat imagenUmbral;


	for (int imgarchivo = 1; imgarchivo <= 20; imgarchivo++)
	{
		// Cargamos imagen
		imagen = cv::imread("Fotos\\Tv"+std::to_string(imgarchivo)+".bmp");
		if (imagen.dims == 0) // Saltarse la foto 6 que no está. Si no carga foto, omites el contador.
		continue;
	
		// Transformación a escala de grises
		cv::cvtColor(imagen, imagenGris, CV_BGR2GRAY);
		cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_1.bmp", imagenGris);


		// Umbralización con Otsu
		cv::threshold(imagenGris, imagenUmbral, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
		cv::imwrite("ImagenSalida\\prueba"+ std::to_string(imgarchivo) +"_2.bmp", imagenUmbral);

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
		cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_3.bmp", imagenUmbral);
		int prueba= imagen.channels();
		//  Formar objetos 
		imgDatos datos(imagenUmbral);
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
		cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_4.bmp", datos.conect);
		cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_5.bmp", datos.numObj*255/datos.contObj);
		
		// Imagen negra donde se albergan los centroides. Será una imagen BGR de ceros donde los centroides se dibujarán en rojo.
		cv::Mat imgCentroides(imagen.size(), CV_8UC3, cv::Scalar({ 0,0,0 }));
		cv::Mat imgCentroidesSep[3]; //se almacenan por separado las bandas aquí.
		cv::split(imgCentroides, imgCentroidesSep); //se dividen las bandas para poder trabajar con una de ellas.
		
		for (int i = 0; i < datos.centroide.size(); i++) //desde cero hasta el último centroide detectado
		{	// banda del rojo
			imgCentroidesSep[2].at<unsigned char>(int(datos.centroide[i].val[0]), int(datos.centroide[i].val[1])) =255;
		}
		cv::merge(imgCentroidesSep, 3, imgCentroides); // se vuelven a unir modificadas
		cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_6.bmp", imgCentroides);

	}
	




	return(0);
}



