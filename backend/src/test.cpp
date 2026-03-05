#include <cstdio>
#include <iostream>

#include <base/color_similarity.hpp>
#include <base/image.hpp>

#include <qimage.h>

#include "image_utility.hpp"
#include "energy_mat.hpp"

int main()
{
    std::cout << "Input the input file path:\n";
    std::string inFilepath;
    std::cin >> inFilepath;

    std::cout << "Input the output file path:\n";
    std::string outFilepath;
    std::cin >> outFilepath;

    const Image img = loadImageFromFile(inFilepath);
    if (img.isEmpty())
    {
        printf("Failed to load image.\n");
        exit(1);
    }
    printf("Origin size: [%d, %d].\n", img.cols, img.rows);

    const Image scaledImg = limitImageScale(img, 720, 720);
    printf("Scaled size: [%d, %d].\n", scaledImg.cols, scaledImg.rows);

    const Image energyImg = energyMatToGrayImage(normalizeEnergyMat(createEnergyMat(scaledImg)));

    const auto qimg = QImage(
        energyImg.data(), energyImg.cols, energyImg.rows, energyImg.cols, QImage::Format_Grayscale8);
    // const auto qimg = QImage(
    //     scaledImg.data(), scaledImg.cols, scaledImg.rows, 3 * scaledImg.cols, QImage::Format_RGB888);
    if (qimg.isNull())
    {
        printf("The Qt image is Null.\n");
        exit(1);
    }

    if (qimg.save(outFilepath.c_str()))
        printf("Save image successfully.\n");
    else
        printf("Failed to save image.\n");

    return 0;
}
