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
Double_t check(TH2D* histo, Int_t nxbin = 10, Int_t nybin = 10){
	Double_t sum = 0;
	for (int i = 0; i < nxbin; ++i)
	{
		for (int j = 0; j < nybin; ++j)
		{
			sum += histo->GetBinContent(i+1,j+1);
		}
	}
	return sum;
}
void qa_inputHist(TString name, TH2D* histo, Int_t n){
	TH2D *histo2 = copyHisto(histo);
	rebin(histo2, n);
	TCanvas *c = new TCanvas(name.Data(), name.Data(), 500,500);
	c->cd();
	histo2->Draw("colztext");
	c->SaveAs(Form("%s.pdf", name.Data()));
}
void normalisation(TString name, TH2D* histo, Int_t n, Double_t height){
	TH2D *histo2 = copyHisto(histo);
	rebin(histo2, n);
	scaling(histo2);
	cout << "Checking: " << check(histo2) << endl;
	TCanvas *c = new TCanvas(name.Data(), name.Data(), 500,500);
	c->cd();
	histo2->Draw("colztext");
	if(!(height==0)) histo2->SetMaximum(height);
	c->SaveAs(Form("%s.pdf", name.Data()));
}
void substraction(TString name, TH2D* histo1, TH2D* histo2, Int_t n, Double_t max=0, Double_t min=0){

	TH2D* histo_cp[2];
	histo_cp[0] = copyHisto(histo1);
	histo_cp[1] = copyHisto(histo2);
	rebin(histo_cp[0], n);
	scaling(histo_cp[0]);
	rebin(histo_cp[1], n);
	scaling(histo_cp[1]);

	histo_cp[0]->Add(histo_cp[1], -1);

	TCanvas *c = new TCanvas(name.Data(), name.Data(), 500, 500);
	histo_cp[0]->Draw("colztext");
	if(!(max==0)) histo_cp[0]->SetMaximum(max);
	if(!(min==0)) histo_cp[0]->SetMinimum(min);

}
void histoDivide(TString name, TH2D* histo1, TH2D* histo2, Int_t n, Double_t max=0, Double_t min=0){

	TH2D* histo_cp[2];
	histo_cp[0] = copyHisto(histo1);
	histo_cp[1] = copyHisto(histo2);
	rebin(histo_cp[0], n);
	scaling(histo_cp[0]);
	rebin(histo_cp[1], n);
	scaling(histo_cp[1]);

	histo_cp[0]->Divide(histo_cp[1]);

	TCanvas *c = new TCanvas(name.Data(), name.Data(), 500, 500);
	histo_cp[0]->Draw("colztext");
	if(!(max==0)) histo_cp[0]->SetMaximum(max);
	if(!(min==0)) histo_cp[0]->SetMinimum(min);
}
void comparison(TString name, TH2D* histo1, TH2D* histo2, Int_t n, Double_t max=0, Double_t min=0){

	TH2D* histo_cp[3];
	histo_cp[0] = copyHisto(histo1);
	histo_cp[1] = copyHisto(histo2);
	histo_cp[2] = copyHisto(histo2);

	rebin(histo_cp[0], n);
	scaling(histo_cp[0]);
	rebin(histo_cp[1], n);
	scaling(histo_cp[1]);

	histo_cp[0]->Add(histo_cp[1], -1);
	histo_cp[0]->Divide(histo_cp[1]);
	histo_cp[0]->Scale(100);

	TCanvas *c = new TCanvas(name.Data(), name.Data(), 500, 500);
	histo_cp[0]->Draw("colztext");
	if(!(max==0)) histo_cp[0]->SetMaximum(max);
	if(!(min==0)) histo_cp[0]->SetMinimum(min);
}
// Double_t getSimilarity(TString name, TH2D* histo1, TH2D* histo2, Int_t n){

