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
// $Id: PrimaryGeneratorAction.cc 77781 2013-11-28 07:54:07Z gcosmo $
//
/// \file PrimaryGeneratorAction.cc
/// \brief Implementation of the PrimaryGeneratorAction class

#include "PrimaryGeneratorAction.hh"

#include "G4Tubs.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(),
  fParticleGun(0), fMessenger(0),
  fPositron(0), fMuon(0), fPion(0), fKaon(0), fProton(0),
  fEnergy(20.*MeV),
  fSigmaEnergy(0.*MeV),
  fSigmaAngle(0.*deg),
  fRandomizePrimary(false),
  fBeamWindow(0),
  fRadius(0.*mm),
  fPositionX(0.*mm),
  fPositionY(0.*mm)
{
    G4int n_particle = 1;
    fParticleGun  = new G4ParticleGun(n_particle);

    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName;
    fPositron = particleTable->FindParticle(particleName="e+");
    fMuon = particleTable->FindParticle(particleName="mu+");
    fPion = particleTable->FindParticle(particleName="pi+");
    fKaon = particleTable->FindParticle(particleName="kaon+");
    fProton = particleTable->FindParticle(particleName="proton");

    // default particle kinematics
    fParticleGun->SetParticleDefinition(fProton);

    // define commands for this class
    DefineCommands();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
    delete fParticleGun;
    delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::GeneratePrimaries(G4Event* event)
{
    // Bean window
    G4double position_angle = 2*CLHEP::pi;
    G4double half_thickness = 1.*mm;
    G4LogicalVolume *beamWinLV = G4LogicalVolumeStore::GetInstance()->GetVolume("beamWindowLogical");
    fBeamWindow = dynamic_cast<G4Tubs*>(beamWinLV->GetSolid());


    G4ParticleDefinition* particle;

    if (fRandomizePrimary)
    {
        G4int i = (int)(5.*G4UniformRand());
        switch(i)
        {
            case 0:
                particle = fPositron;
                break;
            case 1:
                particle = fMuon;
                break;
            case 2:
                particle = fPion;
                break;
            case 3:
                particle = fKaon;
                break;
            default:
                particle = fProton;
                break;
        }
        fParticleGun->SetParticleDefinition(particle);
    }
    else
    {
        particle = fParticleGun->GetParticleDefinition();
    }

    G4double Ekin = fEnergy + (G4UniformRand()-0.5)*fSigmaEnergy;

    fParticleGun->SetParticleEnergy(Ekin);
    //////////////////////////////////
    // 2pi beam directrion(Hafl sphere)
    //////////////////////////////////
    // G4double phi = G4UniformRand()*2*CLHEP::pi;
    // G4double theta = G4UniformRand()*0.5*CLHEP::pi; // polar angle
    // fParticleGun->SetParticleMomentumDirection(G4ThreeVector(std::sin(theta)*std::cos(phi),
    //                                                             std::sin(theta)*std::sin(phi),
    //                                                             std::cos(theta)));
    //////////////////////////////////
    // Beam goes straght
    //////////////////////////////////
    fParticleGun->SetParticleMomentumDirection(G4ThreeVector(0,0,1));

    // G4cout << "Particle momentum direction: " << phi << " " << theta<< G4endl;
    // G4double size = 1; 
    // G4double radius_input = size * G4UniformRand()*mm + fRadius;
    // G4double position_angle_input = position_angle * G4UniformRand();
    // G4double x0 = size * radius_input * std::cos(position_angle_input);
    // G4double y0 = size * radius_input * std::sin(position_angle_input);

    // For aufoil event map
    // G4double unitCellSizeX = 5.04545*mm;
    // G4double unitCellSizeY = 5.04545*mm;
    // For rectangular map
    G4double unitCellSizeX = 20.*mm;
    G4double unitCellSizeY = 20.*mm;
    G4double x0 = unitCellSizeX * G4UniformRand() + fPositionX;
    G4double y0 = unitCellSizeY * G4UniformRand() + fPositionY;

    G4double z0 = (-1.42*m) + half_thickness;

    fParticleGun->SetParticlePosition(G4ThreeVector(x0,y0,z0));

    fParticleGun->GeneratePrimaryVertex(event);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void PrimaryGeneratorAction::DefineCommands()
{
    // Define //generator command directory using generic messenger class
    fMessenger
      = new G4GenericMessenger(this,
                               "/tutorial/generator/",
                               "Primary generator control");

    // Kinetic Energy command
    G4GenericMessenger::Command& energyCmd
      = fMessenger->DeclarePropertyWithUnit("KineticEnergy", "MeV", fEnergy,
                                    "Mean kinetic energy of primaries.");
    energyCmd.SetParameterName("E", true);
    energyCmd.SetRange("E>=0.");
    energyCmd.SetDefaultValue("20.");

    // sigmaMomentum command
    G4GenericMessenger::Command& sigmaEnergyCmd
      = fMessenger->DeclarePropertyWithUnit("sigmaEnergy",
          "MeV", fSigmaEnergy, "Sigma kinteic energy of primaries.");
    sigmaEnergyCmd.SetParameterName("sE", true);
    sigmaEnergyCmd.SetRange("sE>=0.");
    sigmaEnergyCmd.SetDefaultValue("2.");

    // fRadius command
    G4GenericMessenger::Command& radiusCmd
      = fMessenger->DeclarePropertyWithUnit("radius",
          "mm", fRadius, "Radius of source position.");
    radiusCmd.SetParameterName("r", true);
    radiusCmd.SetRange("r>=0.");
    radiusCmd.SetDefaultValue("25.");

    // fPositionX command
    G4GenericMessenger::Command& positionXCmd
      = fMessenger->DeclarePropertyWithUnit("PositionX",
          "mm", fPositionX, "x position of sourceposition.");
    positionXCmd.SetParameterName("x", true);
    positionXCmd.SetDefaultValue("0.");

    // fPositionY command
    G4GenericMessenger::Command& positionYCmd
      = fMessenger->DeclarePropertyWithUnit("PositionY",
          "mm", fPositionY, "y position of sourceposition.");
    positionYCmd.SetParameterName("y", true);
    positionYCmd.SetDefaultValue("0.");

    // sigmaAngle command
    G4GenericMessenger::Command& sigmaAngleCmd
      = fMessenger->DeclarePropertyWithUnit("sigmaAngle", "rad", fSigmaAngle,
                                    "Sigma angle divergence of primaries.");
    sigmaAngleCmd.SetParameterName("t", true);
    sigmaAngleCmd.SetRange("t>=0.");
    sigmaAngleCmd.SetDefaultValue("2.");

    // randomizePrimary command
    G4GenericMessenger::Command& randomCmd
      = fMessenger->DeclareProperty("randomizePrimary", fRandomizePrimary);
    G4String guidance
       = "Boolean flag for randomizing primary particle types.\n";
    guidance
       += "In case this flag is false, you can select the primary particle\n";
    guidance += "  with /gun/particle command.";
    randomCmd.SetGuidance(guidance);
    randomCmd.SetParameterName("flg", true);
    randomCmd.SetDefaultValue("true");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
