// Importing libraries
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include "stdlib.h"
#include "CImg.h"


// declaring constants
#define TOTAL_IMAGE 300 
#define TOTAL_CLASS 3 
#define EPOCHS 10

using namespace cimg_library; 

typedef struct Feature Feature;
typedef CImg<unsigned char> Image; 

// Feature struct
struct Feature
{
    std::string name;
    float mean[3];
    float histogram[3][256];
    int label;
};

int dataLoad(char *list, Image *img_list, Feature *featureVectors)
{
    std::map<int, std::string> class_dict;
    class_dict[0] = "flower";
    class_dict[1] = "cat";
    class_dict[2] = "car";

    int index = 0;
    char buffer[200];
    std::fstream fin;
    fin.open(list, std::ios::in);
    while (fin.getline(buffer, sizeof(buffer)))
    {
        img_list[index].load(buffer);
        featureVectors[index].name = buffer;

        for (int _class = 0; _class<TOTAL_CLASS; _class++)
        {
            if (featureVectors[index].name.find(class_dict[_class], 11)!= std::string::npos)
            {
                featureVectors[index].label = _class;
                break;
            }
        }
        index++;
    }    
    return 0;
}

int extractFeatures(Image *imgList, Feature *featureVectors, int tot_image)
{
    CImg<float> temp;
    float area;

    for (int i=0; i < tot_image; i++)
    {
        // finding area of each image
        area = imgList[i].width() * imgList[i].height();
        
        // for each image each pixel histogram feature is got and then normalized
        for (int chl = 0; chl < 3; chl++)
        {
            featureVectors[i].mean[chl] = imgList[i].get_channel(chl).mean() /256;
            temp = imgList[i].get_channel(chl).get_histogram(256, 0, 255);

            for (int scale= 0; scale<256; scale++)
            {
                featureVectors[i].histogram[chl][scale] = temp(scale, 0, 0, 0)/area;

            }
        }

    }
    return 0; 

}

int initialize(Feature *arr_feature, std::vector<Feature> *classes)
{
    int current_class = 0, idx = 0;


    while (current_class < TOTAL_CLASS)
    {
        if (arr_feature[idx].label == current_class)
        {
            classes[current_class].push_back(arr_feature[idx]);
            current_class++;
        }
        idx++;
    }

    return 0; 
}

float Euclidean_dist(Feature x1, Feature x2)
{
    float result = 0;

    for(int channel=0; channel<3; channel++)
    {
        result += std::pow((x1.mean[channel] - x2.mean[channel]), 2);
        for (int scale=0; scale<256; scale++)
        {
            result += std::pow((x1.histogram[channel][scale] - x2.histogram[channel][scale]),2);

        }
    }

    return std::sqrt(result);
}

void get_result(std::vector<Feature> *classes)
{
    int confusion_matrix[TOTAL_CLASS][TOTAL_CLASS] = {0};
    int true_class;

    // generating a confusion matrix and getting result
    for (int _class = 0; _class<TOTAL_CLASS; _class++)
    {

        for (int i=1; i<classes[_class].size(); i++)
        {
            true_class = classes[_class][i].label;
            confusion_matrix[_class][true_class]++;
        }

    }

    float accuracy = 0;
    for (int _class = 0; _class < TOTAL_CLASS; _class++)
    {
        accuracy += confusion_matrix[_class][_class];

    }

    accuracy /= TOTAL_IMAGE;

    std::cout<<"Accuracy "<<accuracy<<std::endl;


    
}

// updating 
void update(std::vector<Feature> *classes)
{
    for (int _class = 0; _class < TOTAL_CLASS; _class++)
    {
        Feature temp = {"", {0},{0}, _class};
        int num_element = classes[_class].size();

        for (int i =1; i < num_element; i++)
        {
            for (int channel=0; channel<3; channel++)
            {
                temp.mean[channel] += classes[_class][i].mean[channel]/num_element;
                for (int scale =0; scale<256; scale++)
                {
                    temp.histogram[channel][scale] += classes[_class][i].histogram[channel][scale]/num_element;

                }
            }
        }
        classes[_class][0] =  temp;
        classes[_class].erase(classes[_class].begin()+1, classes[_class].end());
    }
} 


int k_means_clustering(Feature *arr_feature, std::vector<Feature> *classes)
{
    float distance;
    float temp;
    int min_class;

    for (int epoch = 0; epoch<EPOCHS; epoch++)
    {

        for (int idx=0; idx<TOTAL_IMAGE; idx++)
        {
            distance = 999999;

            for (int _class=0; _class<TOTAL_CLASS; _class++)
            {   

                temp = Euclidean_dist(arr_feature[idx], classes[_class][0]);
                //std::cout<<"idx - "<<idx << "classes - "<<_class <<" Temp- "<<temp<<std::endl;
                if (temp < distance)
                {
                    distance = temp;
                    min_class = _class;
                }
            }
            // to the class of minimum distance correspoding arr feature is pushed
            classes[min_class].push_back(arr_feature[idx]);

        }

        std::cout << " Epoch: " << epoch << std::endl;

        get_result(classes);

        if (epoch == EPOCHS -1)
            break;
        update(classes); 
    }

    return 0;

}


int main()
{
    int out;
    Image srcImage[TOTAL_IMAGE];
    Feature featureVectorsImg[TOTAL_IMAGE];
    std::vector<Feature> classes[TOTAL_CLASS];
    char *path = "list_test.txt";
    
    out = dataLoad(path,srcImage,featureVectorsImg);
    std::cout<<"Data Loading Done"<<std::endl;

    out = extractFeatures(srcImage, featureVectorsImg,TOTAL_IMAGE);
    std::cout<<"Feature extraction Done"<<std::endl;
    out = initialize(featureVectorsImg, classes);
    std::cout<<"Initalizing features Done"<<std::endl;

    std::random_shuffle(featureVectorsImg, featureVectorsImg+TOTAL_IMAGE);

    out = k_means_clustering(featureVectorsImg, classes);
    std::cout<<" Completed "<<std::endl;
    

    return 0;
  

}
