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
	int i, j;
	//glm::vec3 corner; char c1; char c2;
	//getside(side, corner, c1, c2);



	for (j = 0; j < h; j++) {
		Red.push_back(vector<int>());
		Green.push_back(vector<int>());
		Blue.push_back(vector<int>());
		for (i = 0; i < w * 3; i = i + 3) {
			Red[j].push_back(data[i + w * j]);
			Green[j].push_back(data[i + 1 + w * j]);
			Blue[j].push_back(data[i + 2 + w * j]);

		}
	} //texPos

	cout << "got colors - this is unoptimized" << endl;
}


void DGI::setupsides(int w, int h) {

	int totalsize = w * h * 6;
	TPos = new glm::vec3[totalsize];

	float interval = (skyboxsize * 2) / (float)w;

	glm::vec3 point;
	int i, j, k; k = 0;

	//right
	point = glm::vec3(1.0f * skyboxsize, 1.0f * skyboxsize, -1.0f * skyboxsize);
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i ++) {
			TPos[k] = point;
			point.z += interval;
			k++;
		}
		point.y -= interval;
		point.z = -1.0f * skyboxsize;
	}
	//left
	point = glm::vec3(-1.0f * skyboxsize, 1.0f * skyboxsize, -1.0f * skyboxsize);
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			TPos[k] = point;
			point.z += interval;
			k++;
		}
		point.y -= interval;
		point.z = -1.0f * skyboxsize;
	}
	//top
	point = glm::vec3(-1.0f * skyboxsize, 1.0f * skyboxsize, -1.0f * skyboxsize);
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			TPos[k] = point;
			point.x += interval;
			k++;
		}
		point.z += interval;
		point.x = -1.0f * skyboxsize;
	}
	//bottom
	point = glm::vec3(-1.0f * skyboxsize, -1.0f * skyboxsize, 1.0f * skyboxsize);
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			TPos[k] = point;
			point.x += interval;
			k++;
		}
		point.z -= interval;
		point.x = -1.0f * skyboxsize;
	}
	//front
	point = glm::vec3(-1.0f * skyboxsize, 1.0f * skyboxsize, 1.0f * skyboxsize);
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			TPos[k] = point;
			point.x += interval;
			k++;
		}
		point.y -= interval;
		point.x = -1.0f * skyboxsize;
	}
	//back
	point = glm::vec3(-1.0f * skyboxsize, 1.0f * skyboxsize, -1.0f * skyboxsize);
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			TPos[k] = point;
			point.x += interval;
			k++;
		}
		point.y -= interval;
		point.x = -1.0f * skyboxsize;
	}

	//for (i = 0; i < totalsize; i++) {
	//	cout << TPos[i].x << ", " << TPos[i].y << ", " << TPos[i].z << endl;
	//}
	cout << "got sides" << endl;
}
void DGI::calculateCoefficients() {


	int i, j, k;
	double x, y, z;
	int u = 0;
	int s = Red.size() / 6;
	double theta = 0.0;
	double formfactor = 0.0;
	cout << "here" << endl;
	for ( j = 0; j < Red.size(); j++) { 
		for ( i = 0; i < Red[j].size(); i++) { //for each pixel (or from 0 to 2PI)
			//get theta and phi
			
			//
			for ( k = 0; k < 3; k++) { //for each color
				float c = (float)colors[k]->at(j).at(i); //convert color from RGBA to somethhing else? 0 to 1;
				c = c / 255.0f;
				x = TPos[u].x;	y = TPos[u].y; 	z = TPos[u].z;
				
				if (u > s * 2 && u < s * 4) { //if this is the top and bottom textures, do this:
					formfactor = 1.0 / (3.141592 * pow(pow(x, 2) + pow(y, 2) + 1, 2));
					theta = acos(1.0 / sqrt(pow(x, 2) + pow(y, 2) + 1) );
				}
				else {
					formfactor = z / (3.141592 * pow(pow(y, 2) + pow(z, 2) + 1, 2));
					theta = acos(z / sqrt(pow(y, 2) + pow(z, 2) + 1));
				}

				Llm[k]->at(0).at(0) += c * ( 0.282095) * sin(theta) * formfactor;
				//cout << c * ( 0.282095) * sin(theta) * formfactor << endl;
				Llm[k]->at(1).at(1) += c * (0.488603 * x) * sin(theta) * formfactor;
				Llm[k]->at(1).at(0) += c * (0.488603 * y) * sin(theta) * formfactor;
				Llm[k]->at(1).at(-1) += c * (0.488603 * z) * sin(theta) * formfactor;
				Llm[k]->at(2).at(1) += c * (1.092548 * x * z) * sin(theta) * formfactor;
				Llm[k]->at(2).at(-1) += c * (1.092548 * y * z) * sin(theta) * formfactor;
				Llm[k]->at(2).at(-2) += c * (1.092548 * x * y) * sin(theta) * formfactor;
				Llm[k]->at(2).at(0) += c * (0.315392 * (3 * pow(z, 2) - 1)) * sin(theta) * formfactor;
				Llm[k]->at(2).at(2) += c * (0.546274 * (pow(x, 2) * pow(y, 2))) * sin(theta) * formfactor;
			}
			u++;
		}
	}
	cout << "there" << endl;
}

void DGI::add_coefficient(int l, int m) {

	map<int, map<int, double>>::const_iterator M = L_R.find(l);
	if (M == L_R.end()) {
		cout << "adding level" << endl;
		map<int, double> NewLevel;
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

/*
void DGI::getside(int S, glm::vec3& start_corner, char& dimension_one, char& dimension_two) {
	switch (S) {
	case right:
		start_corner = glm::vec3(3.0f, 3.0f, -3.0f);
		dimension_one = 'z';
		dimension_two = 'y';
		break;
	case left:

		break;
	case top:

		break;
	case bottom:

		break;
	case front:

		break;
	case back:

		break;
	default:
		cout << "the end" << endl;
	}

}*/
