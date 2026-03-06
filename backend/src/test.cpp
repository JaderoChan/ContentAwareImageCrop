#include <cstdio>
#include <iostream>

#include <base/color_similarity.hpp>
#include <base/image.hpp>

#include <qimage.h>

#include "image_utility.hpp"
#include "energy_mat.hpp"
#include "content_aware_image_crop.hpp"

int main()
{
    std::cout << "Input the input file path:\n";
    std::string inFilepath;
    std::cin >> inFilepath;

    std::cout << "Input the output file path:\n";
    std::string outFilepath;
    std::cin >> outFilepath;

    Image img = loadImageFromFile(inFilepath);
    if (img.isEmpty())
    {
        printf("Failed to load image.\n");
        exit(1);
    }
    printf("Origin size: [%d, %d].\n", img.cols, img.rows);

    Image scaledImg = limitImageScale(img, 720, 720);
    printf("Scaled size: [%d, %d].\n", scaledImg.cols, scaledImg.rows);

    auto energyMat = createEnergyMat(scaledImg);
    normalizeEnergyMat(energyMat);
    Image energyImg = energyMatToGrayImage(energyMat);

    auto line = getMinimumEnergyLine(scaledImg);
    line = mapLineToOriginalSize(line, ISize(img.cols, img.rows), ISize(scaledImg.cols, scaledImg.rows));
    highlightLine(img, line, RgbColor(255, 0, 0));
    img = removeLine(img, line);

    // auto qimg = QImage(
    //     energyImg.data(), energyImg.cols, energyImg.rows, energyImg.cols, QImage::Format_Grayscale8);
    const auto qimg = QImage(
        img.data(), img.cols, img.rows, 3 * img.cols, QImage::Format_RGB888);
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
