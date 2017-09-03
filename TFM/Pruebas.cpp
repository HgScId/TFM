#include<opencv2/core/core.hpp> // Librerías necesarias para usar OpenCV
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>

cv::Mat imgOriginal;        // input image
cv::Mat imgGrayscale;       // grayscale of input image
cv::Mat imgBlurred;         // intermediate blured image
cv::Mat imgCanny;           // Canny edge image


imgOriginal = cv::imread("image.bmp");          // open image


//Vector compatible con la función img.at<cv::Vec3b>(i,j)={B,G,R};
cv::Vec3b intensity = { 0,255,0 };


// Anula los valores G y R de la imagen
for (int i = 0; i <= imgOriginal.u->size; i = i + 3)
{
imgOriginal.data[i + 1] = 0;
imgOriginal.data[i + 2] = 0;
}

cv::namedWindow("imgOriginal2", CV_WINDOW_AUTOSIZE);
cv::imshow("imgOriginal2", imgOriginal);
cv::waitKey(0);


cv::Mat imgGrayscale2;
cv::cvtColor(imgOriginal, imgGrayscale2, CV_BGR2GRAY); // Transforma la imagen azul en escala de grises
// Fundamentalmente esta función hace la media aritmética de las tres componentes de la imagen en cada píxel
// Habría que comprobar que la operación que realiza OpenCV es esa realmente y no filtra de alguna manera cada
// banda para transformar a GrayScale.
cv::namedWindow("imgOriginal3", CV_WINDOW_AUTOSIZE);
cv::imshow("imgOriginal3", imgGrayscale2);
cv::waitKey(0);
// Multiplica por 3 el valor del píxel de la imagen de grises(ya que las componentes rojo y azul estaban anuladas
for (int i = 0; i <= imgGrayscale2.u->size; i++)
{
	imgGrayscale2.data[i] = imgGrayscale2.data[i] * 3;

}
cv::namedWindow("imgOriginal2", CV_WINDOW_AUTOSIZE);
cv::imshow("imgOriginal2", imgGrayscale2);
cv::waitKey(0);

// Transformar color de píxeles por secciones cuadradas de la imagen
for (int i = 0; i <= 1195; i++)
{
	for (int j = 0; j < 1595; j++)
	{
		imgOriginal.at<cv::Vec3b>(i, j) = intensity;

	}
}

if (imgOriginal.empty()) {                                  // if unable to open image
	std::cout << "error: image not read from file\n\n";     // show error message on command line
	_getch();                                               // may have to modify this line if not using Windows
	return(0);                                              // and exit program
}

cv::cvtColor(imgOriginal, imgGrayscale, CV_BGR2GRAY);       // convert to grayscale
															//cv::namedWindow("imgOriginal", CV_WINDOW_AUTOSIZE);

															//cv::imshow("imgOriginal", imgOriginal);     // show windows

cv::GaussianBlur(imgGrayscale,          // input image
	imgBlurred,                         // output image
	cv::Size(5, 5),                     // smoothing window width and height in pixels
	1.5);                               // sigma value, determines how much the image will be blurred

cv::Canny(imgBlurred,           // input image
	imgCanny,                   // output image
	100,                        // low threshold
	200);                       // high threshold

								// declare windows

								//cv::namedWindow("HOLA", CV_WINDOW_AUTOSIZE);
								//cv::namedWindow("HOLA2", CV_WINDOW_AUTOSIZE);

cv::namedWindow("imgOriginal", CV_WINDOW_AUTOSIZE);     // note: you can use CV_WINDOW_NORMAL which allows resizing the window
														//cv::namedWindow("imgCanny", CV_WINDOW_AUTOSIZE);        // or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
														// CV_WINDOW_AUTOSIZE is the default
cv::imshow("imgOriginal", imgOriginal);     // show windows
											//cv::resizeWindow("imgOriginal", 1596, 1196);
											//cv::imshow("imgCanny", imgCanny);
											//cv::Mat prueba = imgGrayscale - imgBlurred;
											//cv::imshow("HOLA", prueba);
											//cv::imshow("HOLA2", imgBlurred);



cv::waitKey(0);                 // hold windows open until user presses a key


