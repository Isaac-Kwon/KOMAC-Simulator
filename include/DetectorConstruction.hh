//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
// $Id: DetectorConstruction.hh 76474 2013-11-11 10:36:34Z gcosmo $
//
/// \file DetectorConstruction.hh
/// \brief Definition of the DetectorConstruction class

#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4RotationMatrix.hh"
#include "G4FieldManager.hh"

#include <vector>

class G4VPhysicalVolume;
class G4LogicalVolume;
class G4AssemblyVolume;
class G4Material;
class G4VisAttributes;
class G4GenericMessenger;

/// Detector construction

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  DetectorConstruction();
  virtual ~DetectorConstruction();


  //Here the geomtry is constructed. This method is called only by
  //master thread and all geometry built here is shared among threads
  virtual G4VPhysicalVolume* Construct();

  //This is just a convinience: a method where all materials needed
  //are created
  void ConstructMaterials();
  void ConstructColors();

  G4AssemblyVolume * Collimator(G4Material* material, G4double Xsize, G4double Ysize, G4double Zsize, G4double Radius);
  G4AssemblyVolume * FoilWithCollimator(G4Material * material,
                                        G4double Xsize, G4double Ysize, G4double Zsize, G4double Radius,
                                        G4Material * material_film, G4double thickness);
  G4AssemblyVolume * Absorber();
  G4AssemblyVolume * Mount(G4Material * mountMaterial,
                           G4double mountSizeX, G4double mountSizeY, G4double mountSizeZ,
                           G4double thickness,
                           G4double mountWindowSizeX, G4double mountWindowSizeY,
                           G4double mountWindowPositionX , G4double mountWindowPositionY,
                           G4Material * windowMaterial, G4double windowThickness);
  G4AssemblyVolume * DetectorAndMount(G4Material * mountMaterial,
                                      G4double mountSizeX, G4double mountSizeY, G4double mountSizeZ,
                                      G4double thickness,
                                      G4double mountWindowSizeX, G4double mountWindowSizeY, //Window Size
                                      G4double mountWindowPositionX, G4double mountWindowPositionY,
                                      G4Material * detectorMaterial,
                                      G4double detectorSizeX, G4double detectorSizeY, G4double detectorSizeZ,
                                      G4double detectorDepth,
                                      G4Material * windowMaterial, G4double windowThickness);
  G4AssemblyVolume * Shielding(G4Material * material,
                               G4double Xsize, G4double Ysize, G4double Zsize,
                               G4bool referenceX, G4bool referenceY);
  G4LogicalVolume* GetScoringVolume() const { return fScoringVolume; } // Get the scoring volume variable.

  // Color set
  G4VisAttributes* trans_white;
  G4VisAttributes* gray;
  G4VisAttributes* yellow;
  G4VisAttributes* darkOrange3;
  G4VisAttributes* lightgray;
  // Setting value
  G4VisAttributes* world_color;
  G4VisAttributes* mount_color;
  G4VisAttributes* foil_color;
  G4VisAttributes* mylar_color;
  G4VisAttributes* detector_color;
  G4VisAttributes* collimator_color;
  G4VisAttributes* shielding_color;

private:
  G4LogicalVolume* fScoringVolume;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
