Float_t getAbsorbedDose(Float_t total_deposit, Float_t material_mass)
{
	Float_t jtomev = 1.6*TMath::Power(10, -13);
	Float_t gtokg = TMath::Power(10,3);

	Float_t result = total_deposit/material_mass*jtomev*gtokg;
	return result;
}
TGraph* getElectronicStoppingPower(int debug=0, TString fileName = "pstar_silicon.dat")
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
		gstopping->SetPoint(line-8, energy, electronic_stopping);
		line++;
	}
	if(debug>0) {
		TCanvas *c_electronic_stopping = new TCanvas("c_electronic_stopping", "Electronic stopping power", 800, 600);
		c_electronic_stopping->SetLogx();
		c_electronic_stopping->SetLogy();
		c_electronic_stopping->cd();

		gstopping->Draw("APL");
		gstopping->SetMarkerStyle(24);
		gstopping->SetTitle(";Proton Energy (MeV);Stopping Power");		
		cout << "Debug mode output" << endl;
		cout << "1.00MeV: 1.753E+02(data) / " << gstopping->Eval(1) << endl;
		cout << "1.10MeV: NO DATA         / " << gstopping->Eval(1.1) << endl;
		cout << "1.25MeV: 1.523E+02(data) / " << gstopping->Eval(1.25) << endl;
	}
	return gstopping;
}
TGraph* getElectronicStoppingPowerRange(TString fileName = "pstar_silicon.dat", Float_t range1=0, Float_t range2=100)
{

	cout << range1 << " " << range2 << endl;
	Int_t range1_int = range1;
	Int_t range2_int = range2;
	Int_t range1_digit_decimal = range1_int/10;
	Int_t range2_digit_decimal = range2_int/10;
	cout << range1_digit_decimal << " " << range2_digit_decimal << endl;

	Float_t range1_significant = range1/TMath::Power(10, range1_digit_decimal);
	Float_t range2_significant = range2/TMath::Power(10, range2_digit_decimal);
	cout << range1_significant << " " << range2_significant << endl;

	Float_t step = 0.25;
	Float_t nstep = 2;
	Float_t range1_processed;
	Float_t range2_processed;

	if(step<range1_significant){
		cout << "Step size" <<endl;
		range1_significant = range1_significant - step;
		range1_processed = range1_significant*TMath::Power(10, range1_digit_decimal);
	}
	else{
		range1_processed = range1_significant*TMath::Power(10, range1_digit_decimal) - step*TMath::Power(10, range1_digit_decimal-1);
	}
	cout << range1_significant << " " << range1_processed << endl;
		
	if(step+range2_significant < 10){
		cout << "Step size is larger" <<endl;
		range2_significant = range2_significant + step;
		range2_processed = range2_significant*TMath::Power(10, range2_digit_decimal);
	}
	else{
		range2_processed = range2_significant*TMath::Power(10, range2_digit_decimal) + step*TMath::Power(10, range2_digit_decimal+1);
	}
	cout << range2_significant << " " << range2_processed << endl;

	cout << range1_processed << " " << range2_processed << endl;

	ifstream in;
	in.open(fileName.Data());

	Float_t energy, electronic_stopping, nuclear_stopping, total_stopping;
	string dump;

	TGraph *gstopping = new TGraph();

	int line = 0;
	while (1) {
		if (line<8) {
			getline(in, dump);
			line++;
			continue;
		}
		if (!in.good()) break;
		in >> energy >> electronic_stopping >> nuclear_stopping >> total_stopping;
		if(energy>=range1_processed && energy<=range2_processed) {
			cout << "working" << endl;
			gstopping->SetPoint(line-8, energy, electronic_stopping);
			line++;
		}
	}
	return gstopping;
}
TGraph* getNuclearStoppingPower(int debug=0, TString fileName = "pstar_silicon.dat")
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
		gstopping->SetPoint(line-8, energy, nuclear_stopping);
		line++;
	}
	if(debug>0) {
		TCanvas *c_nuclear_stopping = new TCanvas("c_nuclear_stopping", "Nuclear stopping power", 800, 600);
		c_nuclear_stopping->SetLogx();
		c_nuclear_stopping->SetLogy();
		c_nuclear_stopping->cd();		
		gstopping->Draw("APL");
		gstopping->SetMarkerStyle(24);	
		gstopping->SetTitle(";Proton Energy (MeV);Stopping Power");			
		cout << "Debug mode output" << endl;
		cout << "1.00MeV: 1.385E-01(data) / " << gstopping->Eval(1) << endl;
		cout << "1.10MeV: NO DATA         / " << gstopping->Eval(1.1) << endl;
		cout << "1.25MeV: 1.143E-01(data) / " << gstopping->Eval(1.25) << endl;
	}
	return gstopping;
}


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
		gstopping->SetTitle(";Proton Energy (MeV);Stopping Power");
		cout << "Debug mode output" << endl;
		cout << "1.00MeV: 1.754E+02(data) / " << gstopping->Eval(1) << endl;
		cout << "1.10MeV: NO DATA         / " << gstopping->Eval(1.1) << endl;
		cout << "1.25MeV: 1.524E+02(data) / " << gstopping->Eval(1.25) << endl;
	}
	return gstopping;
}
// material list
// 0: air
// 1: silicon
void calculation(Int_t debug=0, Float_t thickness=0.01, Float_t proton_energy=20, Int_t material = 1) // thickness unit: cm, proton energy unit: MeV
{
	Float_t density = 0; // unit: g/cm^2
	Float_t input_proton_energy = proton_energy;
	TString materialFile;
	switch(material){
		case 0:
			density = 1.20479E-03;
			materialFile = "pstar_air.dat";
			break;
		case 1:
			density = 2.33;
			materialFile = "pstar_silicon.dat";
			break;
		case 2:
			density = 1.93200E+01;
			materialFile = "pstar_gold.dat";
			break;
	}
	Float_t area = 2.0 * 3.6; // unit: cm^2
	Float_t mass = density*area*thickness; // unit: g
	Float_t step_size = 0.0001; // unit: cm
	Float_t total_deposit = 0; // unit: MeV

	TGraph * eStoppingPower = getElectronicStoppingPower(debug, materialFile);
	TGraph * nStoppingPower = getNuclearStoppingPower(debug, materialFile);
	TGraph * tStoppingPower = getTotalStoppingPower(debug, materialFile);

	cout << "Thickness: " << thickness << " cm" << endl;
	cout << "Step size: " << step_size << " cm" << endl;

	Int_t iloop = 0;
	Float_t penetration_length = 0;

	TGraph *gCal_eStoppingPower = new TGraph();
	TGraph *gCal_eDedx = new TGraph();
	TGraph *gCal_eDeposit = new TGraph();
	TGraph *gCal_eDeposit_scale = new TGraph();
	TGraph *gCal_used_stopping = new TGraph();

	// Energy loss
	while(1)
	{
		cout << "loop#: " << iloop;
		cout << "\t|\tLocation: " << penetration_length;
		cout << "\t|\tProton energy: " << proton_energy << " MeV\t|\t";

		// Total energy loss(Electronic + Nuclear)
		Float_t current_stopping_total = tStoppingPower->Eval(proton_energy);
		Float_t dedx_total = current_stopping_total*density;
		Float_t energy_loss_total = dedx_total*step_size;

		// Electronic stopping power(i.e. inelastic)
		Float_t current_stopping_electronic = eStoppingPower->Eval(proton_energy);
		Float_t dedx_electronic = current_stopping_electronic*density;
		Float_t deposit_energy = dedx_electronic*step_size;
		total_deposit += deposit_energy;

		gCal_eStoppingPower->SetPoint(iloop, proton_energy, current_stopping_electronic);
		gCal_eDedx->SetPoint(iloop, proton_energy, dedx_electronic);
		gCal_eDeposit->SetPoint(iloop, proton_energy, deposit_energy);
		Float_t scale_factor = TMath::Power(step_size*density,-1);
		gCal_eDeposit_scale->SetPoint(iloop, proton_energy, deposit_energy*scale_factor);
		proton_energy = proton_energy-energy_loss_total;
		penetration_length += step_size;

		if (penetration_length>thickness)
		{
			cout << endl;
			cout << "---------------------------------------------" << endl;
			cout << "Proton is outside of silicon, loop is stopped" << endl;
			break;
		}
		if(proton_energy <0){
			cout << endl;
			cout << "---------------------------------------------" << endl;
			cout << "Energy is minus, loop is stopped" << endl;
			break;
		}
		cout << "dE/dx: " << dedx_electronic << " MeV/cm\t|\tDeposit Energy: " << deposit_energy << " MeV\t|\tTotal deposit: " << total_deposit << " MeV"<< endl; 
		iloop++;
	}

	Float_t absorbed_dose = 0; // unit: rad	
	absorbed_dose = getAbsorbedDose(total_deposit, mass);
	cout << "Absorbed dose: " << absorbed_dose << " Gy" << endl;

	if(debug){
		TCanvas *c_qa = new TCanvas("c_qa", "QA plots", 800, 600);
		c_qa->cd();
		TMultiGraph *mgr_qa = new TMultiGraph();
		if (proton_energy >0){
			gCal_used_stopping = getElectronicStoppingPowerRange(materialFile, proton_energy, input_proton_energy);
			gCal_used_stopping->SetMarkerStyle(24);
			gCal_used_stopping->SetMarkerSize(1);
			mgr_qa->Add(gCal_used_stopping);
		}
		gCal_eStoppingPower->SetMarkerStyle(24);
		gCal_eStoppingPower->SetMarkerSize(0.7);
		gCal_eStoppingPower->SetMarkerColor(kBlue);
		mgr_qa->Add(gCal_eStoppingPower);

		mgr_qa->SetTitle(";Proton Energy (MeV);Stopping Power(MeV cm^{2}/g)");

		mgr_qa->Draw("AP");
	}
}