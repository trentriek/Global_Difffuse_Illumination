#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
class DGI {

public:
	DGI();
	virtual ~DGI();
	virtual void calculateCoefficients(double pixelsamount); //Llm
	double get_coefficient(int l, int m, int Color); //we reference a coeffiecient based on spherical harmonic level (l) and index (m) 
	void add_coefficient(int l, int m);
	void getrgbvalues(unsigned char* data, int w, int h);
	void setupsides(int w, int h);

	void sample_coefficients_1();

private:
	

	std::vector<int> Red; //these are passed in RBG values.
	std::vector<int> Green;
	std::vector<int> Blue;
	//std::vector<std::vector<glm::vec3>> texPos;



	glm::vec3* TPos;

	std::vector<int>* colors[3]; //passed in color values


	std::map<int, std::map<int, double> >* Llm[3]; //set of Llm RGB values


	std::map<int, std::map<int, double> > L_R;
	std::map<int, std::map<int, double> > L_G;
	std::map<int, std::map<int, double> > L_B;

	
};