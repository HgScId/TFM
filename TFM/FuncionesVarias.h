#pragma once
#include <opencv2/core/core.hpp> // Librerías necesarias para usar OpenCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/// 1º CALCULAR Y DIBUJAR UN HISTOGRAMA
// Se toma una imagen en RGB (BGR) para OpenCV, se divide en sus tres bandas, se calcula el histograma y se muestra.

void DibujaHistColor(cv::Mat& imagen);

void DibujaHistGris(cv::Mat& imagen); // Imagen de un canal.

/// 2º MOSTRAR UNA IMAGEN POR BANDAS EN ESCALA DE GRISES

void ImagenBandas(cv::Mat& imagen);