// 	TH2D* histo_cp[2];
// 	histo_cp[0] = copyHisto(histo1);
// 	histo_cp[1] = copyHisto(histo2);
// 	rebin(histo_cp[0], n);
// 	scaling(histo_cp[0]);
// 	rebin(histo_cp[1], n);
// 	scaling(histo_cp[1]);
// }
void ana_diff(){

	gStyle->SetOptStat(0);
	gStyle->SetPalette(kRainBow);

	TFile *file_komac  = TFile::Open("komac_beam_profile/AuFoil/result_beamProfile.root");
	TFile *file_column_4 = TFile::Open("result/sim_17MeV/run_301/test/save_run1_column4.root");
	TFile *file_column_34 = TFile::Open("result/sim_17MeV/run_301/test/save_run1_column34.root");
	TFile *file_column_234 = TFile::Open("result/sim_17MeV/run_301/test/save_run1_column234.root");
	TFile *file_column_full = TFile::Open("result/sim_17MeV/run_301/test/save_run1_columnFull.root");
	// TFile *file_target = TFile::Open("result/sim_17MeV/run_403/save_run_403.root");
	TFile *file_target = TFile::Open("result/sim_17MeV/run_404/save_run_404.root");

	TFile *file_before = TFile::Open("result/run_507/save_run_507.root");

	Int_t n = 11;
	// Histogram QA
	TH2D *hKOMAC = (TH2D*)file_komac->Get("h2dBeamProfile");
	cout << "KOMAC Measurement" << endl;
	qa_inputHist("KOMAC_Initial_QA", hKOMAC, n);
	normalisation("KOMAC_Initial", hKOMAC, n, 0);
	// cout << "Simulation result: New" << endl;
	// TH2D *hSim_colFull = (TH2D*)file_column_full->Get("Collimator2/h2d_coll2Profile");
	// qa_inputHist("sim_colFull_QA", hSim_colFull, n);
	// normalisation("sim_colFull", hSim_colFull, n, 0);
	// cout << "Simulation result: Before" << endl;
	// TH2D *hSim_before = (TH2D*)file_before->Get("Collimator2/h2d_coll2Profile");
	// qa_inputHist("sim_before_QA", hSim_before, n);
	// normalisation("sim_before", hSim_before, n, 0);
	TH2D *hSim_target = (TH2D*)file_target->Get("Collimator2/h2d_coll2Profile");
	qa_inputHist("sim_target_QA", hSim_target, n);
	normalisation("sim_target", hSim_target, n, 0);

	// Substraction
	cout << "Comparison" << endl;
	Double_t max = 2.5;
	normalisation("Comp_KOMAC_Initial", hKOMAC, n, max);
	// normalisation("Comp_sim_colFull", hSim_colFull, n, max);
	// normalisation("Comp_sim_before", hSim_before, n, max);
	normalisation("Comp_sim_target", hSim_target, n, max);

	// cout << "Simulation_before - KOMAC" << endl;
	// substraction("sim_before-komac", hSim_before, hKOMAC, n, 0.6, -0.7);
	// histoDivide("ratio_sim_before-komac", hSim_before, hKOMAC, n, 2, 0);
	// comparison("diff_sim_before-komac", hSim_before, hKOMAC, n, 30, -30);
	// cout << "Simulation_new - KOMAC" << endl;
	// substraction("sim_colFull-komac", hSim_colFull, hKOMAC, n, 0.6, -0.7);
	// histoDivide("ratio_sim_colFull-komac", hSim_colFull, hKOMAC, n, 2, 0);
	// comparison("diff_sim_colFull-komac", hSim_colFull, hKOMAC, n, 30, -30);

	cout << "Simulation_target - KOMAC" << endl;
	substraction("sim_target-komac", hSim_target, hKOMAC, n, 0.6, -0.7);
	histoDivide("ratio_sim_target-komac", hSim_target, hKOMAC, n, 2, 0);
	comparison("diff_sim_target-komac", hSim_target, hKOMAC, n, 30, -30);

	// TH2D *hSim_col4 = (TH2D*)file_column_4->Get("Collimator2/h2d_coll2Profile");
	// qa_inputHist("sim_col4", hSim_col4, n);
	// TH2D *hSim_col34 = (TH2D*)file_column_34->Get("Collimator2/h2d_coll2Profile");
	// qa_inputHist("sim_col34", hSim_col34, n);
	// TH2D *hSim_col234 = (TH2D*)file_column_234->Get("Collimator2/h2d_coll2Profile");
	// qa_inputHist("sim_col234", hSim_col234, n);

	// substraction("subsctraction_sim_colFull-komac", hSim_colFull, hKOMAC, 11);


	// TH2D *histo2 = new TH2D(Form("hPiece_Step%d", i+1), ";x (mm);y (mm)", rebin0, -27.75, 27.75, rebin0, -27.75, 27.75);
}