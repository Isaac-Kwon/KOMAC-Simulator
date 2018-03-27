void getDose(TH1D* depositE, Double_t nEvent){
	Double_t depositEnergy = depositE->GetMean();
	Double_t depositEnergy_RMS = depositE->GetRMS();

	cout << "-------------------------------------" << endl;
	cout << "Mean Deposit Energy" << endl;
	cout << "Mean: " << depositEnergy << " RMS: " << depositEnergy_RMS << endl;

	Double_t area = 2.0 * 3.6; // unit: cm^2
	Double_t thickness = 0.01; // unit: cm
	Double_t silicon_density = 2.33; // g/cm^3
	Double_t mass = silicon_density*area*thickness; // unit: g
	Double_t g_to_kg = TMath::Power(10, -3);
	Double_t mev_to_joule = 1.6*TMath::Power(10,-13);
	Double_t factor = mev_to_joule/(mass*g_to_kg);
	Double_t gray = depositEnergy*factor;
	Double_t gray_RMS = depositEnergy_RMS*factor;
	Double_t gray_to_rad = 100;

	cout << "-------------------------------------" << endl;
	cout << "Mean Deposit Dose in gray" << endl;
	cout << gray/nEvent << " +/- " << gray_RMS/nEvent << " Gy" << endl;
	cout << "-------------------------------------" << endl;
	cout << "Mean Deposit Dose in rad" << endl;
	cout << gray*gray_to_rad/nEvent << " +/- " << gray_RMS*gray_to_rad/nEvent << " rad" << endl;
}
void fill1dHistoFromTree(TTree* tree, TH1D* histo, TString branch, TString cut){
	tree->Draw(Form("%s>>%s", branch.Data(), histo->GetName()), cut.Data(), "goff");
}
void fill2dHistoFromTree(TTree* tree, TH2D* histo, TString branchX, TString branchY, TString cut){
	tree->Draw(Form("%s:%s>>%s", branchX.Data(), branchY.Data(), histo->GetName()), cut.Data(), "goff");
}
TH1D *getSlice(TH2D *histo, int axis, TString name, int range1, int range2){
	TH1D *slice;
	switch(axis){
		case 0:
			slice = histo->ProjectionX(name.Data(), range1, range2);
			break;
		case 1:
			slice = histo->ProjectionY(name.Data(), range1, range2);
			break;
	}
	return slice;
}
TCanvas *getCanvas(int cat, TString name){
	int sizex, sizey;
	switch(cat){
		case 0:
			sizex = 500, sizey = 500;
			break;
		case 1:
			sizex = 800, sizey = 600;
			break;
	}
	TCanvas *c1 = new TCanvas(Form("c_%s", name.Data()), name.Data(), sizex, sizey);
	return c1;
}
TH2D *getProfile(TFile *file){
	TH2D *profile = (TH2D*)file->Get("histo/h2.1");
	return profile;
}
TTree *getTree(TFile *file, int treeId){
	TTree *tree = (TTree*)file->Get(Form("ntuple/%d", treeId));
	cout << "-------------------------------------" << endl;
	switch (treeId){
		case 101:
			cout << "Tree ID: 101 (Inital beam condition) is loaded!" << endl;
			cout << "Total Entry: " << tree->GetEntries() << endl;
			break;
		case 201:
			cout << "Tree ID: 201 (Detector volume) is loaded!" << endl;
			cout << "Total Entry: " << tree->GetEntries() << endl;
			break;
		case 202:
			cout << "Tree ID: 202 (Mount window volume) is loaded!" << endl;
			cout << "Total Entry: " << tree->GetEntries() << endl;
			break;
		case 203:
			cout << "Tree ID: 203 (Collimator2 hole volume) is loaded!" << endl;
			cout << "Total Entry: " << tree->GetEntries() << endl;
			break;
	}
	return tree;
}
TFile *getFile(TString name){
	TFile *file = TFile::Open(Form("%s.root", name.Data()));
	return file;
}
void draw(TString name = "../build/run", int nEvent = 100000){
	gStyle->SetOptStat(1);
	// Get file & Tree
	TFile *file = getFile(name.Data());
	// Load TTree
	TTree *t_input = getTree(file, 101);
	TTree *t_col2_hole = getTree(file, 203);
	TTree *t_mount_window = getTree(file, 202);
	TTree *t_detector = getTree(file, 201);

	// ---------------------------------------------------------------------------
	// Beam profile at beam window
	// ---------------------------------------------------------------------------

	// Beam profile
	TH2D *h2d_Profile = getProfile(file);
	TCanvas *cProfile = getCanvas(0, "IntialBeamProfile");
	cProfile->cd();
	h2d_Profile->SetTitle("vertex: XY;x (cm); y (cm)");
	h2d_Profile->Draw("colz");

	// Slice range
	// TODO: slice range function & histogram optimse
	Int_t centerX = h2d_Profile->GetXaxis()->FindBin(0.);
	Int_t sbinX = centerX - 3;
	Int_t ebinX = centerX + 2;

	Int_t centerY = h2d_Profile->GetYaxis()->FindBin(0.);
	Int_t sbinY = centerY - 3;
	Int_t ebinY = centerY + 2;

	// Beam sliceX
	TCanvas *cProfile_sliceX = getCanvas(0, "IntialBeamProfile_sliceX");
	TH1D *h2d_sliceX = getSlice(h2d_Profile, 0, "center_slice_X", sbinX, ebinX);
	h2d_sliceX->SetTitle(Form("SliceX[%.2f,%.2f];x (cm);", 
							h2d_Profile->GetXaxis()->GetBinCenter(sbinX),
							h2d_Profile->GetXaxis()->GetBinCenter(ebinX)));
	h2d_sliceX->SetAxisRange(-15,15);
	h2d_sliceX->Draw();

	// Beam sliceY
	TCanvas *cProfile_sliceY = getCanvas(0, "IntialBeamProfile_sliceY");
	TH1D *h2d_sliceY = getSlice(h2d_Profile, 1, "center_slice_Y", sbinY, ebinY);
	h2d_sliceY->SetTitle(Form("SliceY[%.2f,%.2f];x (cm);", 
							h2d_Profile->GetYaxis()->GetBinCenter(sbinY),
							h2d_Profile->GetYaxis()->GetBinCenter(ebinY)));
	h2d_sliceY->SetAxisRange(-15,15);
	h2d_sliceY->Draw();

	// Beam direction
	TCanvas *cBeamDirectionTheta = getCanvas(1, "BeamDirectionTheta");
	cBeamDirectionTheta->cd();
	TH1D *h1d_dirTheta = new TH1D("h1d_dirTheta", ";Theta [rad];", 400, 0, 4);
	fill1dHistoFromTree(t_input, h1d_dirTheta, "dirTheta", "");
	h1d_dirTheta->Draw();

	TCanvas *cBeamDirectionPhi = getCanvas(1, "BeamDirectionPhi");
	cBeamDirectionPhi->cd();
	TH1D *h1d_dirPhi = new TH1D("h1d_dirPhi", ";Phi [rad];", 700, 0, 7);
	fill1dHistoFromTree(t_input, h1d_dirPhi, "dirPhi", "");
	h1d_dirPhi->Draw();

	// Kinetic energy & Comparison
	TCanvas *cInputKinE = getCanvas(1, "InputKinE");
	cInputKinE->Divide(2,2);
	cInputKinE->cd(1);
	TH1D *h1d_Ekin_beamWindow = new TH1D("h1d_Ekin_beamWindow", ";Kinetic Energy (MeV);", 2500, 0, 25);
	fill1dHistoFromTree(t_input, h1d_Ekin_beamWindow, "Ekin", "");
	h1d_Ekin_beamWindow->SetLineColor(kBlack);
	h1d_Ekin_beamWindow->Draw();

	cInputKinE->cd(2);
	TH1D *h1d_Ekin_coll2 = new TH1D("h1d_Ekin_coll2", ";Kinetic Energy (MeV);", 2500, 0, 25);
	// fill1dHistoFromTree(t_col2_hole, h1d_Ekin_coll2, "IncidentEkin", "");
	fill1dHistoFromTree(t_col2_hole, h1d_Ekin_coll2, "IncidentEkin", "pid==2212 && prePosZ == -522.5");
	h1d_Ekin_coll2->SetLineColor(kRed);
	h1d_Ekin_coll2->Draw();

	cInputKinE->cd(3);
	TH1D *h1d_Ekin_mount = new TH1D("h1d_Ekin_mount", ";Kinetic Energy (MeV);", 2500, 0, 25);
	// fill1dHistoFromTree(t_mount_window, h1d_Ekin_mount, "IncidentEkin", "");
	fill1dHistoFromTree(t_mount_window, h1d_Ekin_mount, "IncidentEkin", "pid==2212 && prePosZ == -1");
	h1d_Ekin_mount->SetLineColor(kBlue);
	h1d_Ekin_mount->Draw();

	cInputKinE->cd(4);
	TH1D *h1d_Ekin_detector = new TH1D("h1d_Ekin_detector", ";Kinetic Energy (MeV);", 2500, 0, 25);
	// fill1dHistoFromTree(t_detector, h1d_Ekin_detector, "IncidentEkin", "");
	fill1dHistoFromTree(t_detector, h1d_Ekin_detector, "IncidentEkin", "pid==2212");
	h1d_Ekin_detector->SetLineColor(kGreen);
	h1d_Ekin_detector->Draw();

	// Kinetic energy difference between mount window and detector window
	TCanvas *cDiffKinE = getCanvas(1, "DiffKinE");
	cDiffKinE->cd();

	TH1D *h1d_Ekin_diff = new TH1D("h1d_Ekin_diff", ";Diff (MeV);", 2500, 0, 25);
	h1d_Ekin_diff->Add(h1d_Ekin_mount, h1d_Ekin_detector, 1, -1);

	// t_mount_window->Draw(">>mountList", "pid==2212 && prePosZ== -1", "entrylist");
	// TEntryList *mountList = (TEntryList*)gDirectory->Get("mountList");
	// t_detector->Draw(">>detectorList", "pid==2212", "entrylist");
	// TEntryList *detectorList = (TEntryList*)gDirectory->Get("detectorList");

	// Double_t mountInputKinE, detectorInputKinE;
	// t_mount_window->SetBranchAddress("IncidentEkin", &mountInputKinE);
	// t_detector->SetBranchAddress("IncidentEkin", &detectorInputKinE);


	// for (int i = 0; i < mountList->GetN(); ++i)
	// {
	// 	int mountIndex = mountList->GetEntry(i);
	// 	int detectorIndex = detectorList->GetEntry(i);

	// 	t_mount_window->GetEntry(mountIndex);
	// 	t_mount_window->GetEntry(detectorIndex);
	// 	Double_t diff = mountInputKinE - detectorInputKinE;
	// 	h1d_Ekin_diff->Fill(diff);
	// 	cout << "#" << i << ": " << diff << " MeV" << endl;
	// }
	h1d_Ekin_diff->Draw();

	// ---------------------------------------------------------------------------
	// Detector
	// ---------------------------------------------------------------------------

	// Particle energy entering detector
	TCanvas *cDetEnteringE = getCanvas(1, "DetEnteringE");
	cDetEnteringE->cd();
	TH1D *h1d_detEnteringE = new TH1D("h1d_detEnteringE", ";Kinetic Energy (MeV);", 100, 0, 25);
	fill1dHistoFromTree(t_detector, h1d_detEnteringE, "IncidentEkin", "pid==2212");
	h1d_detEnteringE->Draw();

	// Deposit Energy
	TCanvas *cDepositE = getCanvas(1, "DepositE");
	cDepositE->cd();
	TH1D *h1d_depositE = new TH1D("h1d_depositE", ";Deposit Energy (MeV);", 400, 0, 4);
	fill1dHistoFromTree(t_detector, h1d_depositE, "DepositE", "pid==2212");
	h1d_depositE->Draw();
	getDose(h1d_depositE, nEvent);

	// Position distribution
	TCanvas *cDetPtlPosition = getCanvas(1, "DetPtlPosition");
	cDetPtlPosition->cd();
	TH2D *h2d_IncidentProtonPosition = new TH2D("h2d_IncidentProtonPosition", "h2d_IncidentProtonPosition", 400, -110, -30, 381, -36, 36);
	fill2dHistoFromTree(t_detector, h2d_IncidentProtonPosition, "prePosX", "prePosY", "pid==2212");
	h2d_IncidentProtonPosition->Draw();

	// ---------------------------------------------------------------------------
	// End
	// ---------------------------------------------------------------------------
	cout << "End of macro" << endl;
}