// Crear una matriz de ceros de una banda.
cv::Mat m(100, 100, CV_8UC1, cv::Scalar(0)); 


/// INTENTO FALLIDO DE HACER UN AALGORITMO DE ORDEN DE MOVIMIENTO A LO LARGO DE UN CONTORNO
do
{
// Movimiento derecha
if (contornoSep[1].at<unsigned char>(posActual.val[0], posActual.val[1] + 1) == 255)
{	// Si hay contorno a la derecha almacenado en la banda verde, lo cambiamos a la banda roja ya que
// será del contorno externo.
posActual.val[1] += 1;
contornoSep[1].at<unsigned char>(posActual.val[0], posActual.val[1]) = 0;
contornoSep[2].at<unsigned char>(posActual.val[0], posActual.val[1]) = 255;
cv::merge(contornoSep, 3, datos.contorno);
cv::imshow("Imagen", datos.contorno);
cv::waitKey();
goto stop;
}

// Movimiento abajo
if (contornoSep[1].at<unsigned char>(posActual.val[0] + 1, posActual.val[1]) == 255)
{
posActual.val[0] += 1;
contornoSep[1].at<unsigned char>(posActual.val[0], posActual.val[1]) = 0;
contornoSep[2].at<unsigned char>(posActual.val[0], posActual.val[1]) = 255;
cv::merge(contornoSep, 3, datos.contorno);
cv::imshow("Imagen", datos.contorno);
cv::waitKey();
goto stop;
}

// Movimiento derecha abajo
if (contornoSep[1].at<unsigned char>(posActual.val[0] + 1, posActual.val[1] + 1) == 255)
{
posActual.val[0] += 1;
posActual.val[1] += 1;
contornoSep[1].at<unsigned char>(posActual.val[0], posActual.val[1]) = 0;
contornoSep[2].at<unsigned char>(posActual.val[0], posActual.val[1]) = 255;
cv::merge(contornoSep, 3, datos.contorno);
cv::imshow("Imagen", datos.contorno);
cv::waitKey();
goto stop;
}

// Movimiento izquierda
if (contornoSep[1].at<unsigned char>(posActual.val[0], posActual.val[1] - 1) == 255)
{
posActual.val[1] -= 1;
contornoSep[1].at<unsigned char>(posActual.val[0], posActual.val[1]) = 0;
contornoSep[2].at<unsigned char>(posActual.val[0], posActual.val[1]) = 255;
cv::merge(contornoSep, 3, datos.contorno);
cv::imshow("Imagen", datos.contorno);
cv::waitKey();
goto stop;
}

// Movimiento izquierda abajo
if (contornoSep[1].at<unsigned char>(posActual.val[0] + 1, posActual.val[1] - 1) == 255)
{
posActual.val[0] += 1;
posActual.val[1] -= 1;
contornoSep[1].at<unsigned char>(posActual.val[0], posActual.val[1]) = 0;
contornoSep[2].at<unsigned char>(posActual.val[0], posActual.val[1]) = 255;
cv::merge(contornoSep, 3, datos.contorno);
cv::imshow("Imagen", datos.contorno);
cv::waitKey();
goto stop;
}

// Movimiento arriba
if (contornoSep[1].at<unsigned char>(posActual.val[0] - 1, posActual.val[1]) == 255)
{
posActual.val[0] -= 1;
contornoSep[1].at<unsigned char>(posActual.val[0], posActual.val[1]) = 0;
contornoSep[2].at<unsigned char>(posActual.val[0], posActual.val[1]) = 255;
cv::merge(contornoSep, 3, datos.contorno);
cv::imshow("Imagen", datos.contorno);
cv::waitKey();
goto stop;
}
// Movimiento izquierda arriba
if (contornoSep[1].at<unsigned char>(posActual.val[0] - 1, posActual.val[1] - 1) == 255)
{
posActual.val[0] -= 1;
posActual.val[1] -= 1;
contornoSep[1].at<unsigned char>(posActual.val[0], posActual.val[1]) = 0;
contornoSep[2].at<unsigned char>(posActual.val[0], posActual.val[1]) = 255;
cv::merge(contornoSep, 3, datos.contorno);
cv::imshow("Imagen", datos.contorno);
cv::waitKey();
goto stop;
}


// Movimiento derecha arriba
if (contornoSep[1].at<unsigned char>(posActual.val[0]-1, posActual.val[1] + 1) == 255)
{
posActual.val[0] -= 1;
posActual.val[1] += 1;
contornoSep[1].at<unsigned char>(posActual.val[0], posActual.val[1]) = 0;
contornoSep[2].at<unsigned char>(posActual.val[0], posActual.val[1]) = 255;
cv::merge(contornoSep, 3, datos.contorno);
cv::imshow("Imagen", datos.contorno);
cv::waitKey();
goto stop;
}
else
{
std::cout << "SE HA BLOQUEADO!!!";
cv::merge(contornoSep, 3, datos.contorno);
cv::imwrite("ImagenSalida\\prueba" + std::to_string(imgarchivo) + "_7.bmp", datos.contorno);

cv::imshow("Imagen", datos.contorno);
cv::waitKey();
}
stop:;
}
while (posActual != posIni);
*/



