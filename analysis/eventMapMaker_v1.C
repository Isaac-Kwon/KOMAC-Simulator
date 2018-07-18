TFile *loadFile(Int_t x, Int_t y){
	TFile *file = TFile::Open(Form("result/sim_17MeV/piece/save_run1_%d_%d.root", x, y));
	return file;
}
TH2F *getBeamProfile(){
	TFile *file = TFile::Open("komac_beam_profile/AuFoil/result_beamProfile.root");
	TH2F *histo = (TH2F*)file->Get("beamProfile_rebin");
	return histo;
}
TCanvas *defaultCanvas(TString canvas_name){
	TCanvas *c = new TCanvas(Form("c_%s", canvas_name.Data()), canvas_name.Data(), 1500, 500);
	c->Divide(3,1);
	return c;
}
TH2F *getPiece(TFile *file, Float_t factor=1){
	TH2F *histo = (TH2F*)file->Get("Collimator2/h2d_coll2Profile");
	TH2F *histo_clone = (TH2F*)histo->Clone();
	histo_clone->Scale(factor);
	return histo_clone;
}
TH2F *normalisation(TH2F *histo){
	histo->RebinX(11);
	histo->RebinY(11);
	// Float_t gmax = histo->GetMaximum();
	// Float_t scale = TMath::Power(gmax, -1);
	// histo->Scale(scale);
	return histo;
}
void eventMapMaker_v1(){
	gStyle->SetPalette(kRainBow);

	TH2F *measuredBeamProfile = getBeamProfile();
	TH2F *hPiece[16];
	TH2F *hPiece_Rebin[16];
	TH2F *diff_substract[16];
	TH2F *diff_ratio[16];
	for (int i = 0; i < 16; ++i)
	{
		hPiece[i] = new TH2F(Form("hPiece_Step%d", i+1), ";x (mm);y (mm)", 110, -27.75, 27.75, 110, -27.75, 27.75);

		diff_substract[i] = new TH2F(Form("diff_sub_hStep%d", i+1), ";x (mm);y (mm)", 10, -27.75, 27.75, 10, -27.75, 27.75);
		diff_ratio[i] = new TH2F(Form("diff_ratio_hStep%d", i+1), ";x (mm);y (mm)", 10, -27.75, 27.75, 10, -27.75, 27.75);
	}
	TH2F *hResult = new TH2F("hResult", ";x (mm);y (mm)", 110, -27.75, 27.75, 110, -27.75, 27.75);

	// Load the piece root files
	// Below is file id called in this macro
	// -----------------
	// | 3	7	11	15 |
	// | 2	6	10	14 |
	// | 1	5	9	13 |
	// | 0	4	8	12 |
	// -----------------
	TFile *piece[16];
	Int_t count = 0;
	for (int i = 3; i < 7; ++i)
	{
		for (int j = 3; j < 7; ++j)
		{
			piece[count] = loadFile(i, j);
			count++;
		}
	}

	// background
	TFile *fBackground = TFile::Open("result/sim_17MeV/run_103/save_run2_merged.root");
	TH2F *hBackground = (TH2F*)fBackground->Get("Collimator2/h2d_coll2Profile");

	// Factor
	Float_t factor[16][3] = {0};
	// 1st column -------------------------------------------------------------------------
	// Step1
	factor[0][0] = 0.8;
	factor[0][1] = 1;
	hPiece[0]->Add(getPiece(piece[2], factor[0][0]));
	hPiece[0]->Add(getPiece(piece[1], factor[0][1]));

	hResult->Add(getPiece(piece[2], factor[0][0]));
	hResult->Add(getPiece(piece[1], factor[0][1]));

	TCanvas *c_step1 = defaultCanvas("step1");
	c_step1->cd(1);
	normalisation(getPiece(piece[1], factor[0][1]))->Draw("colztext");
	c_step1->cd(2);
	normalisation(getPiece(piece[2], factor[0][0]))->Draw("colztext");
	c_step1->cd(3);
	normalisation(hPiece[0])->Draw("colztext");

	// Step2
	factor[1][0] = 0.3;
	hPiece[1]->Add(getPiece(piece[0], factor[1][0]));
	hResult->Add(getPiece(piece[0], factor[1][0]));

	TCanvas *c_step2 = defaultCanvas("step2");
	c_step2->cd(1);
	hPiece[0]->Draw("colztext");
	c_step2->cd(2);
	normalisation(getPiece(piece[0], factor[1][0]))->Draw("colztext");
	c_step2->cd(3);
	normalisation(hPiece[1])->Draw("colztext");
	hPiece[1]->Add(hPiece[0]);

	// Step3
	factor[2][0] = 0.3;
	hPiece[2]->Add(getPiece(piece[3], factor[2][0]));
	hResult->Add(getPiece(piece[3], factor[2][0]));

	TCanvas *c_step3 = defaultCanvas("step3");
	c_step3->cd(1);
	hPiece[1]->Draw("colztext");
	c_step3->cd(2);
	normalisation(getPiece(piece[3], factor[2][0]))->Draw("colztext");
	c_step3->cd(3);
	normalisation(hPiece[2])->Draw("colztext");
	hPiece[2]->Add(hPiece[1]);

	// 2nd column -------------------------------------------------------------------------
	// Step4
	factor[3][0] = 0.5;
	hPiece[3]->Add(getPiece(piece[5], factor[3][0]));
	hResult->Add(getPiece(piece[5], factor[3][0]));

	TCanvas *c_step4 = defaultCanvas("step4");
	c_step4->cd(1);
	hPiece[2]->Draw("colztext");
	c_step4->cd(2);
	normalisation(getPiece(piece[5], factor[3][0]))->Draw("colztext");
	c_step4->cd(3);
	normalisation(hPiece[3])->Draw("colztext");
	hPiece[3]->Add(hPiece[2]);

	// Step5
	factor[4][0] = 0.4;
	hPiece[4]->Add(getPiece(piece[6], factor[4][0]));
	hResult->Add(getPiece(piece[6], factor[4][0]));

	TCanvas *c_step5 = defaultCanvas("step5");
	c_step5->cd(1);
	hPiece[3]->Draw("colztext");
	c_step5->cd(2);
	normalisation(getPiece(piece[6], factor[4][0]))->Draw("colztext");
	c_step5->cd(3);
	normalisation(hPiece[4])->Draw("colztext");
	hPiece[4]->Add(hPiece[3]);

	// Step6
	factor[5][0] = 0.3; // Need to check when applyig piece[9]
	hPiece[5]->Add(getPiece(piece[4], factor[5][0]));
	hResult->Add(getPiece(piece[4], factor[5][0]));

	TCanvas *c_step6 = defaultCanvas("step6");
	c_step6->cd(1);
	hPiece[4]->Draw("colztext");
	c_step6->cd(2);
	normalisation(getPiece(piece[4], factor[5][0]))->Draw("colztext");
	c_step6->cd(3);
	normalisation(hPiece[5])->Draw("colztext");
	hPiece[5]->Add(hPiece[4]);

	// Step7
	factor[6][0] = 0.5;
	hPiece[6]->Add(getPiece(piece[7], factor[6][0]));
	hResult->Add(getPiece(piece[7], factor[6][0]));

	TCanvas *c_step7 = defaultCanvas("step7");
	c_step7->cd(1);
	hPiece[5]->Draw("colztext");
	c_step7->cd(2);
	normalisation(getPiece(piece[7], factor[6][0]))->Draw("colztext");
	c_step7->cd(3);
	normalisation(hPiece[6])->Draw("colztext");
	hPiece[6]->Add(hPiece[5]);

	// 3rd column -------------------------------------------------------------------------
	// Step8
	factor[7][0] = 0.6;
	hPiece[7]->Add(getPiece(piece[9], factor[7][0]));
	hResult->Add(getPiece(piece[9], factor[7][0]));

	TCanvas *c_step8 = defaultCanvas("step8");
	c_step8->cd(1);
	hPiece[6]->Draw("colztext");
	c_step8->cd(2);
	normalisation(getPiece(piece[9], factor[7][0]))->Draw("colztext");
	c_step8->cd(3);
	normalisation(hPiece[7])->Draw("colztext");
	hPiece[7]->Add(hPiece[6]);

	// Step9
	factor[8][0] = 0.5;
	hPiece[8]->Add(getPiece(piece[10], factor[8][0]));
	hResult->Add(getPiece(piece[10], factor[8][0]));

	TCanvas *c_step9 = defaultCanvas("step9");
	c_step9->cd(1);
	hPiece[7]->Draw("colztext");
	c_step9->cd(2);
	normalisation(getPiece(piece[10], factor[8][0]))->Draw("colztext");
	c_step9->cd(3);
	normalisation(hPiece[8])->Draw("colztext");
	hPiece[8]->Add(hPiece[7]);

	// Step10
	factor[9][0] = 0.5;
	hPiece[9]->Add(getPiece(piece[8], factor[9][0]));
	hResult->Add(getPiece(piece[8], factor[9][0]));

	TCanvas *c_step10 = defaultCanvas("step10");
	c_step10->cd(1);
	hPiece[8]->Draw("colztext");
	c_step10->cd(2);
	normalisation(getPiece(piece[8], factor[9][0]))->Draw("colztext");
	c_step10->cd(3);
	normalisation(hPiece[9])->Draw("colztext");
	hPiece[9]->Add(hPiece[8]);

	// Step11
	factor[10][0] = 0.1;
	hPiece[10]->Add(getPiece(piece[11], factor[10][0]));
	hResult->Add(getPiece(piece[11], factor[10][0]));

	TCanvas *c_step11 = defaultCanvas("step11");
	c_step11->cd(1);
	hPiece[9]->Draw("colztext");
	c_step11->cd(2);
	normalisation(getPiece(piece[11], factor[10][0]))->Draw("colztext");
	c_step11->cd(3);
	normalisation(hPiece[10])->Draw("colztext");
	hPiece[10]->Add(hPiece[9]);

	// 4th column -------------------------------------------------------------------------
	// Step12
	factor[11][0] = 0.2;
	hPiece[11]->Add(getPiece(piece[12], factor[11][0]));
	hResult->Add(getPiece(piece[12], factor[11][0]));

	TCanvas *c_step12 = defaultCanvas("step12");
	c_step12->cd(1);
	hPiece[10]->Draw("colztext");
	c_step12->cd(2);
	normalisation(getPiece(piece[12], factor[11][0]))->Draw("colztext");
	c_step12->cd(3);
	normalisation(hPiece[11])->Draw("colztext");
	hPiece[11]->Add(hPiece[10]);

	// Step13
	factor[12][0] = 0.3;
	hPiece[12]->Add(getPiece(piece[13], factor[12][0]));
	hResult->Add(getPiece(piece[13], factor[12][0]));

	TCanvas *c_step13 = defaultCanvas("step13");
	c_step13->cd(1);
	hPiece[11]->Draw("colztext");
	c_step13->cd(2);
	normalisation(getPiece(piece[13], factor[12][0]))->Draw("colztext");
	c_step13->cd(3);
	normalisation(hPiece[12])->Draw("colztext");
	hPiece[12]->Add(hPiece[11]);

	// Step14
	factor[13][0] = 0.3;
	hPiece[13]->Add(getPiece(piece[14], factor[13][0]));
	hResult->Add(getPiece(piece[14], factor[13][0]));

	TCanvas *c_step14 = defaultCanvas("step14");
	c_step14->cd(1);
	hPiece[12]->Draw("colztext");
	c_step14->cd(2);
	normalisation(getPiece(piece[14], factor[13][0]))->Draw("colztext");
	c_step14->cd(3);
	normalisation(hPiece[13])->Draw("colztext");
	hPiece[13]->Add(hPiece[12]);
	
	// Step15
	factor[14][0] = 0.2;
	hPiece[14]->Add(getPiece(piece[15], factor[14][0]));
	hResult->Add(getPiece(piece[15], factor[14][0]));

	TCanvas *c_step15 = defaultCanvas("step15");
	c_step15->cd(1);
	hPiece[12]->Draw("colztext");
	c_step15->cd(2);
	normalisation(getPiece(piece[15], factor[14][0]))->Draw("colztext");
	c_step15->cd(3);
	normalisation(hPiece[14])->Draw("colztext");
	hPiece[14]->Add(hPiece[13]);

	TCanvas *c_profile = new TCanvas("c_profile", "Profile", 800, 800);
	c_profile->cd();
	hResult->Draw("colz");


}