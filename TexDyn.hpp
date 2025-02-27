/*
Author: Daniel Rehberg
Date Modified: Janurary 26, 2025
*/

#ifndef __DYNAMIC_TEXTURE_HPP__
#define _DYNAMIC_TEXTURE_HPP__

#include <GL/glew.h>
#include <SDL_opengl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <atomic>
#include <random>
#include <mutex>
#include "Stamp.hpp"

constexpr size_t DIM = 64;

class DynamicTexture
{
public:
	DynamicTexture();// size_t width, size_t height);
	~DynamicTexture();
	const glm::mat4& getModel() const;
	void updateTexture(const glm::vec3& axis, const float angle);
	void uploadTexture(GLuint texID);
    // width, height, centerX, and centerY are normalized; see Stamp.hpp for more
    void applyStamp(const Stamp& stamp, double width, double height, double centerX, double centerY, double maxStrength);
private:
	size_t w = DIM, h = DIM;
	bool vertical = true;
	//Texels to process, including their velocities
	//	The state change stores the loss and gain of particles at the ith texel
	//	Current process is every 4 bytes is RGBA
	//		R: velocity
	//		G: Particles added to current texel
	//		B: Particles removed from current texel
	//		A: Total number of particles a current texel
	std::uint8_t* texels; //Avoiding volatile for now :), but might become necessary
	glm::mat4 model;

};

void kernelHorizontal(std::mutex& m, size_t begin, size_t end, void* data);
void kernelVertical(std::mutex& m, size_t begin, size_t end, void* data);

#endif
