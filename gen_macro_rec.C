// GUI input
// 0: No event display
// 1: OGS
// 2: HEPREP
// 3: DAWN (Not yet added in this code)
void gen_macro_rec(int gui =0, int runAcc = 0){ 

	std::ofstream ofs ("run_generated.mac", std::ofstream::out);

	float momentum = 194; //Unit: MeV

	int event_map1[10][10] = {  	
		{2,	2,	2,	2,	2,	2,	2,	1,	1,	1},
		{3,	3,	3,	3,	3,	3,	3,	2,	2,	1},
		{2,	5,	5,	5,	5,	5,	5,	2,	2,	1},
		{3,	5,	5,	20,	20,	5,	5,	2,	2,	1},
		{3,	5,	10,	20,	20,	10,	5,	2,	2,	1},
		{3,	5,	10,	30,	20,	15,	10,	2,	2,	1},
		{3,	5,	5,	20,	20,	5,	5,	2,	2,	1},
		{3,	5,	5,	5,	5,	5,	5,	2,	2,	1},
		{3,	3,	3,	3,	3,	3,	3,	2,	2,	1},
		{3,	2,	2,	2,	2,	2,	2,	1,	1,	1}
	};	

	ofs << "# KOMAC Simulator" << endl;
	ofs << "#" << endl;
	if (gui==1){
		ofs << "# Add trajectories to the visualization." << endl;
		ofs << "/vis/scene/add/trajectories" << endl;
		ofs << "# Accumulate multiple events in one picture." << endl;
		ofs << "/vis/scene/endOfEventAction accumulate -1" << endl;
		ofs << "# Multithread mode" << endl;
		ofs << "/vis/multithreading/maxEventQueueSize -1" << endl;
		ofs << "/vis/ogl/set/displayListLimit 9999999" << endl;
		ofs << "#Multithreading mode for visulaization" << endl;
		ofs << "/vis/multithreading/maxEventQueueSize -1" << endl;
		ofs << "/vis/multithreading/actionOnEventQueueFull wait" << endl;
	}

	if (gui==2){
		ofs << "# HepRepFile diriver" << endl;
		ofs << "/vis/open HepRepFile" << endl;
		ofs << "# Add world volume" << endl;
		ofs << "/vis/drawVolume" << endl;
		ofs << "# Add trajectories to the visualization." << endl;
		ofs << "/vis/scene/add/trajectories" << endl;
		// ofs << "# Accumulate multiple events in one picture." << endl;
		// ofs << "/vis/scene/endOfEventAction accumulate -1" << endl;
		ofs << "/vis/drawOnlyToBeKeptEvents" << endl;
		ofs << "#Multithreading mode for visulaization" << endl;
		ofs << "/vis/multithreading/maxEventQueueSize -1" << endl;
		ofs << "/vis/multithreading/actionOnEventQueueFull wait" << endl;
	}

	if (runAcc>0){
		ofs << "# Accumulate multiple runs in one picture." << endl;
		ofs << "/vis/scene/endOfRunAction accumulate" << endl;  	
	}

	ofs << "# Initialize kernel" << endl;
	ofs << "/run/initialize" << endl;
	ofs << "# Particle Energy" << endl;
	ofs << Form("/tutorial/generator/momentum %f MeV", momentum) << endl;

	int maxBin = 10;
	float start_xBin_position = -100;
	float start_yBin_position = -100;

	for (int iX = 0; iX < maxBin; ++iX)
	{
		for (int iY = 0; iY < maxBin; ++iY){
			ofs << "################################################" << endl;
			ofs << "#" << endl;
			ofs << Form("# cell coordinate: %d,%d", iX+1, iY+1) << endl;
			ofs << "#" << endl;
			ofs << "# Particle position distribution in polar angle" << endl;
			ofs << Form("/tutorial/generator/PositionX %f mm", start_xBin_position + 20*iX) << endl;
			ofs << Form("/tutorial/generator/PositionY %f mm", start_yBin_position + 20*iY) << endl;
			ofs << "#" << endl;
			ofs << "# Rootfile setup" << endl;
			ofs << "#" << endl;
			ofs << Form("/analysis/setFileName run1_%d_%d", iX, iY) << endl;
			ofs << "/analysis/h1/set 1 100 1 30 MeV" << endl;
			ofs << "/analysis/h2/setX 1 300 -15 15 cm  #x-y" << endl;
			ofs << "/analysis/h2/setY 1 300 -15 15 cm  #x-y" << endl;
			ofs << "/analysis/h2/setX 4 120 0 360 deg" << endl;
			ofs << "/analysis/h2/setY 4 101 -1.005 1.005 none" << endl;
			ofs << "#" << endl;
			ofs << "/run/printProgress 100" << endl;
			ofs << "#" << endl;
			ofs << Form("/run/beamOn %d", 3*3*600*event_map1[9-iY][iX]) << endl;  		
		}
	}
	ofs.close();
}
