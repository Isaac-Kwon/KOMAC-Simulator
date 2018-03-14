Float_t getAbsorbedDose(Float_t total_deposit, Float_t material_mass)
{
	Float_t jtomev = 1.6*TMath::Power(10, -13);
	Float_t gtokg = TMath::Power(10,3);

	Float_t result = total_deposit/material_mass*jtomev*gtokg;
	return result;
}
TGraph* getElectronicStoppingPower(int debug=0)
{

	ifstream in;
	in.open("pstar_silicon.dat");

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
TGraph* getNuclearStoppingPower(int debug=0)
{

	ifstream in;
	in.open("pstar_silicon.dat");

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


TGraph* getTotalStoppingPower(int debug=0)
{

	ifstream in;
	in.open("pstar_silicon.dat");

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

void calculation(Int_t debug=0, Float_t thickness=0.01) // thickness unit: cm
{
	Float_t silicon_density = 2.33; // unit: g/cm^2
	Float_t proton_energy = 14.0798; // unit: 20MeV
	Float_t area = 2.0 * 3.6; // unit: cm^2
	Float_t mass = silicon_density*area*thickness; // unit: g
	Float_t step_size = 0.0001; // unit: cm
	Float_t total_deposit = 0; // unit: MeV

	TGraph * eStoppingPower = getElectronicStoppingPower(debug);
	TGraph * nStoppingPower = getNuclearStoppingPower(debug);
	TGraph * tStoppingPower = getTotalStoppingPower(debug);

	cout << "Thickness: " << thickness << "cm" << endl;
	cout << "Step size: " << step_size << endl;

	Int_t iloop = 0;
	Float_t penetration_length = 0;

	TGraph *gCal_eStoppingPower = new TGraph();
	TGraph *gCal_eDedx = new TGraph();
	TGraph *gCal_eDeposit = new TGraph();
	TGraph *gCal_eDeposit_scale = new TGraph();

	// Energy loss
	while(1)
	{
		cout << "loop#: " << iloop;
		cout << "\t|\tLocation: " << penetration_length << "/" << thickness;
		cout << "\t|\tProton energy: " << proton_energy << " MeV\t|\t";

		// Total energy loss(Electronic + Nuclear)
		Float_t current_stopping_total = tStoppingPower->Eval(proton_energy);
		Float_t dedx_total = current_stopping_total*silicon_density;
		Float_t energy_loss_total = dedx_total*step_size;

		// Electronic stopping power(i.e. inelastic)
		Float_t current_stopping_electronic = eStoppingPower->Eval(proton_energy);
		Float_t dedx_electronic = current_stopping_electronic*silicon_density;
		Float_t deposit_energy = dedx_electronic*step_size;
		total_deposit += deposit_energy;

		gCal_eStoppingPower->SetPoint(iloop, proton_energy, current_stopping_electronic);
		gCal_eDedx->SetPoint(iloop, proton_energy, dedx_electronic);
		gCal_eDeposit->SetPoint(iloop, proton_energy, deposit_energy);
		Float_t scale_factor = TMath::Power(step_size*silicon_density,-1);
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
		c_qa->SetLogx();
		c_qa->SetLogy();
		c_qa->cd();
		TMultiGraph *mgr_qa = new TMultiGraph();
		eStoppingPower->SetMarkerStyle(24);
		eStoppingPower->SetMarkerSize(0.5);
		mgr_qa->Add(eStoppingPower);
		gCal_eStoppingPower->SetMarkerStyle(20);
		gCal_eStoppingPower->SetMarkerSize(0.3);
		gCal_eStoppingPower->SetMarkerColor(kBlue);
		mgr_qa->Add(gCal_eStoppingPower);
		gCal_eDeposit->SetMarkerStyle(20);
		gCal_eDeposit->SetMarkerSize(0.3);
		gCal_eDeposit->SetMarkerColor(kRed);
		mgr_qa->Add(gCal_eDeposit);
		gCal_eDeposit_scale->SetMarkerStyle(24);
		gCal_eDeposit_scale->SetMarkerSize(0.7);
		gCal_eDeposit_scale->SetMarkerColor(kRed);
		mgr_qa->Add(gCal_eDeposit_scale);

		mgr_qa->SetTitle(";Proton Energy (MeV);Stopping Power");

		mgr_qa->Draw("AP");
	}
}