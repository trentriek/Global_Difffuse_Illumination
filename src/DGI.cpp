#include "DGI.h"
#include <iostream>
using namespace std;

DGI::DGI() {
	colors[0] = &Red;
	colors[1] = &Green;
	colors[2] = &Blue;

	Llm[0] = &L_R;
	Llm[1] = &L_G;
	Llm[2] = &L_B;
}

DGI::~DGI() {

}
void DGI::getrgbvalues(unsigned char* data, int w, int h) {
	//remember if the size is 1024 then the actual width is times three, because three color: 3072
	vector<int> r;
	vector<int> g;
	vector<int> b;
	int i, j;
	for (j = 0; j < h; j++) {
		for (i = 0; i < w * 3; i = i + 3) {
			r.push_back(data[i + w * j]);
			g.push_back(data[i + 1 + w * j]);
			b.push_back(data[i + 2 + w * j]);
		}
	}
	for (j = 0; j < h; j++) {
		Red.push_back(vector<int>());
		Green.push_back(vector<int>());
		Blue.push_back(vector<int>());
		for (i = 0; i < w; i = i++) {
			Red[j].push_back(r.at(j * w + i));
			Green[j].push_back(g.at(j * w + i));
			Blue[j].push_back(b.at(j * w + i));
		}
	}
	cout << "got colors - this is unoptimized" << endl;
}

void DGI::calculateCoefficients() {


	for (int j = 0; j < Red.size(); j++) { 
		for (int i = 0; i < Red[j].size(); i++) { //for each pixel (or from 0 to 2PI)
			//get theta and phi
			double theta, phi;
			//
			for (int k = 0; k < 3; k++) { //for each color
				int c = colors[k]->at(j).at(i); //convert color from RGBA to somethhing else?
				double x, y, z;
				x = sin(theta) * cos(phi); y = sin(theta) * sin(phi); z = cos(theta);

				Llm[k]->at(0).at(0) += c * ( 0.282095) * sin(theta);
				Llm[k]->at(1).at(1) += c * (0.488603 * x) * sin(theta);
				Llm[k]->at(1).at(0) += c * (0.488603 * y) * sin(theta);
				Llm[k]->at(0).at(-1) += c * (0.488603 * z) * sin(theta);
				Llm[k]->at(2).at(1) += c * (1.092548 * x * z) * sin(theta);
				Llm[k]->at(2).at(-1) += c * (1.092548 * y * z) * sin(theta);
				Llm[k]->at(2).at(-2) += c * (1.092548 * x * y) * sin(theta);
				Llm[k]->at(2).at(0) += c * (0.315392 * (3 * pow(z, 2) - 1)) * sin(theta);
				Llm[k]->at(2).at(2) += c * (0.546274 * (pow(x, 2) * pow(y, 2))) * sin(theta);
			}
		}
	}
}

void DGI::add_coefficient(int l, int m) {

	map<int, map<double, int>>::const_iterator M = L_R.find(l);
	if (M == L_R.end()) {
		cout << "adding level" << endl;
		map<double, int> NewLevel;
		L_R[l] = NewLevel;
		L_R[l][m] = 0.0;
		L_G[l] = NewLevel;
		L_G[l][m] = 0.0;
		L_B[l] = NewLevel;
		L_B[l][m] = 0.0;
	}
	else {
		L_R[l][m] = 0.0;
		L_G[l][m] = 0.0;
		L_B[l][m] = 0.0;
	}
}

double DGI::get_coefficient(int l, int m, int color) {
	return Llm[color]->at(l).at(m);
}
