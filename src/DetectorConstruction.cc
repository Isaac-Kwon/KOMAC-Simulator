#include "DetectorConstruction.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4MaterialTable.hh"
#include "G4NistManager.hh"


#include "G4VSolid.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"


#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4PVReplica.hh"

#include "G4Transform3D.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"

#include "G4UserLimits.hh"
#include "G4AssemblyVolume.hh"

#include "G4RunManager.hh"
#include "G4GenericMessenger.hh"

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4ios.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction(),
    fScoringVolume0(0),
    fScoringVolume1(0),
    fScoringVolume2(0)
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume * DetectorConstruction::Construct()
{
  // Construct basis(Material, Color set)
  ConstructMaterials();
  ConstructColors();

  // Option to switch on/off checking of volumes overlaps
  G4bool checkOverlaps = true;

  // Geometry
  // experimental hall (world volume)
  G4Material* air = G4Material::GetMaterial("G4_AIR");
  // G4Material* air = G4Material::GetMaterial("G4_Galactic"); // For QA


  G4VSolid* worldSolid
    = new G4Box("worldBox", 1.*m, 4.*m, 2.*m);
    // = new G4Sphere("worldSphere", 0, 5.*m, 0, CLHEP::twopi, 0, CLHEP::pi);
  G4LogicalVolume* worldLogical
    = new G4LogicalVolume(worldSolid,air,"worldLogical");
  G4VPhysicalVolume* worldPhysical
    = new G4PVPlacement(0,G4ThreeVector(),worldLogical,"worldPhysical",0,false,0,checkOverlaps);

  worldLogical -> SetVisAttributes(world_color);

  // Experimental setup
  // Get the material
  G4Material * aluminum = G4Material::GetMaterial("G4_Al");
  G4Material * gold = G4Material::GetMaterial("G4_Au");
  G4Material * silicon = G4Material::GetMaterial("G4_Si");
  G4Material * mylar = G4Material::GetMaterial("G4_MYLAR");

  // Beam window
  G4Tubs * beamWindow = new G4Tubs("beamWindow",    // Name
                                    0*mm,           // radius min
                                    100.0*mm,         // radius max
                                    1.*mm,        // Thickness/2
                                    0,              // Start angle
                                    360);           // End angle  
  G4LogicalVolume* beamWindowLogical = new G4LogicalVolume(beamWindow, air, "beamWindowLogical");
  new G4PVPlacement(0,                                  // No rotation
                    G4ThreeVector(0.*m,0.*m,-1.42*m),   // at (0,0,-1.42m)
                    beamWindowLogical,                  // its logical volume
                    "beamWindowPhysical",               // its name
                    worldLogical,                       // its mother volume
                    false,                              // no boolean operations
                    0,                                  // copy number
                    checkOverlaps);                     // Checking overlaps
  beamWindowLogical->SetVisAttributes(gray);
  // 1st Collimator
  G4AssemblyVolume * CollimatorAssembly = Collimator(aluminum, // Collimator material
                                                     120.0 *mm, 120.0 *mm, 5.0 *mm, 25.0*mm); // x, y, z size, hole radius
  G4RotationMatrix * Ra = new G4RotationMatrix(0.*deg, 0.*deg, 0.*deg);
  G4ThreeVector Ta1 = G4ThreeVector(0.*m, 0.*m, -0.57 *m);
  CollimatorAssembly->MakeImprint(worldLogical, Ta1, Ra);

  // 2nd Collimator with gold film
  G4AssemblyVolume * CollimatorFilmAssembly = FoilWithCollimator(aluminum, // Collimator material
                                                                 120.0 *mm, 120.0 *mm, 5.0 *mm, 25.0*mm, // x, y, z size, hole radius
                                                                 gold, 0.1 *mm); // Foil material, Foil thickness
  G4ThreeVector Ta2 = G4ThreeVector(0.*m, 0.*m, -0.52 *m);
  CollimatorFilmAssembly -> MakeImprint(worldLogical, Ta2, Ra); // NOTE: Comment out this line for QA

  // Beam profile detector at 2nd collimator hole
  G4VSolid * beamProfileSolid1 = new G4Tubs("beamProfileSolid1",
                                            0.0 *mm,      //inner radius
                                            25.0*mm,      //outer radius
                                            2.5*mm,       //tub's height
                                            0.0 *deg,     //start angle
                                            360.0 *deg);  //end angle
  G4LogicalVolume* beamProfileLogical1 = new G4LogicalVolume(beamProfileSolid1, air, "beamProfileLogical1");
  new G4PVPlacement(0,                                  // No rotation
                    Ta2,                                // at (0,0,-0.52m)
                    beamProfileLogical1,                  // its logical volume
                    "beamProfilePhysical1",               // its name
                    worldLogical,                       // its mother volume
                    false,                              // no boolean operations
                    0,                                  // copy number
                    checkOverlaps);                     // Checking overlaps
  beamProfileLogical1->SetVisAttributes(gray);
  fScoringVolume2 = beamProfileLogical1;

  // Shielding plate
  G4AssemblyVolume * ShieldingAssembly = Shielding(aluminum, 69.5 *mm, 140.*mm, 10. *mm, true, false);
  G4ThreeVector Ta3 = G4ThreeVector(-60. * mm, -60. *mm, -0.50 *m);
  ShieldingAssembly -> MakeImprint(worldLogical, Ta3, Ra);

  // Mount
  G4AssemblyVolume * MountAssembly = DetectorAndMount(aluminum, // Mount material
                                                      302.*mm, 150.*mm, 137.*mm, // Mount size
                                                      6.*mm,                  // Mount thickness
                                                      20.*mm, 36.*mm,         // Window Size
                                                      76.*mm, 0.*mm,          // Window position based mount center
                                                      silicon,                // Detector material
                                                      20.*mm, 36.*mm, 0.1*mm, // Detector size, NOTE: Change thickness from 0.1mm to 3mm for QA
                                                      25.*mm,                 // Detector depth from surface
                                                      mylar, 6.*mm);          // Window material, window thickness FIXME: Window thickness not needed(?)
  G4ThreeVector Ta4 = G4ThreeVector(-7.*cm,0.*m,0.*m);
  MountAssembly -> MakeImprint(worldLogical, Ta4, Ra);

  // Beam profile detector at mount window
  G4VSolid *beamProfileSolid2 = new G4Box("beamProfileSolid2",
                                         20/2*mm,
                                         36/2*mm,
                                         0.5*mm);  // Window solid, use it in subtraction to make a front side
  G4LogicalVolume* beamProfileLogical2 = new G4LogicalVolume(beamProfileSolid2, air, "beamProfileLogical2");
  new G4PVPlacement(0,                                      // No rotation
                    G4ThreeVector(-7.*cm, 0.*cm, -0.5*mm),  // at (-7cm, 0, -0.5mm)
                    beamProfileLogical2,                    // its logical volume
                    "beamProfilePhysical2",                 // its name
                    worldLogical,                           // its mother volume
                    false,                                  // no boolean operations
                    0,                                      // copy number
                    checkOverlaps);                         // Checking overlaps
  beamProfileLogical2->SetVisAttributes(gray);
  fScoringVolume1 = beamProfileLogical2;


  return worldPhysical;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
// General coordination info
// Z: beam direction
// Y: Height
// XY: Transverse plane / XZ: ground plane

G4AssemblyVolume * DetectorConstruction::Collimator(G4Material* material, G4double Xsize, G4double Ysize, G4double Zsize, G4double Radius)
{

  G4VSolid * boxSolid = new G4Box("Collimator_Box_Solid", Xsize/2, Ysize/2, Zsize/2);
  G4double holeDepth;
  holeDepth = (Xsize + Ysize + Zsize);

  G4double YDeg = 0.0;

  G4VSolid * holeSolid = new G4Tubs("HoleFirstCollimator",
                                    0.0 *m,      //inner radius
                                    Radius,      //outer radius
                                    holeDepth,   //tub's height
                                    0.0 *deg,    //start angle
                                    360.0 *deg); //end angle

  G4RotationMatrix * Ra = new G4RotationMatrix(90.0 *deg, YDeg, 90.0 *deg);
  G4ThreeVector Ta = G4ThreeVector(0.0 *m, 0.0 *m, 0.0 *m);

  G4VSolid * CollimatorSolid = new G4SubtractionSolid("Collimater_Solid", boxSolid, holeSolid, Ra, Ta);


  G4LogicalVolume * CollimatorLogical = new G4LogicalVolume(CollimatorSolid, material, "CollimatorLogical");
  G4RotationMatrix * Ra0 = new G4RotationMatrix(0.0 *deg, 0.0 *deg, 0.0 *deg);

  G4AssemblyVolume * CollimatorAssembly = new G4AssemblyVolume(CollimatorLogical, Ta, Ra0);

  return CollimatorAssembly;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4AssemblyVolume * DetectorConstruction::FoilWithCollimator(G4Material * material,
                                                            G4double Xsize, G4double Ysize, G4double Zsize, G4double Radius,
                                                            G4Material * material_film, G4double thickness)
{

  G4AssemblyVolume * CollimatorAssembly = Collimator(material, Xsize, Ysize, Zsize, Radius);

  G4double filmSizeX;
  G4double filmSizeY = Ysize;

  G4double filmPositionX, filmPositionY, filmPositionZ;
  G4double filmRotationX, filmRotationY, filmRotationZ;

  filmRotationX = 90.0*deg;
  filmRotationY = 90.0*deg;
  filmRotationZ = 90.0*deg;

  filmPositionX = 0.0 * m;
  filmPositionY = 0.0 * m;
  filmPositionZ = 0.0 * m;

  filmSizeX = Xsize;
  filmRotationY = 0.0 *deg;
  filmPositionZ = Zsize + thickness;

  G4VSolid * filmSolid = new G4Box("Film_Solid", filmSizeX/2, filmSizeY/2, thickness/2);
  G4LogicalVolume * filmLogical = new G4LogicalVolume(filmSolid, material_film, "Film_Logical");
  filmLogical -> SetVisAttributes(foil_color);

  G4RotationMatrix * Ra = new G4RotationMatrix(filmRotationX, filmRotationY, filmRotationZ);
  G4ThreeVector Ta = G4ThreeVector(filmPositionX, filmPositionY, filmPositionZ);

  CollimatorAssembly->AddPlacedVolume(filmLogical, Ta, Ra);

  return CollimatorAssembly;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4AssemblyVolume * DetectorConstruction::Absorber() // NOTE: Legacy parts. Use when needed the absorber geometry
{
  G4AssemblyVolume * Absorber = new G4AssemblyVolume();
  return Absorber;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4AssemblyVolume * DetectorConstruction::Mount(G4Material * mountMaterial,
                                               G4double mountSizeX, G4double mountSizeY, G4double mountSizeZ,
                                               G4double thickness,
                                               G4double mountWindowSizeX, G4double mountWindowSizeY, // Window Size
                                               G4double mountWindowPositionX, G4double mountWindowPositionY, // Window Position from center of side
                                               G4Material * windowMaterial, G4double windowThickness)
{

  G4AssemblyVolume * MountAssembly = new G4AssemblyVolume(); // Initalize

  G4RotationMatrix* Ra = new G4RotationMatrix(0.*deg, 0.*deg, 0.*deg); // Default rotation: No rotation.

  // Top & Bottom plate
  G4VSolid * TBPlateSolid = new G4Box("TBPlateSolid", mountSizeX/2 - thickness, thickness/2, mountSizeZ/2 - thickness);

  G4LogicalVolume * TBPlateLogical = new G4LogicalVolume(TBPlateSolid, mountMaterial, "TBPlateLogical", 0, 0, 0);
  TBPlateLogical -> SetVisAttributes(mount_color);

  G4ThreeVector TopVector = G4ThreeVector(mountWindowPositionX,
                                          (mountSizeY-thickness)/2 - mountWindowPositionY,
                                          mountSizeZ/2);
  G4ThreeVector BottomVector = G4ThreeVector(mountWindowPositionX,
                                             -(mountSizeY-thickness)/2 - mountWindowPositionY,
                                             mountSizeZ/2);

  MountAssembly->AddPlacedVolume(TBPlateLogical, TopVector, Ra); // Place the top plate.
  MountAssembly->AddPlacedVolume(TBPlateLogical, BottomVector, Ra); // Place the bottom plate.

  // The plate for back and front
  G4VSolid * ZSidePlateSolid = new G4Box("ZSidePlateSolid", mountSizeX/2, mountSizeY/2, thickness/2);
  G4LogicalVolume * ZSidePlateLogical = new G4LogicalVolume(ZSidePlateSolid, mountMaterial, "ZSidePlateLogical", 0, 0, 0); // Logical volume for back.
  ZSidePlateLogical->SetVisAttributes(mount_color);

  // Subtract two volume to make mount window in front plate.
  G4VSolid *mountWindowSolid = new G4Box("mountWindowSolid",
                                         mountWindowSizeX/2,
                                         mountWindowSizeY/2,
                                         thickness);  // Window solid, use it in subtraction to make a front side
  G4ThreeVector windowPositionVector = G4ThreeVector(-mountWindowPositionX, -mountWindowPositionY, 0); // Naming follows the 3D CAD notation, but geant4 follows the beam axis as z direction.

  G4VSolid * windowSidePlateSolid = new G4SubtractionSolid("mountWindowSizePlateSolid", ZSidePlateSolid, mountWindowSolid, Ra, windowPositionVector); // Define the front plate including a window.
  G4LogicalVolume * windowSidePlateLogical = new G4LogicalVolume(windowSidePlateSolid, mountMaterial, "windowSidePlateLogical", 0, 0, 0);
  windowSidePlateLogical->SetVisAttributes(mount_color);

  G4ThreeVector FrontVector = G4ThreeVector(mountWindowPositionX,
                                            -mountWindowPositionY,
                                            -(mountSizeZ-thickness)/2 + mountSizeZ/2);
  G4ThreeVector BacksideVector = G4ThreeVector(mountWindowPositionX,
                                               -mountWindowPositionY,
                                               (mountSizeZ/2-thickness/2) + mountSizeZ/2);

  MountAssembly -> AddPlacedVolume(windowSidePlateLogical, FrontVector, Ra); // Place the front plate.
  MountAssembly -> AddPlacedVolume(ZSidePlateLogical, BacksideVector, Ra);   // Place the back plate.

  // The plate for left and right. (left is positive direction in x axis and right is negative direction in x axis.)
  G4VSolid *XSidePlateSolid = new G4Box("XSidePlate", thickness/2, mountSizeY/2, mountSizeZ/2 - thickness); // Left, Rifht cover
  G4LogicalVolume *XSidePlateLogical = new G4LogicalVolume(XSidePlateSolid, mountMaterial, "XSidePlateLogical", 0, 0, 0);
  XSidePlateLogical -> SetVisAttributes(mount_color);

  G4ThreeVector LeftVector = G4ThreeVector((mountSizeX/2-thickness/2) + mountWindowPositionX,
                                           -mountWindowPositionY,
                                           mountSizeZ/2);
  G4ThreeVector RightVector = G4ThreeVector(-(mountSizeX/2-thickness/2) + mountWindowPositionX,
                                            -mountWindowPositionY,
                                            mountSizeZ/2);

  MountAssembly -> AddPlacedVolume(XSidePlateLogical, LeftVector, Ra); // Place the left plate.
  MountAssembly -> AddPlacedVolume(XSidePlateLogical, RightVector, Ra); // Place the right plate.

  // Window cover to shield light from outside.
  // TODO: Separate the input variable. The cover geometry and window geometry is different.
  // FIXME: Separate the cover and mylar film. It's different.
  G4VSolid *mountWindowCoverSolid = new G4Box("mountWindowCoverSolid",
                                              mountWindowSizeX/2,
                                              mountWindowSizeY/2,
                                              windowThickness/2);
  G4LogicalVolume * mountWindowCoverLogical = new G4LogicalVolume(mountWindowCoverSolid, windowMaterial, "mountWindowCoverLogical", 0, 0, 0);
  mountWindowCoverLogical -> SetVisAttributes(mylar_color);
  G4ThreeVector coverVector = G4ThreeVector(0.*m, 0.*m, -windowThickness/2);
  // MountAssembly->AddPlacedVolume(mountWindowCoverLogical, coverVector, Ra); // Place the cover plate. NOTE: Comment out for QA

  return MountAssembly;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4AssemblyVolume * DetectorConstruction::DetectorAndMount(G4Material * mountMaterial,
                                                          G4double mountSizeX, G4double mountSizeY, G4double mountSizeZ,
                                                          G4double thickness,
                                                          G4double mountWindowSizeX, G4double mountWindowSizeY, //Window Size
                                                          G4double mountWindowPositionX, G4double mountWindowPositionY,
                                                          G4Material * detectorMaterial,
                                                          G4double detectorSizeX, G4double detectorSizeY, G4double detectorSizeZ,
                                                          G4double detectorDepth,
                                                          G4Material * windowMaterial, G4double windowThickness)
{

  G4AssemblyVolume * DetectorAndMountAssembly = Mount(mountMaterial,
                                                      mountSizeX, mountSizeY, mountSizeZ,
                                                      thickness,
                                                      mountWindowSizeX, mountWindowSizeY, //Window Size
                                                      mountWindowPositionX, mountWindowPositionY,
                                                      windowMaterial, windowThickness);
  // Original detector solid
  G4VSolid * detectorSolid = new G4Box("DetectorSolid",
                                       detectorSizeX/2, detectorSizeY/2, detectorSizeZ/2);
  G4LogicalVolume * detectorLogical = new G4LogicalVolume(detectorSolid, detectorMaterial, "DetectorLogical");
  detectorLogical -> SetVisAttributes(detector_color);

  G4RotationMatrix * Ra = new G4RotationMatrix(0.*deg, 0.*deg, 0.*deg);
  G4ThreeVector Ta = G4ThreeVector(0.*mm, 0.*mm, detectorDepth);
  DetectorAndMountAssembly -> AddPlacedVolume(detectorLogical, Ta, Ra);

  fScoringVolume0 = detectorLogical;

  return DetectorAndMountAssembly;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4AssemblyVolume * DetectorConstruction::Shielding(G4Material * material,
                                                   G4double Xsize, G4double Ysize, G4double Zsize,
                                                   G4bool referenceX, G4bool referenceY) //Sheidling reference on Bottom side Edge
{

  G4VSolid * shieldingPlateSolid = new G4Box("shieldingPlateSolid", Xsize/2, Ysize/2, Zsize/2);
  G4LogicalVolume * shieldingPlateLogical = new G4LogicalVolume(shieldingPlateSolid, material, "sheldingPlateLogical");

  G4AssemblyVolume * shieldingAssembly = new G4AssemblyVolume();

  G4double refPointX, refPointY;
  if(referenceX){
    refPointX = -Xsize/2;
  }else{
    refPointX = Xsize/2;
  }

  if(referenceY){
    refPointY = -Ysize/2;
  }else{
    refPointY = Ysize/2;
  }

  G4ThreeVector Ta = G4ThreeVector(refPointX, refPointY , 0.*m);
  G4RotationMatrix * Ra = new G4RotationMatrix(0.*deg, 0.*deg, 0.*deg);

  shieldingAssembly -> AddPlacedVolume(shieldingPlateLogical, Ta, Ra);
  return shieldingAssembly;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructMaterials()
{
  G4NistManager* nistManager = G4NistManager::Instance();

  nistManager->FindOrBuildMaterial("G4_AIR");
  nistManager->FindOrBuildMaterial("G4_Al");
  nistManager->FindOrBuildMaterial("G4_Au");
  nistManager->FindOrBuildMaterial("G4_Si");
  nistManager->FindOrBuildMaterial("G4_MYLAR");
  nistManager->FindOrBuildMaterial("G4_Galactic");

  G4cout << G4endl << "The materials defined are : " << G4endl << G4endl;
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::ConstructColors()
{
  white = new G4VisAttributes( G4Colour());
  white -> SetVisibility(true);
  white -> SetForceSolid(true);

  trans_white = new G4VisAttributes( G4Colour());
  trans_white -> SetVisibility(false);
  trans_white -> SetForceSolid(false);

  blue = new G4VisAttributes(G4Colour(0. ,0. ,1.));
  blue -> SetVisibility(true);
  blue -> SetForceSolid(true);

  gray = new G4VisAttributes( G4Colour(0.5, 0.5, 0.5 ));
  gray-> SetVisibility(true);
  gray-> SetForceSolid(true);

  lightgray = new G4VisAttributes( G4Colour(0.75, 0.75, 0.75 ));
  lightgray-> SetVisibility(true);
  lightgray-> SetForceSolid(true);

  red = new G4VisAttributes(G4Colour(1. ,0. ,0.));
  red-> SetVisibility(true);
  red-> SetForceSolid(true);

  yellow = new G4VisAttributes(G4Colour(1., 1., 0. ));
  yellow-> SetVisibility(true);
  yellow-> SetForceSolid(true);

  green = new G4VisAttributes( G4Colour(25/255. , 255/255. ,  25/255. ));
  green -> SetVisibility(true);
  green -> SetForceSolid(true);

  darkGreen = new G4VisAttributes( G4Colour(0/255. , 100/255. ,  0/255. ));
  darkGreen -> SetVisibility(true);
  darkGreen -> SetForceSolid(true);

  darkOrange3 = new G4VisAttributes( G4Colour(205/255. , 102/255. ,  000/255. ));
  darkOrange3 -> SetVisibility(true);
  darkOrange3 -> SetForceSolid(true);

  skyBlue = new G4VisAttributes( G4Colour(135/255. , 206/255. ,  235/255. ));
  skyBlue -> SetVisibility(true);
  skyBlue -> SetForceSolid(true);

  // Color setup
  world_color = trans_white;    // For world volume
  mount_color = lightgray;      // For mount
  mylar_color = gray;
  foil_color = yellow;
  detector_color = darkOrange3;
  collimator_color = gray;
  shielding_color = gray;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
