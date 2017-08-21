#pragma once
#include <opencv2/core/core.hpp> // Librerías necesarias para usar OpenCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/// 1º CALCULAR Y DIBUJAR UN HISTOGRAMA
// Se toma una imagen en RGB (BGR) para OpenCV, se divide en sus tres bandas, se calcula el histograma y se muestra.

void DibujaHist(cv::Mat imagen);

/// 2º MOSTRAR UNA IMAGEN POR BANDAS EN ESCALA DE GRISES

void ImagenBandas(cv::Mat imagen);

/// 3º DIBUJA EJE PRINCIPAL DE INERCIA Y LO DEVUELVE COMO IMAGEN

void DibujaEje(cv::Mat& imagenRef, cv::Vec2f& centroide, float angulo);

/// 4º DIBUJA EJE SECUNDARIO

void DibujaEjeSec(cv::Mat& imagenRef, cv::Vec2f& centroide, float angulo);
