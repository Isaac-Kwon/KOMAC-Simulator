void resultScan(){

	TGraphErrors *gr = new TGraphErrors();
	TGraphErrors *gr2 = new TGraphErrors();
	int counter = 0;
	for (int i = 0; i < 100; ++i)
	{
		TFile *file = TFile::Open(Form("result/run_101/run_%02d_mm.root", i));
		TTree *tree = (TTree*)file->Get("ntuple/201");
		TTree *tree2 = (TTree*)file->Get("ntuple/202");
		TTree *tree3 = (TTree*)file->Get("ntuple/203");
		float nDet = 0;
		float nColl2 = 0;
		tree->Draw(Form(">>list%d", i), "pid==2212", "entrylist");
		TEntryList *list = (TEntryList*)gDirectory->Get(Form("list%d", i));

		tree3->Draw(Form(">>list3%d", i), "pid==2212 && prePosZ == -522.5", "entrylist");
		TEntryList *list3 = (TEntryList*)gDirectory->Get(Form("list3%d", i));
		if (list) nDet = list->GetN();
		if (list3) nColl2 = list3->GetN();
		if(nDet>0) {
			cout << "File ID: " << Form("%02d", i) << " Number: " << nDet << " nColl2: " << nColl2 << endl;
			float nDet_sigma = TMath::Sqrt(nDet);
			float nColl2_sigma = TMath::Sqrt(nColl2);

			gr->SetPoint(counter, i+0.5, nDet);
			gr->SetPointError(counter, 0.5, nDet_sigma);

			gr2->SetPoint(counter, i+0.5, nColl2);
			gr2->SetPointError(counter, 0.5, 0);
			cout << ratio << " " << ratio_sigma << endl;
			counter++;
		}
	}
	TCanvas *c1 = new TCanvas("c1", "Result", 800, 600);
	c1->cd();
	gr->SetTitle(";radius (mm);Number of proton in detector");
	gr->Draw("AP");
	TCanvas *c2 = new TCanvas("c2", "Result", 800, 600);
	c2->cd();
	gr2->SetTitle(";radius (mm);Number of proton in collimator2");
	gr2->Draw("AP");
}