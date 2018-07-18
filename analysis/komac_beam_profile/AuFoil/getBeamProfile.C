void getBeamProfile(){
	Int_t debug = 1;
	gStyle->SetOptStat(0);
	gStyle->SetPalette(kRainBow);
	ifstream in;
	in.open("beamProfile.csv");
	Float_t count;
	Float_t x=0;
	Float_t y=74 - 46.75;

	Int_t nbinx = 110;
	Int_t nbiny = 110;
	TH2F *beamProfile = new TH2F("h2dBeamProfile", "Beam Profile", nbinx, -27.75, 27.75, nbiny, -27.75, 27.75);
	TFile *save = new TFile("result_beamProfile.root", "recreate");

	Int_t nlines = 0;
	TString cell, correctedCell;
	for (int iY = 0; iY < 108; ++iY){
		x = 76.5-49.25;
		for (int iX = 0; iX < 110; ++iX)
		{
			if (!(iX==109)) cell.ReadToDelim(in, ',');
			else cell.ReadToDelim(in);
			Float_t count = cell.Atof();
			int binx = beamProfile->GetXaxis()->FindBin(x);
			int biny = beamProfile->GetYaxis()->FindBin(y);
			if(debug>0){
				cout << "Position coordinate: (" << x << "," << y << "," << count << ")" << endl;			
				cout << "Bin coordinate: (" << binx << "," << biny << "," << count << ")" << endl;
				// if (iX==0 ) cout << "First: " << count << endl;
				// if (iX==109) cout << "End: " << count << endl;				
			}
			beamProfile->SetBinContent(binx, biny, count);
			x -= 0.5;
		}
		y -= 0.5;
	}
	cout << nlines << endl;

	TCanvas *c1 = new TCanvas("c1", "Beam Profile", 500, 500);
	c1->cd();
	beamProfile->Draw("colz");
	beamProfile->SetTitle("BeamProfile;x (mm);y (mm)");
	save->cd();
	beamProfile->Write();

	TCanvas *c2 = new TCanvas("c2", "Beam Profile in X", 500, 500);
	c2->cd();
	TH1D *h1dSliceX = beamProfile->ProjectionX("sliceX", 55, 56);
	Float_t min_xBinCenter = beamProfile->GetXaxis()->GetBinCenter(55);
	Float_t max_xBinCenter = beamProfile->GetXaxis()->GetBinCenter(56);
	Float_t min_xBinWidth = beamProfile->GetXaxis()->GetBinWidth(55);
	Float_t max_xBinWidth = beamProfile->GetXaxis()->GetBinWidth(56);
	Float_t min_x = min_xBinCenter - min_xBinWidth;
	Float_t max_x = max_xBinCenter + max_xBinWidth;
	cout << min_xBinWidth << endl;
	h1dSliceX->SetTitle(Form("h1dSliceX[%.3f, %.3f];x (mm);", min_x, max_x));
	h1dSliceX->Draw();
	TCanvas *c3 = new TCanvas("c3", "Beam Profile in Y", 500, 500);
	c3->cd();
	TH1D *h1dSliceY = beamProfile->ProjectionY("h1dSliceY", 55, 56);
	Float_t min_yBinCenter = beamProfile->GetXaxis()->GetBinCenter(55);
	Float_t max_yBinCenter = beamProfile->GetXaxis()->GetBinCenter(56);
	Float_t min_yBinWidth = beamProfile->GetXaxis()->GetBinWidth(55);
	Float_t max_yBinWidth = beamProfile->GetXaxis()->GetBinWidth(56);
	Float_t min_y = min_yBinCenter - min_yBinWidth;
	Float_t max_y = max_yBinCenter + max_yBinWidth;
	h1dSliceY->SetTitle(Form("h1dSliceY[%.3f, %.3f];y (mm);", min_y, max_y));
	h1dSliceY->GetXaxis()->SetTitleOffset(1.0);
	h1dSliceY->Draw();

	TCanvas *c4 = new TCanvas("c4", "Beam Profile_rebin", 500, 500);
	c4->cd();
	TH2F *beamProfile_rebin = (TH2F*)beamProfile->Clone();
	beamProfile_rebin->SetName("beamProfile_rebin");
	beamProfile_rebin->RebinX(11);
	beamProfile_rebin->RebinY(11);
	Float_t gmax = beamProfile_rebin->GetMaximum();
	Float_t scale = TMath::Power(gmax, -1);
	beamProfile_rebin->Scale(scale);
	beamProfile_rebin->Draw("colztext");
	beamProfile_rebin->SetTitle("BeamProfile;x (mm);y (mm)");
	save->cd();
	beamProfile_rebin->Write();
}