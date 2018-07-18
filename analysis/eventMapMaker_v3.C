TFile *loadFile(Int_t x, Int_t y){
	TFile *file = TFile::Open(Form("result/sim_17MeV/run_402/save_run1_%d_%d.root", x, y));
	// TFile *file = TFile::Open(Form("result/sim_17MeV/run_301/test/save_run1_%d_%d.root", x, y));
	return file;
}
Double_t getScaleFactor(TH2D* histo){
	// Double_t maximum = histo->GetMaximum();
	Double_t integral = histo->Integral();
	cout << "Integral: " << integral << endl;
	Double_t factor = TMath::Power(integral, -1);
	Double_t factor_normalisation = factor*100;
	return factor_normalisation;
}
void scaling(TH2D *histo){
	Double_t scale_factor = getScaleFactor(histo);
	histo->Scale(scale_factor);
}
TH2D *copyHisto(TH2D*histo){
	TH2D* histo2 = (TH2D*)histo->Clone();
	return histo2;
}
void rebin(TH2D* histo, Int_t n){
	histo->RebinX(n);
	histo->RebinY(n);	
}
void qa_inputHist(TString name, TH2D* histo, Int_t n){
	TH2D *histo2 = copyHisto(histo);
	rebin(histo2, n);

	TCanvas *c = new TCanvas(name.Data(), name.Data(), 500,500);
	c->cd();
	histo2->Draw("colztext");
	c->SaveAs(Form("%s.pdf", name.Data()));
}
TH2D *getBeamProfile(){
	TFile *file = TFile::Open("komac_beam_profile/AuFoil/result_beamProfile.root");
	TH2D *histo = (TH2D*)file->Get("beamProfile_rebin");
	return histo;
}
TCanvas *defaultCanvas(TString canvas_name){
	TCanvas *c = new TCanvas(Form("c_%s", canvas_name.Data()), canvas_name.Data(), 1500, 500);
	c->Divide(3,1);
	return c;
}
TH2D *normalisation(TH2D *histo, Int_t n){
	TH2D *histo2 = copyHisto(histo);
	rebin(histo2, n);
	scaling(histo2);
	return histo2;
}
TH2D *getPiece(TFile *file, Int_t n = 11){
	TH2D *histo = (TH2D*)file->Get("Collimator2/h2d_coll2Profile");
	TH2D *histo2 = normalisation(histo, n);
	return histo2;
}
void eventMapMaker_v3(){ // 5x5 map
	int debug = 1;
	gStyle->SetPalette(kRainBow);

	TH2D *measuredBeamProfile = getBeamProfile();
	TH2D *hPiece[25];
	TH2D *hPiece_Rebin[25];
	TH2D *diff_substract[25];
	TH2D *diff_ratio[25];
	// for (int i = 0; i < 16; ++i)
	// {
	// 	hPiece[i] = new TH2D(Form("hPiece_Step%d", i+1), ";x (mm);y (mm)", 110, -27.75, 27.75, 110, -27.75, 27.75);
	// 	hPiece_Rebin[i] = new TH2D(Form("hPiece_Step%d_rebin", i+1), ";x (mm);y (mm)", 10, -27.75, 27.75, 10, -27.75, 27.75);

	// 	diff_substract[i] = new TH2D(Form("diff_sub_hStep%d", i+1), ";x (mm);y (mm)", 10, -27.75, 27.75, 10, -27.75, 27.75);
	// 	diff_ratio[i] = new TH2D(Form("diff_ratio_hStep%d", i+1), ";x (mm);y (mm)", 10, -27.75, 27.75, 10, -27.75, 27.75);
	// }
	TH2D *hResult = new TH2D("hResult", ";x (mm);y (mm)", 10, -27.75, 27.75, 10, -27.75, 27.75);

	// Load the piece root files
	// Below is file id called in this macro
	// -----------------
	// | 3	7	11	15 |
	// | 2	6	10	14 |
	// | 1	5	9	13 |
	// | 0	4	8	12 |
	// -----------------
	TFile *piece[25];
	Int_t count = 0;
	Float_t setup[25] = {
		0, 1, 1, 1, 0, 
		1, 1, 1, 1, 1, 
		1, 1, 1, 1, 1, 
		1, 1, 1, 1, 1, 
		0, 1, 1, 1, 0
	};
	for (int i = 0; i < 5; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			if(setup[count]==0) {
				count++;
				continue;
			}
			piece[count] = loadFile(i, j);
			count++;
		}
	}
	// Each piece
	if(debug>0){
		TCanvas *c_piece[25];
		for (int i = 0; i < 25; ++i)
		{
			if(setup[i]==0) continue;
			cout << "Piece ID: " << i << " information" << endl;
			c_piece[i] = new TCanvas(Form("fileID_%d", i), Form("fileID_%d", i), 500, 500);
			hPiece[i] = getPiece(piece[i], 11);
			c_piece[i]->cd();
			hPiece[i]->Draw("colztext");
			c_piece[i]->SaveAs(Form("output/piece/fileID_%d.pdf", i));
		}		
	}

	// Factor
	Float_t factor[25] = {
		0, 1, 1, 1, 0, 
		1, 1, 1, 1, 1, 
		1, 1, 1, 1, 1, 
		1, 1, 1, 1, 1, 
		0, 1, 1, 1, 0
	};

	Float_t scaleFactor = 0;
	for (int i = 0; i < 25; ++i)
	{
		scaleFactor += factor[i];
	}
	cout << "Scale factor: " << scaleFactor << endl;
	for (int i = 0; i < 25; ++i)
	{
		if(setup[i]==0) continue;
		hResult->Add(getPiece(piece[i], 11), factor[i]);
	}
	TCanvas *c_result = new TCanvas("c_result", "Result_v2", 500, 500);
	c_result->cd();
	hResult->Scale(TMath::Power(scaleFactor, -1));
	hResult->SetMaximum(2.5);
	hResult->Draw("colztext");

}