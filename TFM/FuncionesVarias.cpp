#include "FuncionesVarias.h"
#include <opencv2/core/core.hpp> // Librerías necesarias para usar OpenCV
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include<iostream>


void DibujaHist(cv::Mat imagen)
{
	// Dividir las imágenes y representarlas en escala de grises
	std::vector<cv::Mat> imagenBandas;
	cv::split(imagen, imagenBandas);
	
	//
	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	bool uniform = true; bool accumulate = false;

	cv::Mat b_hist, g_hist, r_hist;

	// Compute the histograms:
	calcHist(&imagenBandas[0], 1, 0, cv::Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&imagenBandas[1], 1, 0, cv::Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
	calcHist(&imagenBandas[2], 1, 0, cv::Mat(), r_hist, 1, &histSize, &histRange, uniform, accumulate);
	for (int i = 0; i <= 255; i++)
	{
		std::cout << int(b_hist.data[i]) << " ";
	}
	std::cout << "\n ACABA AZUL\n";
	for (int i = 256; i <= 1023; i++)
	{
		std::cout << int(b_hist.data[i]) << " ";
	}
	std::cout << "\n ACABA AZUL\n";

	for (int i = 0; i <= 1023; i++)
	{
		std::cout << int(g_hist.data[i]) << " ";
	}
	std::cout << "\n ACABA AZUL\n";
	for (int i = 0; i <= 1023; i++)
	{
		std::cout << int(r_hist.data[i]) << " ";
	}

	// Draw the histograms for B, G and R
	int hist_w = 800; int hist_h = 600;
	int bin_w = cvRound((double)hist_w / histSize);

	cv::Mat histImageB(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat histImageG(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));
	cv::Mat histImageR(hist_h, hist_w, CV_8UC3, cv::Scalar(0, 0, 0));

	// Normalize the result to [ 0, histImage.rows ]
	normalize(b_hist, b_hist, 0, histImageB.rows, cv::NORM_MINMAX, -1, cv::Mat());
	normalize(g_hist, g_hist, 0, histImageG.rows, cv::NORM_MINMAX, -1, cv::Mat());
	normalize(r_hist, r_hist, 0, histImageR.rows, cv::NORM_MINMAX, -1, cv::Mat());

	// Draw each channel
	for (int i = 1; i < histSize; i++)
	{
		line(histImageB, cv::Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			cv::Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
			cv::Scalar(255, 0, 0), 2, 8, 0);
		line(histImageG, cv::Point(bin_w*(i - 1), hist_h - cvRound(g_hist.at<float>(i - 1))),
			cv::Point(bin_w*(i), hist_h - cvRound(g_hist.at<float>(i))),
			cv::Scalar(0, 255, 0), 2, 8, 0);
		line(histImageR, cv::Point(bin_w*(i - 1), hist_h - cvRound(r_hist.at<float>(i - 1))),
			cv::Point(bin_w*(i), hist_h - cvRound(r_hist.at<float>(i))),
			cv::Scalar(0, 0, 255), 2, 8, 0);
	}
	// Display
	cv::namedWindow("Histograma Azul", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("Histograma Verde", CV_WINDOW_AUTOSIZE);
	cv::namedWindow("Histograma Rojo", CV_WINDOW_AUTOSIZE);

	imshow("Histograma Azul", histImageB);
	imshow("Histograma Verde", histImageG);
	imshow("Histograma Rojo", histImageR);
	cv::waitKey(0);
	cv::imwrite("Histograma.bmp", histImageB);
}

void ImagenBandas(cv::Mat imagen)
{
	std::vector<cv::Mat> imagenBandas;
	cv::split(imagen, imagenBandas);

	// Display
	cv::namedWindow("Banda Azul", CV_WINDOW_NORMAL);
	cv::namedWindow("Banda Verde", CV_WINDOW_NORMAL);
	cv::namedWindow("Banda Rojo", CV_WINDOW_NORMAL);
	
	cv::resizeWindow("Banda Azul", 1596/2, 1196/2);
	cv::resizeWindow("Banda Verde", 1596 / 2, 1196 / 2);
	cv::resizeWindow("Banda Rojo", 1596 / 2, 1196 / 2);
	
	imshow("Banda Azul", imagenBandas[0]);
	imshow("Banda Verde", imagenBandas[1]);
	imshow("Banda Rojo", imagenBandas[2]);
	cv::waitKey(0);

}
