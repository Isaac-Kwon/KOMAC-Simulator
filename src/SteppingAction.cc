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
// $Id: B1SteppingAction.cc 74483 2013-10-09 13:37:06Z gcosmo $
//
/// \file B1SteppingAction.cc
/// \brief Implementation of the B1SteppingAction class
// Modify this file to KOMAC-simulator

#include "SteppingAction.hh"
#include "EventAction.hh"
#include "DetectorConstruction.hh"
#include "HistoManager.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fScoringVolume0(0),
  fScoringVolume1(0),
  fScoringVolume2(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  if (!fScoringVolume0) {
    const DetectorConstruction* detectorConstruction
      = static_cast<const DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume0 = detectorConstruction->GetScoringVolume(0);
  }

  if (!fScoringVolume1) {
    const DetectorConstruction* detectorConstruction
      = static_cast<const DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume1 = detectorConstruction->GetScoringVolume(1);
  }

  if (!fScoringVolume2) {
    const DetectorConstruction* detectorConstruction
      = static_cast<const DetectorConstruction*>
        (G4RunManager::GetRunManager()->GetUserDetectorConstruction());
    fScoringVolume2 = detectorConstruction->GetScoringVolume(2);
  }

  // get volume of the current step
  G4LogicalVolume* volume
    = step->GetPreStepPoint()->GetTouchableHandle()
      ->GetVolume()->GetLogicalVolume();

  // check if we are in scoring volume
  if (volume != fScoringVolume0 && volume != fScoringVolume1 && volume != fScoringVolume2) return;

  // ntuple id selection
  int ntuple = 0;
  if (volume == fScoringVolume0 ) ntuple = 2;
  if (volume == fScoringVolume1 ) ntuple = 3;
  if (volume == fScoringVolume2 ) ntuple = 4;

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();

  // collect energy deposited in this step
  G4double edepStep = step->GetTotalEnergyDeposit();
  if (ntuple==2) fEventAction->AddEdep(edepStep);

  G4double stepLength = step->GetStepLength();

  G4Track* track         = step->GetTrack();
  G4double kinEnergy     = track->GetKineticEnergy();
  G4int pid              = track->GetDynamicParticle()->GetPDGcode();
  G4ThreeVector direction = track->GetMomentumDirection();  
  G4double theta = direction.theta(), phi = direction.phi();

  G4ThreeVector preStep = step->GetPreStepPoint()->GetPosition();
  G4double preStepX = preStep.x()/CLHEP::mm, preStepY = preStep.y()/CLHEP::mm, preStepZ = preStep.z()/CLHEP::mm;
  G4ThreeVector postStep = step->GetPostStepPoint()->GetPosition();
  G4double postStepX = postStep.x()/CLHEP::mm, postStepY = postStep.y()/CLHEP::mm, postStepZ = postStep.z()/CLHEP::mm;

  G4double kinEnergy_preStep = step->GetPreStepPoint()->GetKineticEnergy();
  G4double kinEnergy_postStep = step->GetPostStepPoint()->GetKineticEnergy();

  G4cout << "Step information" << G4endl;
  G4cout << "------------------------------------------------" << G4endl;
  G4cout << "Pre-step position(mm): (" << preStep.x()/CLHEP::mm << ", " << preStep.y()/CLHEP::mm << ", " << preStep.z()/CLHEP::mm << ")" << G4endl;
  G4cout << "Post-step position(mm): (" << postStep.x()/CLHEP::mm << ", " << postStep.y()/CLHEP::mm << ", " << postStep.z()/CLHEP::mm << ")" << G4endl;
  G4cout << "Step Energy: " << kinEnergy/CLHEP::MeV << " MeV\t|\t";
  G4cout << "Prestep Kinetic Energy: " << kinEnergy_preStep/CLHEP::MeV << " MeV\t|\t";
  G4cout << "Poststep Kinetic Energy: " << kinEnergy_postStep/CLHEP::MeV << " MeV\t|\t";  
  G4cout << "Deposit Energy" <<  edepStep/CLHEP::MeV << " MeV\t|\tStepLength: " << stepLength/CLHEP::cm << " cm" << G4endl;

  analysisManager->FillNtupleIColumn(ntuple,0,pid);
  analysisManager->FillNtupleDColumn(ntuple,1,kinEnergy_preStep);
  analysisManager->FillNtupleDColumn(ntuple,2,edepStep);
  analysisManager->FillNtupleDColumn(ntuple,3,preStepX);
  analysisManager->FillNtupleDColumn(ntuple,4,preStepY);
  analysisManager->FillNtupleDColumn(ntuple,5,preStepZ);
  analysisManager->FillNtupleDColumn(ntuple,6,postStepX);
  analysisManager->FillNtupleDColumn(ntuple,7,postStepY);
  analysisManager->FillNtupleDColumn(ntuple,8,postStepZ);
  analysisManager->FillNtupleDColumn(ntuple,9,theta);
  analysisManager->FillNtupleDColumn(ntuple,10,phi);
  analysisManager->AddNtupleRow(ntuple);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
