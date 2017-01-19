#include "MdisPlugin.h"
#include "MdisNacSensorModel.h"

#include <Isd.h>
#include <EcefCoord.h>
#include <ImageCoord.h>

#include <iostream>

using namespace std;

int main() {
  
  // Create and load in the ISD object from an ISD file
  csm::Isd isd;
  
  // Create the plugin
  MdisPlugin plugin;
  
  // Create the Sensor model from the ISD object using the plugin
  MdisNacSensorModel model = 
    plugin.constructModelFromISD(isd, "ISIS_MDISNAC_USGSAstro_1_Linux64_csm30.so");
  
  // Test the model's accuracy by visually comparing the results of the function calls
  csm::EcefCoord groundPoint;
  csm::ImageCoord imagePoint;
  
  imagePoint.samp = 300;
  imagePoint.line = 400;
  
  for (int i = 0; i < 10; i++) {
    groundPoint = model.imageToGround(imagePoint, 0);
    
    std::cout << "Image Point (s, l) : (" << imagePoint.samp << ", " << imagePoint.line << "); "
              << "Ground Point (x, y, z) : (" << groundPoint.x << ", " << groundPoint.y << ", " 
                                              << groundPoint.z << ")" << endl;
    
    imagePoint = model.groundToImage(groundPoint);
  }
  
  return 0;
}