#pragma once

#include <vector>
#include <map>
#include <glm/glm.hpp>
class DGI {

public:
	DGI();
	virtual ~DGI();
	virtual void calculateCoefficients(); //Llm
	double get_coefficient(int l, int m, int Color); //we reference a coeffiecient based on spherical harmonic level (l) and index (m) 
	void add_coefficient(int l, int m);
	void getrgbvalues(unsigned char* data, int w, int h);
private:
	

	std::vector<std::vector<int>> Red; //these are passed in RBG values.
	std::vector<std::vector<int>> Green;
	std::vector<std::vector<int>> Blue;
	std::vector<std::vector<glm::vec3>> texPos; //might need this to find direction.

	std::vector<std::vector<int>>* colors[3]; //passed in color values


	std::map<int, std::map<double, int> >* Llm[3]; //set of Llm RGB values


	std::map<int, std::map<double,int> > L_R;
	std::map<int, std::map<double, int> > L_G;
	std::map<int, std::map<double, int> > L_B;
};