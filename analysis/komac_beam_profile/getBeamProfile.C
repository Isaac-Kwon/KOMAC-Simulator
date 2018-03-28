void getBeamProfile(){
	Int_t debug = 0;
	gStyle->SetOptStat(0);
	ifstream in;
	in.open("beamProfile.csv");
	Float_t count;
	Float_t x=0;
	Float_t y=1.95;

	Int_t nbinx = 110;
	Int_t nbiny = 108;
	TH2F *beamProfile = new TH2F("h2dBeamProfile", "Beam Profile", nbinx, 2.125, 7.625, nbiny, 1.925, 7.325);
	TH2F *correctedBeamProfile = new TH2F("h2dCorrectedBeamProfile", "Beam Profile", nbinx, 2.125, 7.625, nbiny, 1.925, 7.325);

	Int_t nlines = 0;
	TString cell, correctedCell;
	for (int iY = 0; iY < 108; ++iY){
		x = 2.15;
		for (int iX = 0; iX < 110; ++iX)
		{
			if (!(iX==109)) cell.ReadToDelim(in, ',');
			else cell.ReadToDelim(in);
			Float_t count = cell.Atof();
			int binx = beamProfile->GetXaxis()->FindBin(x);
			int biny = beamProfile->GetYaxis()->FindBin(y);
			if(debug>0){
				if(iY<2) cout << "Position coordinate: (" << x << "," << y << "," << count << ")" << endl;			
				if(iY<2) cout << "Bin coordinate: (" << binx << "," << biny << "," << count << ")" << endl;
				if (iX==0 ) cout << "First: " << count << endl;
				if (iX==109) cout << "End: " << count << endl;				
			}
			beamProfile->SetBinContent(binx, biny, count);
			x += 0.05;
		}
		y += 0.05;
	}
	cout << nlines << endl;
	Float_t max = beamProfile->GetMaximum();

	for (int iY = 0; iY < 108; ++iY){
		x = 2.15;
		for (int iX = 0; iX < 110; ++iX)
		{
			if (!(iX==109)) correctedCell.ReadToDelim(in, ',');
			else cell.ReadToDelim(in);
			Float_t count = cell.Atof();
			int binx = beamProfile->GetXaxis()->FindBin(x);
			int biny = beamProfile->GetYaxis()->FindBin(y);
			if(debug>0){
				if(iY<2) cout << "Position coordinate: (" << x << "," << y << "," << count << ")" << endl;			
				if(iY<2) cout << "Bin coordinate: (" << binx << "," << biny << "," << count << ")" << endl;
				if (iX==0 ) cout << "First: " << count << endl;
				if (iX==109) cout << "End: " << count << endl;
			}
			if(count<max*0.8) correctedBeamProfile->SetBinContent(binx, biny, count);
			x += 0.05;
		}
		y += 0.05;
	}

	TCanvas *c1 = new TCanvas("c1", "Beam Profile", 500, 500);
	c1->cd();
	Int_t gbin = beamProfile->GetMaximumBin();
	Int_t max_bin_x, max_bin_y, max_bin_z;
	beamProfile->GetBinXYZ(gbin, max_bin_x, max_bin_y, max_bin_z);
	Float_t max_x = beamProfile->GetXaxis()->GetBinCenter(max_bin_x) ;
	Float_t max_y = beamProfile->GetYaxis()->GetBinCenter(max_bin_y) ;
	Float_t scale = TMath::Power(max, -1);
	beamProfile->Scale(scale);
	beamProfile->Draw("colz");
	beamProfile->SetTitle("BeamProfile;x (cm);y (cm)");
	cout << max_x <<  " " << max_y << endl;	
	cout << max_x - 4.875 <<  " " << max_y - 4.625 << endl;

	TCanvas *c2 = new TCanvas("c2", "Beam Profile in X", 500, 500);
	c2->cd();
	TH1D *h1dSliceX = beamProfile->ProjectionX("sliceX", 55, 55);
	Float_t xBinCenter = beamProfile->GetXaxis()->GetBinCenter(55);
	Float_t xBinWidth = beamProfile->GetXaxis()->GetBinWidth(55);
	Float_t min_xBinCenter = xBinCenter - xBinWidth/2;
	Float_t max_xBinCenter = xBinCenter + xBinWidth/2;
	h1dSliceX->SetTitle(Form("h1dSliceX[%.3f, %.3f];x (cm);", min_xBinCenter, max_xBinCenter));
	h1dSliceX->Draw();
	TCanvas *c3 = new TCanvas("c3", "Beam Profile in Y", 500, 500);
	c3->cd();
	TH1D *h1dSliceY = beamProfile->ProjectionY("h1dSliceY", 54, 54);
	Float_t yBinCenter = beamProfile->GetYaxis()->GetBinCenter(54);
	Float_t yBinWidth = beamProfile->GetYaxis()->GetBinWidth(54);
	Float_t min_yBinCenter = yBinCenter - yBinWidth/2;
	Float_t max_yBinCenter = yBinCenter + yBinWidth/2;
	h1dSliceY->SetTitle(Form("h1dSliceY[%.3f, %.3f];y (cm);", min_yBinCenter, max_yBinCenter));
	h1dSliceY->GetXaxis()->SetTitleOffset(1.0);
	h1dSliceY->Draw();
}