void resultScan(){

	for (int i = 0; i < 100; ++i)
	{
		TFile *file = TFile::Open(Form("../build/run_%02d_mm.root", i));
		TTree *tree = (TTree*)file->Get("ntuple/201");
		int checker = 0;
		checker = tree->GetEntries();
		if(checker>0) cout << "File ID: " << Form("%02d", i) << " Number: " << checker << endl;
	}
}