void getBeamProfile(){
	Int_t debug = 1;
	gStyle->SetOptStat(1);
	gStyle->SetPalette(kRainBow);
	ifstream in;
	in.open("mountWindow_beamProfile.csv");
	Float_t count;
	Float_t x=0;
	Float_t y=0;

	Int_t nbinx = 200;
	Int_t nbiny = 200;
	TH2F *beamProfile = new TH2F("h2dBeamProfile", "Beam Profile", nbinx, -112.25, -12.25, nbiny, -49.75, 50.25);

	Int_t nlines = 0;
	TString cell, correctedCell;
	for (int iY = 0; iY < 200; ++iY){
		x = 0;
		for (int iX = 0; iX < 200; ++iX)
		{
			if (!(iX==199)) cell.ReadToDelim(in, ',');
			else cell.ReadToDelim(in);
			Float_t count = cell.Atof();
			int binx = beamProfile->GetXaxis()->FindBin(x-12.5);
			int biny = beamProfile->GetYaxis()->FindBin(y+50);
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

	cout << "Ratio of dose in target area" << endl;
	Double_t xStart_value = -70-7.5;
	Double_t xEnd_value = -70+7.5;
	Double_t yStart_value = -15;
	Double_t yEnd_value = 15;
	Int_t xStart = beamProfile->GetXaxis()->FindBin(xStart_value);
	Int_t xEnd = beamProfile->GetXaxis()->FindBin(xEnd_value);
	Int_t yStart = beamProfile->GetYaxis()->FindBin(yStart_value);
	Int_t yEnd = beamProfile->GetYaxis()->FindBin(yEnd_value);
	Double_t total = beamProfile->Integral();
	Double_t target = beamProfile->Integral(xStart, xEnd, yStart, yEnd);
	cout << "Target/Total = " << target << "/" << total << " = " << target/total << endl;

	Double_t dose_target = 0;
	Double_t dose_target2 = 0;
	Int_t denominator = 0;
	TH1D *h_dose_target = new TH1D("h_dose_target", ";Dose in EBT film (Gy);", 100, 5, 15);
	for (int iX_bin = xStart; iX_bin < xEnd+1; ++iX_bin)
	{
		for (int iY_bin = yStart; iY_bin < yEnd+1; ++iY_bin)
		{
			Double_t binValue = beamProfile->GetBinContent(iX_bin, iY_bin);
			dose_target += binValue;
			dose_target2 = TMath::Power(binValue, 2);
			h_dose_target->Fill(binValue);
			denominator++;
		}
	}
	TCanvas *c_dose_target = new TCanvas("c_dose_target", "Dose distribution in Target", 800, 600);
	c_dose_target->cd();
	h_dose_target->Draw();
	Double_t mean_dose_target = dose_target/denominator;
	Double_t rms_dose_target2 = dose_target2/denominator;
	Double_t rms_dose_target = TMath::Sqrt(rms_dose_target2);

	cout << "Beam Uniformity in target area" << endl;
	cout << "Mean: " << mean_dose_target << " RMS: " << rms_dose_target << endl;
	cout << "Mean/RMS in percent" << rms_dose_target/mean_dose_target << "%" << endl;
	cout << "Histogram Info | Mean: " << h_dose_target->GetMean() << " | RMS: " << h_dose_target->GetRMS() << endl;
	Double_t firstBin = h_dose_target->FindFirstBinAbove();
	Double_t lastBin = h_dose_target->FindLastBinAbove();
	Double_t firstBin_val = h_dose_target->GetBinCenter(firstBin);
	Double_t lastBin_val = h_dose_target->GetBinCenter(lastBin);
	Double_t uniformity = (lastBin_val - firstBin_val)/(2*mean_dose_target)*100;
	cout << "Uniformity: " << uniformity << "%" << endl;



	int drawBox = 0;
	if(drawBox >0){
		TBox *integral_area = new TBox(xStart_value, yStart_value, xEnd_value, yEnd_value);
		integral_area->SetFillStyle(3344);
		integral_area->SetFillColorAlpha(kBlack, 0.35);
		integral_area->Draw();
		TBox *integral_area_line = new TBox(xStart_value, yStart_value, xEnd_value, yEnd_value);
		integral_area_line->SetFillStyle(0);
		integral_area_line->SetLineColor(kBlack);
		integral_area_line->SetLineWidth(1);
		integral_area_line->Draw();		
	}


	TCanvas *c2 = new TCanvas("c2", "Beam Profile in X", 500, 500);
	c2->cd();
	TH1D *h1dSliceX = beamProfile->ProjectionX("sliceX", 100, 101);
	Float_t min_xBinCenter = beamProfile->GetXaxis()->GetBinCenter(100);
	Float_t max_xBinCenter = beamProfile->GetXaxis()->GetBinCenter(101);
	Float_t min_xBinWidth = beamProfile->GetXaxis()->GetBinWidth(100);
	Float_t max_xBinWidth = beamProfile->GetXaxis()->GetBinWidth(101);
	Float_t min_x = min_xBinCenter - min_xBinWidth;
	Float_t max_x = max_xBinCenter + max_xBinWidth;
	cout << min_xBinWidth << endl;
	h1dSliceX->SetTitle(Form("h1dSliceX[%.3f, %.3f];x (mm);", min_x, max_x));
	h1dSliceX->Draw();
	TCanvas *c3 = new TCanvas("c3", "Beam Profile in Y", 500, 500);
	c3->cd();
	TH1D *h1dSliceY = beamProfile->ProjectionY("h1dSliceY", 100, 101);
	Float_t min_yBinCenter = beamProfile->GetXaxis()->GetBinCenter(100);
	Float_t max_yBinCenter = beamProfile->GetXaxis()->GetBinCenter(101);
	Float_t min_yBinWidth = beamProfile->GetXaxis()->GetBinWidth(100);
	Float_t max_yBinWidth = beamProfile->GetXaxis()->GetBinWidth(101);
	Float_t min_y = min_yBinCenter - min_yBinWidth;
	Float_t max_y = max_yBinCenter + max_yBinWidth;
	cout << min_yBinWidth << endl;
	h1dSliceY->SetTitle(Form("h1dSliceY[%.3f, %.3f];y (mm);", min_y, max_y));
	h1dSliceY->GetXaxis()->SetTitleOffset(1.0);
	h1dSliceY->Draw();

	TCanvas *c4 = new TCanvas("c4", "Beam Profile_rebin", 500, 500);
	c4->cd();
	TH2F *beamProfile_rebin = (TH2F*)beamProfile->Clone();
	beamProfile_rebin->RebinX(11);
	beamProfile_rebin->RebinY(11);
	Float_t gmax = beamProfile_rebin->GetMaximum();
	Float_t scale = TMath::Power(gmax, -1);
	beamProfile_rebin->Scale(scale);
	beamProfile_rebin->Draw("colztext");
	beamProfile_rebin->SetTitle("BeamProfile;x (mm);y (mm)");


}