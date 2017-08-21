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
	for (int imgarchivo = 1; imgarchivo <= 20; imgarchivo++)
	{
		/// LECTURA DE LA IMAGEN
		cv::Mat imagen;
		imagen = cv::imread("Fotos\\Tv"+std::to_string(imgarchivo)+".bmp", CV_LOAD_IMAGE_COLOR);
		if (imagen.dims == 0) continue; // Saltarse la foto 6 que no está. Si no carga foto, omites el contador.
		
	
		/// TRANSFORMACIÓN IMAGEN BGR A ESCALA DE GRISES
		cv::Mat imagenGris;
		cv::cvtColor(imagen, imagenGris, CV_BGR2GRAY);
		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_1.bmp", imagenGris);


		/// UMBRALIZACIÓN CON OTSU
		cv::Mat imagenUmbral;
		cv::threshold(imagenGris, imagenUmbral, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
		//cv::imwrite("ImagenSalida\\prueba"+ std::to_string(imgarchivo) +"_2.bmp", imagenUmbral);
		// Invertir umbralización 255 -> 0 y 0 -> 255. 
		// Utilizando el miembro data de las imágenes de OpenCV se evita usar doble bucle para acceder a los elementos con la función at.
		for (int i = 0; i < imagenUmbral.rows*imagenUmbral.cols; i++)
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
		
		
		///  FORMACIÓN DE OBJETOS 
		imgDatos datos(imagenUmbral); // Se crea el objeto de la clase de datos de la imagen que almacenará la información extraída.

		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_4.bmp", datos.conect);
		
		/// DIBUJAR CENTROIDE DEL OBJETO PRINCIPAL
		// Imagen negra donde se albergan los centroides. Será una imagen BGR de ceros donde los centroides se dibujarán en rojo.
		cv::Mat imgCentroide(imagen.size(), CV_8UC3, cv::Scalar({ 0,0,0 }));
		imgCentroide.at<cv::Vec3b>(int(datos.centroide.val[0]), int(datos.centroide.val[1]))[0] = 0;
		imgCentroide.at<cv::Vec3b>(int(datos.centroide.val[0]), int(datos.centroide.val[1]))[1] = 0;
		imgCentroide.at<cv::Vec3b>(int(datos.centroide.val[0]), int(datos.centroide.val[1]))[2] = 255;

		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_6.bmp", imgCentroide);

		
		/// DIBUJAR INSECTO AZUL, CONTORNO EXTERNO ROJO Y CONTORNO INTERNO VERDE
		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_7.bmp", datos.contorno);


		/// DIBUJA EJES PRINCIPALES DE INERCIA

		cv::Mat EjePrin(imagen.size(), CV_8UC1, cv::Scalar(0)); // Imagen matriz que mostrará los ejes principales de inercia
		cv::Mat EjeSec(imagen.size(), CV_8UC1, cv::Scalar(0)); // Imagen matriz que mostrará los ejes principales de inercia
		DibujaEje(EjePrin, datos.centroide, datos.AngGiro);
		DibujaEjeSec(EjeSec, datos.centroide, datos.AngGiro);
		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_8.bmp", Ejes);

		/// BUSCAR INTERSECCIÓN CONTORNO EJE PRINCIPAL
		std::vector<cv::Vec2i> intersecc;
		for (int i = 0; i < EjePrin.rows; i++)
		{
			for (int j = 0; j < EjePrin.cols; j++)
			{
				// Comprobación para que no se pueda salir de la imagen el chequeo de columna.
				int testcol1 = 1, testcol2 = 1;
				if (j - testcol1 < 0) testcol1 = 0;
				if (j + testcol2 >= EjePrin.cols) testcol2 = 0;
				if (datos.contorno.at<cv::Vec3b>(i, j)[2] == 255 && EjePrin.at<unsigned char>(i, j) == 255 ||
					datos.contorno.at<cv::Vec3b>(i, j+testcol2)[2] == 255 && EjePrin.at<unsigned char>(i, j) == 255 || //Estas dos comprobaciones son para 
					datos.contorno.at<cv::Vec3b>(i, j-testcol1)[2] == 255 && EjePrin.at<unsigned char>(i, j) == 255) //casos en los que se cruzan. Ejemplo imagen 3
				{
					if (intersecc.size() >= 2)	intersecc.pop_back();
					intersecc.push_back({ i,j });
				}
			}
		}
		auto dist = std::sqrt(std::pow((intersecc[0].val[0] - intersecc[1].val[0]), 2) + std::pow((intersecc[0].val[1] - intersecc[1].val[1]-2), 2)); 
		// Ese -2 sirve para eliminar las dos unidades columnas añadidas en cada medición para evitar los cruces de líneas.
		std::cout << "La distancia de cola a cabeza en la imagen numero " << imgarchivo << " es de " << dist << " pixeles.\n";
	}	




	return (0);
}



