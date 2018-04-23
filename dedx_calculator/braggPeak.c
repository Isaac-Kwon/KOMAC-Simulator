#include "atlas_style/AtlasStyle.C"
TGraph* getTotalStoppingPower(int debug=0, TString fileName = "pstar_silicon.dat")
{

	ifstream in;
	in.open(fileName.Data());

	Float_t energy, electronic_stopping, nuclear_stopping, total_stopping;
	string dump;

	TGraph *gstopping = new TGraph();

	int line = 0;
	while (1) {
		if (line<8) {
			getline(in, dump);
			if(debug>0) cout << dump << endl;
			line++;
			continue;
		}
		if (!in.good()) break;
		in >> energy >> electronic_stopping >> nuclear_stopping >> total_stopping;
		if(debug>0) cout << energy << " " << electronic_stopping << " " << nuclear_stopping << " " << total_stopping << endl;
		gstopping->SetPoint(line-8, energy, total_stopping);
		line++;
	}
	if(debug>0) {
		TCanvas *c_total_stopping = new TCanvas("c_total_stopping", "Total stopping power", 800, 600);
		c_total_stopping->SetLogx();
		c_total_stopping->SetLogy();
		c_total_stopping->cd();
		gstopping->Draw("APL");
		gstopping->SetMarkerStyle(24);
		gstopping->SetTitle(";Proton Energy (MeV);Stopping Power (MeV cm^{2}/g");
		cout << "Debug mode output" << endl;
		cout << "1.00MeV: 1.754E+02(data) / " << gstopping->Eval(1) << endl;
		cout << "1.10MeV: NO DATA         / " << gstopping->Eval(1.1) << endl;
		cout << "1.25MeV: 1.524E+02(data) / " << gstopping->Eval(1.25) << endl;
	}
	return gstopping;
}
void braggPeak()
{
	SetAtlasStyle();
	int debug = 0;
	Float_t density = 1.19; // unit: g/cm^2
	Float_t input_proton_energy[3] = {8, 12, 20};
	TString materialFile = "pstar_plexiglass.dat";

	TGraph * tStoppingPower = getTotalStoppingPower(debug, materialFile);

	Int_t iloop = 0;
	Float_t penetration_length = 0;
	Float_t step_size = 0.00001; // unit: cm
	Float_t proton_energy = 0;

	TGraph *g_result[3];
	for (int i = 0; i < 3; ++i)
	{
		g_result[i] = new TGraph();
	}
	// Energy loss
	for (int i = 0; i < 3; ++i)
	{
		proton_energy = input_proton_energy[i];
		penetration_length = 0;
		iloop = 0;
		while(1)
		{
			if(debug>0){
				cout << "loop#: " << iloop;
				cout << "\t|\tLocation: " << penetration_length;
				cout << "\t|\tProton energy: " << proton_energy << " MeV\t|\t";				
			}

			// Total energy loss(Electronic + Nuclear)
			Float_t current_stopping_total = tStoppingPower->Eval(proton_energy);
			Float_t dedx_total = current_stopping_total*density;
			Float_t energy_loss_total = dedx_total*step_size;

			g_result[i]->SetPoint(iloop, penetration_length, energy_loss_total);
			proton_energy = proton_energy-energy_loss_total;
			penetration_length += step_size;

			if(proton_energy <0){
				cout << endl;
				cout << "---------------------------------------------" << endl;
				cout << "Energy is minus, loop is stopped" << endl;
				break;
			}
			if(debug>0) cout << "dE/dx: " << dedx_total << " MeV/cm\t|\tDeposit Energy: " << energy_loss_total << " MeV" << endl; 
			iloop++;
		}
	}

	TMultiGraph *mg = new TMultiGraph();
	g_result[0]->SetMarkerStyle(20);
	g_result[0]->SetMarkerColor(kBlack);
	g_result[0]->SetMarkerSize(0.3);
	g_result[0]->SetLineColor(kBlack);
	mg->Add(g_result[0]);
	g_result[1]->SetMarkerStyle(20);
	g_result[1]->SetMarkerColor(kRed);
	g_result[1]->SetMarkerSize(0.3);
	g_result[1]->SetLineColor(kRed);
	mg->Add(g_result[1]);
	g_result[2]->SetMarkerStyle(20);
	g_result[2]->SetMarkerColor(kBlue);
	g_result[2]->SetMarkerSize(0.3);
	g_result[2]->SetLineColor(kBlue);
	mg->Add(g_result[2]);

	TCanvas *c_result = new TCanvas("c_result", "Result", 800, 600);
	c_result->cd();
	mg->Draw("APL");
	mg->SetTitle(";Penetration length (cm);Energy Loss in step (MeV)");
	mg->GetXaxis()->SetLimits(0,0.5);
	TLegend *legend = new TLegend(0.1,0.7,0.48,0.9);
	legend->SetHeader("Step size = 0.00001 cm | Plexiglass density = 1.19 g/cm^{2}");
	legend->AddEntry(g_result[0],"Kinetic Energy =  8 MeV","lp");
	legend->AddEntry(g_result[1],"Kinetic Energy = 12 MeV","lp");
	legend->AddEntry(g_result[2],"Kinetic Energy = 20 MeV","lp");
	legend->Draw();

	TLine *line = new TLine(0.07, 0, 0.07, 0.0115);
	line->SetLineStyle(2);
	line->SetLineColor(46);
	line->Draw();

}