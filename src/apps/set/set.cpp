#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <csm.h>
#include <Isd.h>

#include <gdal/gdal.h>
#include <gdal/gdal_priv.h>
#include <gdal/cpl_conv.h>
#include <gdal/cpl_string.h>

#include <IsdReader.h>
#include <MdisPlugin.h>
#include <MdisNacSensorModel.h>


using namespace std;

void cubeArray(vector<vector<float> > *cube, GDALRasterBand *poBand);

int main(int argc,char *argv[]) {

  csm::Isd *isd = readISD("../../../tests/data/EN1007907102M.json");

  // Create the plugin 
  MdisPlugin plugin;
  MdisNacSensorModel *model;
  
   // Initialize the MdisNacSensorModel from the ISD using the plugin
  try {
    model = dynamic_cast<MdisNacSensorModel*>
            (plugin.constructModelFromISD(*isd, "ISIS_MDISNAC_USGSAstro_1_Linux64_csm30.so"));
  }
  catch (csm::Error &e) {
    cout << e.what() << endl;
    exit (EXIT_FAILURE);
  }

  if (model == nullptr) {
     cout << "Could not construct the sensor model from the plugin." << endl;
     return 0;
  }
  
  // Test the model's accuracy by visually comparing the results of the function calls
  csm::EcefCoord groundPoint;
  csm::ImageCoord imagePoint;

  for (int i = 0; i < 10; i++) {
    groundPoint = model->imageToGround(imagePoint, 0);

    std::cout << "Image Point (s, l) : (" << imagePoint.samp << ", " << imagePoint.line << "); "
              << "Ground Point (x, y, z) : (" << groundPoint.x << ", " << groundPoint.y << ", " 
                                              << groundPoint.z << ")" << endl;

    //imagePoint = model->groundToImage(groundPoint);
  }

  //Read from a cube using the GDAL API, and outputs the DN values to a 2D vector matrix
  string cubePath("../../../tests/data/CN0108840044M_IF_5_NAC_spiced.cub");
  GDALDataset *poDataset;
  GDALRasterBand *poBand;
  int nBlockXSize,nBlockYSize;

  cout << endl;
  cout << "Testing GDAL"  << endl;
  cout << endl;
  GDALAllRegister();

  poDataset = (GDALDataset *)GDALOpen(cubePath.c_str(),GA_ReadOnly);

  if (poDataset == NULL) {

    cout << "Could not open the:  " + cubePath  << endl;

  }

  else {

    poBand = poDataset->GetRasterBand(1);
    poBand->GetBlockSize(&nBlockXSize,&nBlockYSize);

    cout << "Num samples = " << nBlockXSize << endl;
    cout << "Num lines = " <<   nBlockYSize << endl;

  //Read a band of data

    vector< vector<float> > cubeMatrix;
    cubeArray(&cubeMatrix,poBand);

  //Output the values (or not if you don't want to see a long list of numbers)
#if 0

    for (int i =0; i < cubeMatrix.size(); i++ ) {

      vector<float> v = cubeMatrix[i];
      for (int j = 0; j < v.size(); j++)
        if (j== v.size()-1)
             cout <<"****** Line:  "<< i <<" ******" << endl;
        else
              cout <<" " << v[j] << " ";

    }
#endif

  }  //end else
 
  exit (EXIT_SUCCESS);
}


void cubeArray(vector <vector<float> > *cube,GDALRasterBand *poBand) {

  vector<float> tempVector;
  float *pafScanline;
  int nsamps = poBand->GetXSize();
  int nlines = poBand->GetYSize();
  for (int j = 0;j<nlines;j++) {

    pafScanline = (float *)CPLMalloc(sizeof(float)*nsamps);
    poBand->RasterIO(GF_Read,0,j,nsamps,1,pafScanline,nsamps,1,GDT_Float32,0,0);

    for (int i = 0;i < nsamps;i++) {
      tempVector.push_back(pafScanline[i]);
    }
    cube->push_back(tempVector);
    tempVector.clear();
    CPLFree(pafScanline);
  }
}
