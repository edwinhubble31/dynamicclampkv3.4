// A fast, transient K+ PN conductance using the Hodgkin-Huxley formalism.
// For speed, the parameters alphaM, betaM, alphaH, and betaH are pre-calculated
// and put in lookup tables stored as global variables.

// Declare the lookup table variables
float alphaM[1501] = {0.0};                          // Pre-calculate activation and inactivation parameters 
float betaM[1501] = {0.0};                           // for sodium currents: Vm from -100 mV to +50 mV in 
float alphaH[1501] = {0.0};                          // steps of 0.1 mV
float betaH[1501] = {0.0};

// Generate the lookup tables
void GeneratePotassiumPNLUT() {
  float v;
  for (int x=0; x<1501; x++) {
    v = (float)x/10 - 100.0;                        // We use membrane potentials between -100 mV and +50 mV
    if (x==930) {                                   // The if-else statement makes sure alphaM stays finite at v = -7 mV.
      alphaM[x] = 0.1;
    } else {
      alphaM[x] = 0.005 * (7 + v) / (1-expf(-(7 + v)/70));
    }
    betaM[x] = 0.004 * expf(-(v-110)/28);
    alphaH[x] = 0.0000013 * expf(-(v+10)/65);
    betaH[x] = 0.041 / (1+ expf(-(v-0.8)/3.6)); 
  }
}

// At every time step, calculate the sodium current in the Hodgkin-Huxley manner
float PotassiumPN(float v) {
  static float mKpnVar = 0.0;    // activation gate
  static float hKpnVar = 1.0;    // inactivation gate
  float v10 = v*10.0;
  int vIdx = (int)v10 + 1000;
  vIdx = constrain(vIdx,0,1500);
  mKpnVar = mKpnVar + dt * ( alphaM[vIdx]*(1-mKpnVar) - betaM[vIdx]*mKpnVar );
  if (mKpnVar < 0.0) mKpnVar = 0.0;
  hKpnVar = hKpnVar + dt * ( alphaH[vIdx]*(1-hKpnVar) - betaH[vIdx]*hKpnVar );
  if (hKpnVar < 0.0) hKpnVar = 0.0;
  float current1 = - gK_PN * mKpnVar * mKpnVar * mKpnVar * mKpnVar * hKpnVar * (v + 80);  // EK = -68 corrected mV
  return current1;
}

