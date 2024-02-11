// A fast, transient K+ PM conductance using the Hodgkin-Huxley formalism.
// For speed, the parameters alphaM, betaM, alphaH, and betaH are pre-calculated
// and put in lookup tables stored as global variables.

// Declare the lookup table variables
float alphaM2[1501] = {0.0};                          // Pre-calculate activation and inactivation parameters 
float betaM2[1501] = {0.0};                           // for sodium currents: Vm from -100 mV to +50 mV in 
float alphaH2[1501] = {0.0};                          // steps of 0.1 mV
float betaH2[1501] = {0.0};

// Generate the lookup tables
void GeneratePotassiumPMLUT() {
  float v;
  for (int x=0; x<1501; x++) {
    v = (float)x/10 - 100.0;                        // We use membrane potentials between -100 mV and +50 mV
    if (x==930) {                                   // The if-else statement makes sure alphaM stays finite at v = -7 mV.
      alphaM2[x] = 0.1;
    } else {
      alphaM2[x] = 0.005 * (7 + v) / (1-expf(-(7 + v)/70));
    }
    betaM2[x] = 0.004 * expf(-(v-110)/28);
    alphaH2[x] = 0.00058 * expf(-(v-100)/500);
    betaH2[x] = 0.004 / (1+ expf(-(v-25)/12)); 
  }
}

// At every time step, calculate the potassium current in the Hodgkin-Huxley manner
float PotassiumPM(float v) {
  static float mKpmVar = 0.0;    // activation gate
  static float hKpmVar = 1.0;    // inactivation gate
  float v10 = v*10.0;
  int vIdx = (int)v10 + 1000;
  vIdx = constrain(vIdx,0,1500);
  mKpmVar = mKpmVar + dt * ( alphaM2[vIdx]*(1-mKpmVar) - betaM2[vIdx]*mKpmVar );
  if (mKpmVar < 0.0) mKpmVar = 0.0;
  hKpmVar = hKpmVar + dt * ( alphaH2[vIdx]*(1-hKpmVar) - betaH2[vIdx]*hKpmVar );
  if (hKpmVar < 0.0) hKpmVar = 0.0;
  float current2 = - gK_PM * mKpmVar * mKpmVar * mKpmVar * mKpmVar * hKpmVar * (v + 80);  // EK = -90 mV
  return current2;
}

