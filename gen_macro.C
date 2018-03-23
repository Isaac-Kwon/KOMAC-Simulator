void gen_macro(){

  std::ofstream ofs ("run_generated.mac", std::ofstream::out);

  float momentum = 194; //Unit: MeV

  ofs << "# KOMAC Simulator" << endl;
  ofs << "#" << endl;
  ofs << "# Initialize kernel" << endl;
  ofs << "/run/initialize" << endl;
  ofs << "# Particle Energy" << endl;
  ofs << Form("/tutorial/generator/momentum %f MeV", momentum) << endl;

  int max_radius = 100;
  int min_event = 100;
  for (int i_radius = 0; i_radius < max_radius; ++i_radius)
  {
  	ofs << "################################################" << endl;
	ofs << "#" << endl;
	ofs << Form("# Radius: %d~%dmm", i_radius, i_radius+1) << endl;
	ofs << "#" << endl;
	ofs << "# Particle position distribution in polar angle" << endl;
	ofs << Form("/tutorial/generator/radius %d mm", i_radius);
	ofs << "#" << endl;
	ofs << "# Rootfile setup" << endl;
	ofs << "#" << endl;
	ofs << Form("/analysis/setFileName run_%2d_mm", i_radius) << endl;
	ofs << "/analysis/h1/set 1 100 1 30 MeV" << endl;
	ofs << "/analysis/h2/setX 1 300 -15 15 cm  #x-y" << endl;
	ofs << "/analysis/h2/setY 1 300 -15 15 cm  #x-y" << endl;
	ofs << "/analysis/h2/setX 4 120 0 360 deg" << endl;
	ofs << "/analysis/h2/setY 4 101 -1.005 1.005 none" << endl;
	ofs << "#" << endl;
	ofs << "/run/printProgress 100" << endl;
	ofs << "#" << endl;
	ofs << Form("/run/beamOn %d", min_event*(2*i_radius+1)) << endl;
  }
  ofs.close();

}