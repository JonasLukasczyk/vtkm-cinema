
#include <iostream>

#include <vtkm/io/VTKDataSetReader.h>
#include <vtkm/rendering/ScalarRenderer.h>

#include "VTKDataSetWriter2.h"

// This method adds a global field to the dataset with a given underlying data type, name, and data
template<typename T>
void addGlobalField(
  vtkm::cont::DataSet& dataSet,
  std::string name,
  std::vector<T> vector
){
  dataSet.AddField(
    vtkm::cont::Field(
      name,
      vtkm::cont::Field::Association::WHOLE_MESH,
      vtkm::cont::make_ArrayHandle<T>(vector,vtkm::CopyFlag::On)
    )
  );
}


// This method returns a filename for a dataset by concatenating all 1-component field data values
std::string getFileName(
  vtkm::cont::DataSet& dataSet
){
  std::string name = "";
  for (vtkm::Id f = 0; f < dataSet.GetNumberOfFields(); f++)
  {
    auto field = dataSet.GetField(f);
    if (!field.IsFieldGlobal())
      continue;

    auto data = field.GetData();

    vtkm::IdComponent nComponents = data.GetNumberOfComponentsFlat();

    if(nComponents==1){
      if(data.IsBaseComponentType<vtkm::Float32>()){
        auto componentArray = data.ExtractArrayFromComponents<vtkm::Float32>();
        auto portal = componentArray.ReadPortal();
        name += std::to_string(portal.Get(0));
      } else {
        auto componentArray = data.ExtractArrayFromComponents<vtkm::Int32>();
        auto portal = componentArray.ReadPortal();
        name += std::to_string(portal.Get(0));
      }
      name += "_";
    }
  }

  if(name.length()<1)
    name="FILE.";
  else
    name[name.length()-1] = '.';

  // name = name.substr(0, name.length()-1);
  name += "vtk";

  return name;
}

int main(int argc, char** argv) {

  std::cout<<"Get Global Bounds"<<std::endl;
  vtkm::Bounds bounds(
    vtkm::Range(0,1),
    vtkm::Range(0,1),
    vtkm::Range(0,1)
  );

  // init camera with global bounds
  vtkm::rendering::Camera camera;
  camera.ResetToBounds(bounds);

  // init renderer

  // for each contour (here dummy)
  for(int contourIndex=0; contourIndex<3; contourIndex++){
    std::string inPath = "../top-contours-input-output/output/output.contour."+std::to_string(contourIndex)+".vtk";
    std::cout<<"============================================= "<<std::endl;
    std::cout<<"Reading "<<inPath<<std::endl;

    vtkm::io::VTKDataSetReader reader(inPath);
    auto dataset = reader.ReadDataSet();

    vtkm::rendering::ScalarRenderer renderer;
    renderer.SetInput(dataset);
    renderer.SetDefaultValue(1.0);
    renderer.SetWidth(10);
    renderer.SetHeight(10);

    for(int elevation=-60; elevation<=60; elevation+=60){
      camera.Elevation((vtkm::Float32)elevation);
      for(int azimuth=0; azimuth<360; azimuth+=45){
        camera.Azimuth((vtkm::Float32)azimuth);

        // render
        std::cout<<"Rendering "<<elevation<<" "<<azimuth<<std::endl;
        vtkm::rendering::ScalarRenderer::Result res = renderer.Render(camera);

        vtkm::cont::DataSet result = res.ToDataSet();
        addGlobalField<vtkm::Int32>(result, "CountourID", {contourIndex});
        addGlobalField<vtkm::Int32>(result, "CamAzimuth", {azimuth});
        addGlobalField<vtkm::Int32>(result, "CamElevation", {elevation});

        addGlobalField<vtkm::Vec3f_32>(result, "CamPosition", {camera.GetPosition()});
        addGlobalField<vtkm::Vec3f_32>(result, "CamUp", {camera.GetViewUp()});
        addGlobalField<vtkm::Vec3f_32>(result, "CamDirection", {(camera.GetLookAt()-camera.GetPosition())});

        auto outPath = getFileName(result);
        std::cout<<"Writing "<<outPath<<std::endl;
        VTKDataSetWriter2 writer("./"+outPath);
        writer.WriteDataSet(result);
      }
    }
  }

  return 0;
}
