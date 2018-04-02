void fill2dHistoFromTree(TTree* tree, TH2D* histo, Double_t factor){
	Double_t prePosX, prePosY, prePosZ;
	Int_t pid;
	tree->SetBranchAddress("pid", &pid);
	tree->SetBranchAddress("prePosX", &prePosX);
	tree->SetBranchAddress("prePosY", &prePosY);
	tree->SetBranchAddress("prePosZ", &prePosZ);
	tree->Draw(">>list", "pid==2212 && prePosZ==-522.5", "entrylist");
	TEntryList *list = (TEntryList*)gDirectory->Get("list");
	Double_t nEntries = list->GetN();
	Int_t nloop = nEntries*factor;

	for (int i = 0; i < nloop; ++i)
	{
		int index = list->GetEntry(i);
		tree->GetEntry(index);
		histo->Fill(prePosX, prePosY, 1);
	}
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
TTree *getTree(TFile *file, int treeId){
	TTree *tree = (TTree*)file->Get(Form("ntuple/%d", treeId));
	cout << "Tree ID: 203 (Collimator2 hole volume) is loaded!" << endl;
	cout << "Total Entry: " << tree->GetEntries() << endl;
	return tree;
}
TFile *getFile(TString name){
	TFile *file = TFile::Open(Form("%s.root", name.Data()));
	return file;
}
void makeBeamProfile(TString path = "../build/", TString file_name = "run", Double_t percent=100){
	gStyle->SetOptStat(0);
	gStyle->SetPalette(kRainBow);
	// Get file & Tree
	TFile *file = getFile(Form("%s/%s", path.Data(), file_name.Data()));
	TTree *t_col2_hole = getTree(file, 203);
	TFile *save = new TFile(Form("result/beamProfile/%s_factor_%.0fpercent.root", file_name.Data(), percent), "RECREATE");

	// ---------------------------------------------------------------------------
	// Collimaotr2(Au foil input)
	// ---------------------------------------------------------------------------
	TCanvas *cColl2Profile = getCanvas(0, "Coll2Profile");
	cColl2Profile->cd();
	TH2D *h2d_coll2Profile = new TH2D("h2d_coll2Profile", ";x (mm);y (mm)", 110, -27.75, 27.75, 110, -27.75, 27.75);
	Double_t factor = percent/100;
	cout << factor << endl;
	fill2dHistoFromTree(t_col2_hole, h2d_coll2Profile, factor);
	h2d_coll2Profile->Draw("colz");
	save->cd();
	h2d_coll2Profile->Write();
	
}