/// IMPORTANTE!!!!
/// FORMA DE SEÑALAR EN UNA MATRIZ DE TRES CANALES!!!
cv::Mat imgCentroides(imagen.size(), CV_8UC3, cv::Scalar({ 0,0,0 }));
cv::Vec3b& señala = imgCentroides.at<cv::Vec3b>(0, 0);
señala[0] = 255;
señala[1] = 100;
señala[2] = 25;
unsigned char c1 = imgCentroides.at<cv::Vec3b>(0, 0)[0];
unsigned char c2 = imgCentroides.at<cv::Vec3b>(0, 0)[1];
unsigned char c3 = imgCentroides.at<cv::Vec3b>(0, 0)[2];
/// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
/// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



/// SEGMENTACIÓN CON OTSU POR BANDAS. MEJORA DEL AZUL EN LA DELIMITACIÓN.
cv::Mat imagenSep[3];
cv::split(imagen, imagenSep);
cv::Mat imagenUmbralB;
cv::Mat imagenUmbralG;
cv::Mat imagenUmbralR;


cv::threshold(imagenSep[0], imagenUmbralB, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
cv::threshold(imagenSep[1], imagenUmbralG, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
cv::threshold(imagenSep[2], imagenUmbralR, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);

cv::Mat resta(imagen.size(), CV_8UC3, cv::Scalar({ 0,0,0 }));

for (int i = 0; i < imagen.rows; i++)
{
	for (int j = 0; j < imagen.cols; j++)
	{
		int numB = imagenSep[0].at<unsigned char>(i, j) - imagenUmbralR.at<unsigned char>(i, j);
		int numG = imagenSep[1].at<unsigned char>(i, j) - imagenUmbralR.at<unsigned char>(i, j);
		int numR = imagenSep[2].at<unsigned char>(i, j) - imagenUmbralR.at<unsigned char>(i, j);
		if (numB < 0) numB = 0;
		if (numG < 0) numG = 0;
		if (numR < 0) numR = 0;
		resta.at<cv::Vec3b>(i, j)[0] = numB;
		resta.at<cv::Vec3b>(i, j)[1] = numG;
		resta.at<cv::Vec3b>(i, j)[2] = numR;
	}
}
cv::imwrite("ImagenSalida\\resta" + std::to_string(imgarchivo) + "R.bmp", resta);



/// SOBEL GRADIENTES BORDES 
cv::Mat imagengris, gradx, grady, grad, gradxy;
cvtColor(imagen, imagengris, CV_BGR2GRAY);

Sobel(imagengris, gradx, CV_32F, 1, 0, 3);
gradx = gradx.mul(gradx);
cv::convertScaleAbs(gradx, gradx);

cv::imwrite("ImagenSalida\\gradx.bmp", gradx);
Sobel(imagengris, gradxy, CV_8U, 1, 1, 3);
cv::imwrite("ImagenSalida\\gradxy.bmp", gradxy);
Sobel(imagengris, grady, CV_8U, 0, 1, 3);
cv::imwrite("ImagenSalida\\grady.bmp", grady);
cv::addWeighted(gradx, 0.5, grady, 0.5, 0, grad);
cv::imwrite("ImagenSalida\\grad.bmp", grad);