
#include <iostream>

#include <vtkm/io/VTKDataSetReader.h>

#include "CinemaExporter.h"

int main(int argc, char** argv) {

  std::cout<<"Get Global Bounds"<<std::endl;
  vtkm::Bounds bounds(
    vtkm::Range(0,1),
    vtkm::Range(0,1),
    vtkm::Range(0,1)
  );

  // for each contour (here dummy)
  for(int contourIndex=0; contourIndex<3; contourIndex++){
    std::string inPath = "../top-contours-input-output/output/output.contour."+std::to_string(contourIndex)+".vtk";
    std::cout<<"============================================= "<<std::endl;
    std::cout<<"Reading "<<inPath<<std::endl;

    vtkm::io::VTKDataSetReader reader(inPath);
    auto dataSet = reader.ReadDataSet();

    CinemaExporter::RenderAndWrite(
      contourIndex, // int contourID
      dataSet, // vtkm::cont::DataSet& dataSet
      bounds, // vtkm::Bounds& bounds
      100, 100, // int resX, int resY,
      0, 270, 90, // int elevation, int elevation1, int elevationStep,
      -60, 60, 60 // int azimuth, int azimuth1, int azimuthStep
    );
  }

  return 0;
}
