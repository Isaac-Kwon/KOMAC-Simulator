// GUI input
// 0: No event display
// 1: OGS
// 2: HEPREP
// 3: DAWN (Not yet added in this code)
void gen_macro_aufoil(int gui =0, int runAcc = 0){ 

	std::ofstream ofs ("run_generated.mac", std::ofstream::out);

	float momentum = 17; //Unit: MeV

	float event_map1[10][10] = {
		{0,			0,		0.0754,	0.3004,	0.3948,	0.3727,	0.2469,	0.0515,	0,		0		},
		{0,			0.2031,	0.6081,	0.6270,	0.5987,	0.5681,	0.5389,	0.4789,	0.1409,	0		},
		{0.1007,	0.7134,	0.7588,	0.7204,	0.6892,	0.6504,	0.6122,	0.5779,	0.4981,	0.0753	},
		{0.4259,	0.8945,	0.8559,	0.8123,	0.7710,	0.7298,	0.6865,	0.6397,	0.5914,	0.2843	},
		{0.6502,	0.9703,	0.9248,	0.8765,	0.8263,	0.7843,	0.7404,	0.6826,	0.6335,	0.4253	},
		{0.6571,	1,		0.9596,	0.9156,	0.8678,	0.8188,	0.7716,	0.7200,	0.6660,	0.4363	},
		{0.4555,	0.9892,	0.9487,	0.9150,	0.8766,	0.8302,	0.7792,	0.7262,	0.6764,	0.2991	},
		{0.0952,	0.8004,	0.8918,	0.8700,	0.8399,	0.8028,	0.7552,	0.7017,	0.5680,	0.0590	},
		{0,			0.2044,	0.7290,	0.7965,	0.7744,	0.7468,	0.7053,	0.5856,	0.1382,	0		},
		{0,			0,		0.0719,	0.3413,	0.4840,	0.4628,	0.2858,	0.0510,	0,		0		}
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
	ofs << Form("/tutorial/generator/KineticEnergy %f MeV", momentum) << endl;

	int maxBin = 10;
	float start_xBin_position = -25.22725;
	float start_yBin_position = -25.22725;

	for (int i = 0; i < 1; ++i)
	{
		for (int iX = 0; iX < maxBin; ++iX)
		{
			for (int iY = 0; iY < maxBin; ++iY){
				if(event_map1[9-iY][iX]==0) continue;
				ofs << "################################################" << endl;
				ofs << "#" << endl;
				ofs << Form("# cell coordinate: %d,%d", iX+1, iY+1) << endl;
				ofs << "#" << endl;
				ofs << "# Particle position distribution in polar angle" << endl;
				ofs << Form("/tutorial/generator/PositionX %f mm", start_xBin_position + 5.04545*iX) << endl;
				ofs << Form("/tutorial/generator/PositionY %f mm", start_yBin_position + 5.04545*iY) << endl;
				ofs << "#" << endl;
				ofs << "# Rootfile setup" << endl;
				ofs << "#" << endl;
				ofs << Form("/analysis/setFileName run%d_%d_%d", i+1, iX, iY) << endl;
				ofs << "/analysis/h1/set 1 100 1 30 MeV" << endl;
				ofs << "/analysis/h2/setX 1 300 -15 15 cm  #x-y" << endl;
				ofs << "/analysis/h2/setY 1 300 -15 15 cm  #x-y" << endl;
				ofs << "/analysis/h2/setX 4 120 0 360 deg" << endl;
				ofs << "/analysis/h2/setY 4 101 -1.005 1.005 none" << endl;
				ofs << "#" << endl;
				ofs << "/run/printProgress 100" << endl;
				ofs << "#" << endl;
				ofs << Form("/run/beamOn %.0f", 1000*event_map1[9-iY][iX]) << endl;  		
			}
		}
	}
	ofs.close();
}
