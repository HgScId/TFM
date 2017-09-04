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
#include"Eje.h"

#define PI 3.141592653589793238463


int main() 
{
	for (int imgarchivo = 1; imgarchivo <= 20; imgarchivo++)
	{
		/// LECTURA DE LA IMAGEN
		cv::Mat imagen;
		imagen = cv::imread("Fotos\\Tv"+std::to_string(imgarchivo)+".bmp", CV_LOAD_IMAGE_COLOR);
		if (imagen.dims == 0) continue; // Saltarse la foto 6 que no está. Si no carga foto, omites el contador.
		

		/// UMBRALIZACIÓN CON OTSU MEDIANTE LA BANDA DE AZUL
		cv::Mat imagenSep[3];
		cv::split(imagen, imagenSep);
		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_1.bmp", imagenSep[0]);
		
		cv::Mat imagenUmbral;
		cv::threshold(imagenSep[0], imagenUmbral, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
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
		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) +"_4.bmp", datos.contorno);

		while (datos.geometriaImagen.factformaelipse < 0.4)
		{
			datos.Erosion(datos.contorno, 3);
		}
		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_5.bmp", datos.contorno);


		
		
		
		/*
		/// DIBUJAR CENTROIDE DEL OBJETO PRINCIPAL
		// Imagen negra donde se albergan los centroides. Será una imagen BGR de ceros donde los centroides se dibujarán en rojo.
		cv::Mat imgCentroide(imagen.size(), CV_8UC3, cv::Scalar({ 0,0,0 }));
		imgCentroide.at<cv::Vec3b>(int(datos.geometriaImagen.centroide.val[0]), int(datos.geometriaImagen.centroide.val[1]))[0] = 0;
		imgCentroide.at<cv::Vec3b>(int(datos.geometriaImagen.centroide.val[0]), int(datos.geometriaImagen.centroide.val[1]))[1] = 0;
		imgCentroide.at<cv::Vec3b>(int(datos.geometriaImagen.centroide.val[0]), int(datos.geometriaImagen.centroide.val[1]))[2] = 255;

		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_4.bmp", imgCentroide);
		*/
		
		
		/// DIBUJA EJES PRINCIPALES DE INERCIA
		Eje EjePrin(imagen); // Imagen matriz que mostrará los ejes principales de inercia
		Eje EjeSec(imagen); // Imagen matriz que mostrará los ejes principales de inercia
		
		EjePrin.DibujaEje(EjePrin, datos.geometriaImagen.centroide, datos.geometriaImagen.AngGiro);
		EjeSec.DibujaEjeSec(EjeSec, datos.geometriaImagen.centroide, datos.geometriaImagen.AngGiro);
		
		//Eje Ejes(imagen); // Imagen matriz que mostrará los dos ejes.
		//Ejes.DibujaEje(Ejes, datos.geometriaImagen.centroide, datos.geometriaImagen.AngGiro);
		//Ejes.DibujaEjeSec(Ejes, datos.geometriaImagen.centroide, datos.geometriaImagen.AngGiro);
		//cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_6.bmp", Ejes.mat);

		/// BUSCAR INTERSECCIÓN CONTORNO - EJE PRINCIPAL Y LONGITUD
		double longPrin = EjePrin.Intersecc(EjePrin, datos.contorno);
		//std::cout << "La distancia de cola a cabeza en la imagen numero " << imgarchivo << " es de " << longPrin << " pixeles.\n";
		EjeSec.IntEje = EjeSec.InterseccSec(datos.geometriaImagen.centroide, EjePrin, datos.contorno, datos.geometriaImagen.AngGiro);
		cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_7.bmp", EjeSec.IntEje.recta1);
		cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_8.bmp", EjeSec.IntEje.recta2);
	}	


	

	return (0);
}



