// GUI input
// 0: No event display
// 1: OGS
// 2: HEPREP
// 3: DAWN (Not yet added in this code)
void gen_macro_piece(int gui =0, int runAcc = 0){ 

	std::ofstream ofs ("run_generated.mac", std::ofstream::out);

	float momentum = 194; //Unit: MeV

	ofs << "# KOMAC Simulator" << endl;
	ofs << "#" << endl;
	ofs << "# Initialize kernel" << endl;
	ofs << "/run/initialize" << endl;
	ofs << "# Particle Energy" << endl;
	ofs << Form("/tutorial/generator/momentum %f MeV", momentum) << endl;

	int maxBin = 10;

	for (int i = 0; i < 10; ++i)
	{
		ofs << "################################################" << endl;
		ofs << "#" << endl;
		ofs << "# cell coordinate: 3,4" << endl;
		ofs << "#" << endl;
		ofs << "# Particle position distribution in polar angle" << endl;
		ofs << "/tutorial/generator/PositionX -40 mm" << endl;
		ofs << "/tutorial/generator/PositionY -20 mm" << endl;
		ofs << "#" << endl;
		ofs << "# Rootfile setup" << endl;
		ofs << "#" << endl;
		ofs << Form("/analysis/setFileName run1_3_4_%d", i) << endl;
		ofs << "/analysis/h1/set 1 100 1 30 MeV" << endl;
		ofs << "/analysis/h2/setX 1 300 -15 15 cm  #x-y" << endl;
		ofs << "/analysis/h2/setY 1 300 -15 15 cm  #x-y" << endl;
		ofs << "/analysis/h2/setX 4 120 0 360 deg" << endl;
		ofs << "/analysis/h2/setY 4 101 -1.005 1.005 none" << endl;
		ofs << "#" << endl;
		ofs << "/run/printProgress 100" << endl;
		ofs << "#" << endl;
		ofs << "/run/beamOn 30000" << endl;
		ofs << "################################################" << endl;
		ofs << "#" << endl;
		ofs << "# cell coordinate: 3,5" << endl;
		ofs << "#" << endl;
		ofs << "# Particle position distribution in polar angle" << endl;
		ofs << "/tutorial/generator/PositionX -40 mm" << endl;
		ofs << "/tutorial/generator/PositionY 0 mm" << endl;
		ofs << "#" << endl;
		ofs << "# Rootfile setup" << endl;
		ofs << "#" << endl;
		ofs << Form("/analysis/setFileName run1_3_5_%d", i) << endl;
		ofs << "/analysis/h1/set 1 100 1 30 MeV" << endl;
		ofs << "/analysis/h2/setX 1 300 -15 15 cm  #x-y" << endl;
		ofs << "/analysis/h2/setY 1 300 -15 15 cm  #x-y" << endl;
		ofs << "/analysis/h2/setX 4 120 0 360 deg" << endl;
		ofs << "/analysis/h2/setY 4 101 -1.005 1.005 none" << endl;
		ofs << "#" << endl;
		ofs << "/run/printProgress 100" << endl;
		ofs << "#" << endl;
		ofs << "/run/beamOn 30000" << endl;
	}
	ofs.close();
}
