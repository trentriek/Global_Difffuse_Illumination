#include "DGI.h"
#include <iostream>
#include <fstream>
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
		for (i = 0; i < (w * 3); i = i + 3) {
			int g = i + w * j;
			int value = data[i + (w-1) * j];

			Red.push_back(data[i + w * 3 * j]);
			Green.push_back(data[i + 1 + w * 3 * j]);
			Blue.push_back(data[i + 2 + w * 3 * j]);
		}
	} //texPos

	cout << "got colors - this is unoptimized" << endl;
}


void DGI::setupsides(int w, int h) {

	//ofstream myfile;

	int totalsize = w * h * 6;
	TPos = new glm::vec3[totalsize];

	float interval = 2 / (float)w;

	glm::vec3 point;
	int i, j, k; k = 0;

	//myfile.open("points.txt");

	//right - interval/2
	//myfile << "Right" << endl;
	point = glm::vec3(1.0f, 1.0f, -1.0f);
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i ++) {
			TPos[k] = point;
			//myfile << point.x << ", " << point.y << ", " << point.z << endl;
			point.z += interval;
			k++;
		}
		point.y -= interval;
		point.z = -1.0f;
	}
	//left
	//myfile << "Left" << endl;
	point = glm::vec3(-1.0f, 1.0f, -1.0f);
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			TPos[k] = point;
			//myfile << point.x << ", " << point.y << ", " << point.z << endl;;
			point.z += interval;
			k++;
		}
		point.y -= interval;
		point.z = -1.0f;
	}
	//top
	//myfile << "Top" << endl;
	point = glm::vec3(-1.0f, 1.0f, -1.0f);
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			TPos[k] = point;
			//myfile << point.x << ", " << point.y << ", " << point.z << endl;
			point.x += interval;
			k++;
		}
		point.z += interval;
		point.x = -1.0f;
	}
	//bottom
	//myfile << "Bottom" << endl;
	point = glm::vec3(-1.0f, -1.0f, 1.0f);
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			TPos[k] = point;
			//myfile << point.x << ", " << point.y << ", " << point.z << endl;
			point.x += interval;
			k++;
		}
		point.z -= interval;
		point.x = -1.0f;
	}
	//front
	//myfile << "Front" << endl;
	point = glm::vec3(-1.0f, 1.0f, 1.0f);
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			TPos[k] = point;
			//myfile << point.x << ", " << point.y << ", " << point.z << endl;
			point.x += interval;
			k++;
		}
		point.y -= interval;
		point.x = -1.0f;
	}
	//back
	//myfile << "Back" << endl;
	point = glm::vec3(-1.0f, 1.0f, -1.0f);
	for (j = 0; j < h; j++) {
		for (i = 0; i < w; i++) {
			TPos[k] = point;
			//myfile << point.x << ", " << point.y << ", " << point.z << endl;
			point.x += interval;
			k++;
		}
		point.y -= interval;
		point.x = -1.0f;
	}
	//myfile.close();
	//for (i = 0; i < totalsize; i++) {
	//	cout << TPos[i].x << ", " << TPos[i].y << ", " << TPos[i].z << endl;
	//}
	cout << "got sides" << endl;
}
void DGI::calculateCoefficients(double pixelsamount) {


	int i, j, k;
	double x, y, z;

	int s = Red.size() / 6;
	double theta = 0.0;
	double formfactor = 0.0;

	double Llm00[3] = {0.0,0.0,0.0};
	double Llm11[3] = {0.0,0.0,0.0};
	double Llm10[3] = { 0.0,0.0,0.0 };
	double Llm1_1[3] = { 0.0,0.0,0.0 };
	double Llm21[3] = { 0.0,0.0,0.0 };
	double Llm2_1[3] = { 0.0,0.0,0.0 };
	double Llm2_2[3] = { 0.0,0.0,0.0 };
	double Llm20[3] = { 0.0,0.0,0.0 };
	double Llm22[3] = { 0.0,0.0,0.0 };

	double pixelarea = pow(2.0 / pixelsamount, 2);

	//remove these
	double sumofformfactors = 0.0;//i

	for ( i = 0; i < Red.size(); i++) { //for each pixel (or from 0 to 2PI)
			
		//if (i < 0 && i % s == 0) { 
		//	cout << "picture calculated" << endl; }

		for ( k = 0; k < 3; k++) { //for each color

			float c = (float)colors[k]->at(i); //convert color from RGBA
			c = c / 255.0f;
			x = TPos[i].x;	y = TPos[i].y; 	z = TPos[i].z;
			//cout << "color is " << k << ", value is: " << c  << ", position is: " << x << ", " << y << ", " << z << endl;

			if(i < s * 2) {
				formfactor = (abs(y) * pixelarea / (3.141592 * pow(pow(z, 2) + pow(y, 2) + 1, 2)));// *3.141592;
				sumofformfactors += formfactor;
				theta = acos(abs(y) / sqrt(pow(z, 2) + pow(y, 2) + 1));
			}
			else if (i >= s * 2 && i < s * 4) { //if this is the top and bottom textures, do this:
				formfactor = ((pixelarea / (3.141592 * pow(pow(x, 2) + pow(z, 2) + 1, 2)))); //*3.141592;
				sumofformfactors += formfactor;
				theta = acos(1 / sqrt(pow(x, 2) + pow(z, 2) + 1) );
			}
			else
			{
				formfactor = abs(y) * (pixelarea / (3.141592 * pow(pow(x, 2) + pow(y, 2) + 1, 2)));// *3.141592;
				sumofformfactors += formfactor;
				theta = acos(abs(y) / sqrt(pow(x, 2) + pow(y, 2) + 1) );
			}
			formfactor *= 3.141592;
			//formfactor = 3.8e-06;

			
			Llm00[k] += (c * ( 0.282095) * sin(theta) * formfactor);
			//cout << "k = " << k << ", coeffiecient is: " << c * ( 0.282095) * sin(theta) * formfactor << endl;
		
			Llm11[k] += (c * (0.488603 * x) * sin(theta) * formfactor);
			
			Llm10[k] += (c * (0.488603 * y) * sin(theta) * formfactor);
			
			Llm1_1[k] += (c * (0.488603 * z) * sin(theta) * formfactor);
		
			Llm21[k] += (c * (1.092548 * x * y) * sin(theta) * formfactor);
			
			Llm2_1[k] += (c * (1.092548 * z * y) * sin(theta) * formfactor);

			Llm2_2[k] += (c * (1.092548 * x * z) * sin(theta) * formfactor);
			Llm20[k] += (c * (0.315392 * (3 * pow(y, 2) - 1)) * sin(theta) * formfactor);
			Llm22[k] += (c * (0.546274 * (pow(x, 2) - pow(z, 2))) * sin(theta) * formfactor);
				
		}
	}

	cout << "coefficients calculated" << endl;
	cout << "sumofformfactors = "<< sumofformfactors << endl;
	Llm[0]->at(0).at(0) = Llm00[0];
	Llm[0]->at(1).at(1) = Llm11[0];
	Llm[0]->at(1).at(0) = Llm10[0];
	Llm[0]->at(1).at(-1) = Llm1_1[0];
	Llm[0]->at(2).at(1) = Llm21[0];
	Llm[0]->at(2).at(-1) = Llm2_1[0];
	Llm[0]->at(2).at(-2) = Llm2_2[0];
	Llm[0]->at(2).at(0) = Llm20[0];
	Llm[0]->at(2).at(2) = Llm22[0];

	Llm[1]->at(0).at(0) = Llm00[1];
	Llm[1]->at(1).at(1) = Llm11[1];
	Llm[1]->at(1).at(0) = Llm10[1];
	Llm[1]->at(1).at(-1) = Llm1_1[1];
	Llm[1]->at(2).at(1) = Llm21[1];
	Llm[1]->at(2).at(-1) = Llm2_1[1];
	Llm[1]->at(2).at(-2) = Llm2_2[1];
	Llm[1]->at(2).at(0) = Llm20[1];
	Llm[1]->at(2).at(2) = Llm22[1];

	Llm[2]->at(0).at(0) = Llm00[2];
	Llm[2]->at(1).at(1) = Llm11[2];
	Llm[2]->at(1).at(0) = Llm10[2];
	Llm[2]->at(1).at(-1) = Llm1_1[2];
	Llm[2]->at(2).at(1) = Llm21[2];
	Llm[2]->at(2).at(-1) = Llm2_1[2];
	Llm[2]->at(2).at(-2) = Llm2_2[2];
	Llm[2]->at(2).at(0) = Llm20[2];
	Llm[2]->at(2).at(2) = Llm22[2];
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

void DGI::sample_coefficients_1() {
	Llm[0]->at(0).at(0) = 0.0;
	Llm[0]->at(1).at(1) = 0.0;
	Llm[0]->at(1).at(0) = 0.0;
	Llm[0]->at(1).at(-1) = 0.0;
	Llm[0]->at(2).at(1) = 0.0;
	Llm[0]->at(2).at(-1) = 0.0;
	Llm[0]->at(2).at(-2) = 0.0;
	Llm[0]->at(2).at(0) = 0.0;
	Llm[0]->at(2).at(2) = 1.0;

	Llm[1]->at(0).at(0) = 0.0;
	Llm[1]->at(1).at(1) = 0.0;
	Llm[1]->at(1).at(0) = 0.0;
	Llm[1]->at(1).at(-1) = 0.0;
	Llm[1]->at(2).at(1) = 0.0;
	Llm[1]->at(2).at(-1) = 0.0;
	Llm[1]->at(2).at(-2) = 0.0;
	Llm[1]->at(2).at(0) = 0.0;
	Llm[1]->at(2).at(2) = -1.0;

	Llm[2]->at(0).at(0) = 0.0;
	Llm[2]->at(1).at(1) = 0.0;
	Llm[2]->at(1).at(0) = 0.0;
	Llm[2]->at(1).at(-1) = 0.0;
	Llm[2]->at(2).at(1) = 0.0 ;
	Llm[2]->at(2).at(-1) = 0.0;
	Llm[2]->at(2).at(-2) = 0.0;
	Llm[2]->at(2).at(0) = 0.0;
	Llm[2]->at(2).at(2) = 0.0;
}

/*
L00 .79 .44 .54 .38 .43 .45 .36 .26 .23
L1−1 .39 .35 .60 .29 .36 .41 .18 .14 .13
L10 - .34 - .18 - .27 .04 .03 .01 - .02 - .01 - .00
L11 - .29 - .06 .01 - .10 - .10 - .09 .03 .02 .01
L2−2 - .11 - .05 - .12 - .06 - .06 - .04 .02 .01 .00
L2−1 - .26 - .22 - .47 .01 - .01 - .05 - .05 - .03 - .01
L20 - .16 - .09 - .15 - .09 - .13 - .15 - .09 - .08 - .07
L21 .56 .21 .14 - .06 - .05 - .04 .01 .00 .00
L22 .21 - .05 - .30 .02 - .00 - .05 - .08 - .06 .00
*/


