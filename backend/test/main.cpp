#include <cstdio>
#include <iostream>

#include <qelapsedtimer.h>
#include <qimage.h>

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

    std::cout << "Input the number of lines to remove:\n";
    int linesToRemove = 0;
    std::cin >> linesToRemove;

    if (linesToRemove >= img.cols)
    {
        printf("Illegal lines to remove.\n");
        exit(2);
    }

    QElapsedTimer et;
    et.start();
    img = removeMinimumEnergyLines(img, linesToRemove);
    printf("Elapsed: %lldms\n", et.elapsed());

    const auto qimg = QImage(img.data(), img.cols, img.rows, 3 * img.cols, QImage::Format_RGB888);
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
