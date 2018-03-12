Float_t getAbsorbedDose(Float_t total_deposit, Float_t material_mass)
{
	Float_t jtomev = 1.6*TMath::Power(10, -13);
	Float_t gtokg = TMath::Power(10,3);

	Float_t result = total_deposit/material_mass*jtomev*gtokg;
	return result;
}
TGraph* getStoppingPower(int debug=0)
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
		gstopping->Draw("APL");
		cout << "Debug mode output" << endl;
		cout << "1.00MeV: 1.753E+02(data) / " << gstopping->Eval(1) << endl;
		cout << "1.10MeV: NO DATA         / " << gstopping->Eval(1.1) << endl;
		cout << "1.25MeV: 1.523E+02(data) / " << gstopping->Eval(1.25) << endl;
	}
	return gstopping;
}

void calculation(Float_t thickness=0.01) // thickness unit: cm
{
	int debug=0;

	Float_t silicon_density = 2.33; // unit: g/cm^2
	Float_t proton_energy = 20; // unit: 20MeV
	Float_t area = 2.0 * 3.6; // unit: cm^2
	Float_t mass = silicon_density*area*thickness; // unit: g
	Float_t step_size = 0.0001; // unit: cm
	Float_t total_deposit = 0; // unit: MeV

	TGraph * stoppingPower = getStoppingPower(debug);

	cout << "Thickness: " << thickness << "cm" << endl;
	cout << "Step size: " << step_size << endl;

	Int_t iloop = 0;
	Float_t penetration_length = 0;

	// Energy loss
	while(1)
	{
		cout << "loop#: " << iloop << " / ";
		cout << "Location: " << penetration_length << "/" << thickness;
		cout << "Proton energy: " << proton_energy << " MeV / ";

		Float_t current_stopping = stoppingPower->Eval(proton_energy);
		Float_t dedx = current_stopping*silicon_density;
		Float_t deposit_energy = dedx*step_size;
		total_deposit += deposit_energy;
		proton_energy = proton_energy-deposit_energy;
		penetration_length += step_size;
		if (penetration_length>thickness)
		{
			cout << "Proton is outside of silicon, loop is stopped" << endl;
			break;
		}
		if(proton_energy <0){
			cout << "Energy is minus, loop is stopped" << endl;
			break;
		}
		cout << "dE/dx: " << dedx << " MeV/cm / Step size: " << step_size << " / Deposit Energy: " << deposit_energy << " / Total deposit: " << total_deposit << endl; 
		iloop++;
	}

	Float_t absorbed_dose = 0; // unit: rad	
	absorbed_dose = getAbsorbedDose(total_deposit, mass);
	cout << "Absorbed dose: " << absorbed_dose << " Gy" << endl;
}