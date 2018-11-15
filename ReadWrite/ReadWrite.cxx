// Include the relevant header files
#include "itkMesh.h"
#include "itkRegularSphereMeshSource.h"
#include "itkMeshFileWriter.h"
#include "itkMeshFileReader.h"

// Create convenience typedefs
const unsigned int Dimension = 3;
typedef float TCoordinate;

typedef itk::Mesh< TCoordinate, Dimension > TMesh;
typedef itk::RegularSphereMeshSource< TMesh > TSphere;
typedef itk::MeshFileReader< TMesh > TReader;
typedef itk::MeshFileWriter< TMesh > TWriter;

int main(int argc, char * argv[] )
{

  // The user can choose to provide a commandline argument
  // between 0 and 8, in order to specify the file type.
  // If no argument is provided, zero will be used.
  // If an invalid integer is provided, zero will be used.
  // If the integer is less than zero, zero will be used.
  // If the integer is greater than seven, seven will be used.
  int i = 0;
  //if (argc == 2) i = atoi(argv[1]);
  if (argc != 3) {
      std::cerr << "Wrong number of arguments\n" << std::endl;
      return EXIT_FAILURE;
  }

  // Create a string using the appropriate file extension.
  std::string filename_in = argv[1];
  std::string filename_out = argv[2];

  // Print out the file name.
  std::cout << "Input File Name: " << filename_in << std::endl;
  std::cout << "Output File Name: " << filename_out << std::endl;

  // Create the sphere source, writer, and reader.
  TWriter::Pointer writer = TWriter::New();
  TReader::Pointer reader = TReader::New();

  // Set the reader's file name, then update.
  reader->SetFileName( filename_in );
  reader->Update();

  // Set the writer's input and file name, then update.
  writer->SetInput( reader->GetOutput() );
  writer->SetFileName( filename_out );
  writer->Update();


  return EXIT_SUCCESS;

}
