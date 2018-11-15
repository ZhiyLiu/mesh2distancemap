#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkImageData.h>
#include <vtkSphereSource.h>
#include <vtkMetaImageWriter.h>
#include <vtkPolyDataToImageStencil.h>
#include <vtkImageStencil.h>
#include <vtkPointData.h>
#include <vtkGenericDataObjectReader.h>

/**
 * This program generates a sphere (closed surface, vtkPolyData) and converts it into volume
 * representation (vtkImageData) where the foreground voxels are 1 and the background voxels are
 * 0. Internally vtkPolyDataToImageStencil is utilized. The resultant image is saved to disk 
 * in metaimage file format (SphereVolume.mhd).
 */
int main(int argc, char ** argv)
{  

    if(argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " InputFilename" << endl;
        return EXIT_FAILURE;
    }

    // Ensure a filename was specified
    std::string outputFilename = "new.mhd";
    if(argc > 2)
    {
        outputFilename = argv[2];
    }
    double voxel_spacing = 0.5;
    if(argc > 3)
    {
        voxel_spacing = atof( argv[3] ); 
    }

    // Get the filename from the command line
    std::string inputFilename = argv[1];

    // Get all data from the file
    vtkSmartPointer<vtkGenericDataObjectReader> reader = vtkSmartPointer<vtkGenericDataObjectReader>::New();
    reader->SetFileName(inputFilename.c_str());
    reader->Update();


    vtkPolyData* inputData = reader->GetPolyDataOutput();
    double bounds[6];
    if ( argc == 4 ) {
        std::cerr << "bounds automatically determined\n";
        inputData->GetBounds(bounds);
    }
    else {
        if( argc > 4 )
            bounds[0] = atof( argv[4] );
        if( argc > 5 )
            bounds[1] = atof( argv[5] );
        if( argc > 6 )
            bounds[2] = atof( argv[6] );
        if( argc > 7 )
            bounds[3] = atof( argv[7] );
        if( argc > 8 )
            bounds[4] = atof( argv[8] );
        if( argc > 9 )
            bounds[5] = atof( argv[9] );
        if(argc > 10 )
        {
            std::cerr << "Too many arguments" << std::endl;
            return EXIT_FAILURE;
        }
    }
    double image_bounds[6];
    inputData->GetBounds(image_bounds);

    if( image_bounds[0] < bounds[0] ) {
        std::cerr << "Minimum x coordinate is less than deteremined bound\n";
        std::cerr << "Actual: " << image_bounds[0] << std::endl;
        std::cerr << "Input : " << bounds[0] << std::endl;
    }
    if( image_bounds[2] < bounds[2] ) {
        std::cerr << "Minimum y coordinate is less than deteremined bound\n";
        std::cerr << "Actual: " << image_bounds[2] << std::endl;
        std::cerr << "Input : " << bounds[2] << std::endl;
    }
    if( image_bounds[4] < bounds[4] ) {
        std::cerr << "Minimum z coordinate is less than deteremined bound\n";
        std::cerr << "Actual: " << image_bounds[4] << std::endl;
        std::cerr << "Input : " << bounds[4] << std::endl;
    }

    
    if( image_bounds[1] > bounds[1] ) {
        std::cerr << "Maximum x coordinate is greater than deteremined bound\n";
        std::cerr << "Actual: " << image_bounds[1] << std::endl;
        std::cerr << "Input : " << bounds[1] << std::endl;
    }
    if( image_bounds[3] > bounds[3] ) {
        std::cerr << "Maximum y coordinate is greater than deteremined bound\n";
        std::cerr << "Actual: " << image_bounds[3] << std::endl;
        std::cerr << "Input : " << bounds[3] << std::endl;
    }
    if( image_bounds[5] > bounds[5] ) {
        std::cerr << "Maximum z coordinate is greater than deteremined bound\n";
        std::cerr << "Actual: " << image_bounds[5] << std::endl;
        std::cerr << "Input : " << bounds[5] << std::endl;
    }

    vtkSmartPointer<vtkImageData> whiteImage = vtkSmartPointer<vtkImageData>::New();

    double spacing[3]; // desired volume spacing

    spacing[0] = voxel_spacing;
    spacing[1] = voxel_spacing;
    spacing[2] = voxel_spacing;
    whiteImage->SetSpacing(spacing);

    // compute dimensions
    int dim[3];
    for (int i = 0; i < 3; i++)
    {
        dim[i] = static_cast<int>(ceil( (bounds[i * 2 + 1] - bounds[i * 2]) / spacing[i]));
    }
    // DEBUG
    std::cout << "dim[0]: " << dim[0] << std::endl;
    std::cout << "dim[1]: " << dim[1] << std::endl;
    std::cout << "dim[2]: " << dim[2] << std::endl;
    whiteImage->SetDimensions(dim);
    whiteImage->SetExtent(0, dim[0] - 1, 0, dim[1] - 1, 0, dim[2] - 1);

    double origin[3];
    origin[0] = 0;//bounds[0] + spacing[0]/2;//0.5 * (bounds[0] + bounds[1]);
    origin[1] = 0;//bounds[2] + spacing[1]/2;//0.5 * (bounds[3] + bounds[2]);
    origin[2] = 0;//bounds[4] + spacing[2]/2;//0.5 * (bounds[5] + bounds[4]);

    whiteImage->SetOrigin(origin);

#if VTK_MAJOR_VERSION <= 5
    whiteImage->SetScalarTypeToUnsignedChar();
    whiteImage->AllocateScalars();
#else
    whiteImage->AllocateScalars(VTK_UNSIGNED_CHAR,1);
#endif
    // fill the image with foreground voxels:
    unsigned char inval = 1;
    unsigned char outval = 0;
    vtkIdType count = whiteImage->GetNumberOfPoints();
    std::cout<<"Number of points: "<<count<<endl;
    for (vtkIdType i = 0; i < count; ++i)
    {
        whiteImage->GetPointData()->GetScalars()->SetTuple1(i, inval);
    }

    // polygonal data --> image stencil:
    vtkSmartPointer<vtkPolyDataToImageStencil> pol2stenc = vtkSmartPointer<vtkPolyDataToImageStencil>::New();
#if VTK_MAJOR_VERSION <= 5
    pol2stenc->SetInput(inputData);
#else
    pol2stenc->SetInputData(inputData);
#endif
    pol2stenc->SetOutputOrigin(origin);
    pol2stenc->SetOutputSpacing(spacing);
    pol2stenc->SetOutputWholeExtent(whiteImage->GetExtent());
    pol2stenc->Update();

    // cut the corresponding white image and set the background:
    vtkSmartPointer<vtkImageStencil> imgstenc =
        vtkSmartPointer<vtkImageStencil>::New();
#if VTK_MAJOR_VERSION <= 5
    imgstenc->SetInput(whiteImage);
    imgstenc->SetStencil(pol2stenc->GetOutput());
#else
    imgstenc->SetInputData(whiteImage);
    imgstenc->SetStencilConnection(pol2stenc->GetOutputPort());
#endif
    imgstenc->ReverseStencilOff();
    imgstenc->SetBackgroundValue(outval);
    imgstenc->Update();

    vtkSmartPointer<vtkMetaImageWriter> writer =
        vtkSmartPointer<vtkMetaImageWriter>::New();
    writer->SetFileName(outputFilename.c_str());
#if VTK_MAJOR_VERSION <= 5
    writer->SetInput(imgstenc->GetOutput());
#else
    writer->SetInputData(imgstenc->GetOutput());
#endif
    writer->SetCompression(true);
    writer->Write();

    return EXIT_SUCCESS;
}
