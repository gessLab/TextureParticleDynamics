/*
Author: Daniel Rehberg
Date Modified: Janurary 26, 2025
*/

#include <iostream>
#include "TexDyn.hpp"
#include "ThreadPool.hpp"

DynamicTexture::DynamicTexture() : model(1.0f), texels(nullptr), w(DIM), h(DIM)
{
	//model[3][2] = -200.0f;
	//model = glm::rotate(model, (90.0f),
	//	glm::vec3(1.0f, 0.0f, 0.0f));
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> r(0, 255);
	texels = new std::uint8_t[DIM * DIM * 4];
	for (size_t i = 0; i < w * h * 4; i+=4)
	{
		texels[i] = 0;// std::uint8_t(r(generator));
		texels[i + 1] = 0;// std::uint8_t(r(generator));
		texels[i + 2] = 0;// std::uint8_t(r(generator));
		texels[i + 3] = (i % 80);// std::uint8_t(r(generator));
	}
}

DynamicTexture::~DynamicTexture()
{
	if (texels != nullptr) delete[] texels;
}

const glm::mat4& DynamicTexture::getModel() const
{
	return model;
}

void test(std::mutex& m, size_t begin, size_t end, void* data)
{
	std::uint8_t* ref = static_cast<std::uint8_t*>(data);
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> r(0, 2000000000);
	size_t index = begin * 4;
	for (size_t i = begin; i < end; ++i)
	{
		std::uint32_t val = r(generator);

		ref[index] -= static_cast<std::uint8_t>((0xFF000000 & val) >> 24);
		ref[index+1] -= static_cast<std::uint8_t>((0x00FF0000 & val) >> 16); 
		ref[index+2] -= static_cast<std::uint8_t>((0x0000FF00 & val) >> 8);
		ref[index+3] -= static_cast<std::uint8_t>(0x000000FF & val);
		index += 4;
	}
}

void integrate(std::mutex& m, size_t begin, size_t end, void* data)
{
	//Two things to integrate
	//	Alpha particle change
	//	Velocity
	std::uint8_t* ref = static_cast<std::uint8_t*>(data);
	
	std::uint8_t aoi = ref[0];
	
	//two goals to achieve -- add together previous kernel particle quantity changes
	size_t index = begin * 4;
	for (size_t i = begin; i < end; ++i, index += 4)
	{
		//Integrate new texel particle stack size
		//Blue channel is subtraction, green is addition
		//Alpha stores this change
		ref[index + 3] -= ref[index + 2];
		ref[index + 3] += ref[index + 1];
		ref[index + 2] = 0;
		ref[index + 1] = 0;
		//Second integration is acceleration due to gravity
		//	Stored in red channel
		ref[index] = aoi;
	}
}

void kernelHorizontal(std::mutex& m, size_t begin, size_t end, void* data)
{
	size_t row = begin / DIM, col = (begin % DIM) - 1;//NOTE: preemptively subtracting 1 as we start the for-loop by incrementing
	std::uint8_t* ref = static_cast<std::uint8_t*>(data);
	//if row 0, then cannot access anything above
	//if column 0, then cannot access anything on the left
	//if column == MAX - 1, then cann access right
	//if row > MAX - 1, then cannot access down
	//Stride of 4, so storing separate index value
	size_t index = begin * 4;
	for (size_t i = begin; i < end; ++i, index+=4)
	{
		if (++col == DIM)
		{
			col = 0;
			row += 1;
		}
		//check left for capture, else check right
		//	NOTE: Current procedure has all particles falling in unison, but this will not be the future case
		//		Therefore, we will ustilize the general process here now
		//	NOTE: Could add some variety by something like row % 2 to determine if checking left before right
		//		For now, just always checking left before right
		//Particle movement is only allowed if stacks have different heights
		//	AND, this texel's particle stack cannot exceed 255

		if (ref[index + 3] == 255)continue;//no room to accept new particles

		//Check left, particle stack must be greater than , but corner case(s) as well
		if (col != 0)
		{
			if (ref[index + 3] <= ref[index - 1] && ref[index - 1] > 0)
			{
				//Check left pixels velocity, stored in red byte
				//	0: not moving; 1: moving left; 2: moving right
				if (ref[index - 4] == 2)
				{
					//Acquire this particle rightward moving particle
					// this texel can modify self's g index
					// this texel can modify that's b index
					ref[index - 2] = 1;//that blue, will be -1 particle at left texel
					ref[index + 1] = 1;//this green, will be +1 particle at this texel
					continue;
				}
			}
		}
		//NOTE: in the future, could consider accepting two particles from left and right
		//	in one step, if gaining a left particle first still has the right particle being higher than this texel's stack
		if (col != (DIM - 1))
		{
			if (ref[index + 3] <= ref[index + 7] && ref[index + 7] > 0)
			{
				//same process as above, but checking right pixel
				if (ref[index + 4] == 1)
				{
					//Acquire particle moving leftward
					ref[index + 6] = 1;
					ref[index + 1] = 1;
				}
			}
		}
	}
}

void kernelVertical(std::mutex& m, size_t begin, size_t end, void* data)
{
	size_t row = begin / DIM, col = (begin % DIM) - 1;//NOTE: preemptively subtracting 1 as we start the for-loop by incrementing
	std::uint8_t* ref = static_cast<std::uint8_t*>(data);
	//Vertical border
	//if row 0, then cannot access anything in negative direction
	//if row > MAX - 1, then cannot access positve direction
	//Stride of 4, so storing separate index value
	size_t index = begin * 4;
	size_t rowOffset = DIM * 4;
	for (size_t i = begin; i < end; ++i, index += 4)
	{
		if (++col == DIM)
		{
			col = 0;
			row += 1;
		}

		if (ref[index + 3] == 255)continue;//no room to accept new particles

		//Check left, particle stack must be greater than , but corner case as well
		if (row != 0)
		{
			if (ref[index + 3] <= ref[index - rowOffset + 3] && ref[index - rowOffset + 3] > 0)
			{
				//Check left pixels velocity, stored in red byte
				//	0: not moving; 1: moving down; 2: moving up
				if (ref[index - rowOffset] == 2)
				{
					//Acquire this particle rightward moving particle
					// this texel can modify self's g index
					// this texel can modify that's b index
					ref[index - rowOffset + 2] = 1;//that blue, will be -1 particle at left texel
					ref[index + 1] = 1;//this green, will be +1 particle at this texel
					continue;
				}
			}
		}
		//NOTE: in the future, could consider accepting two particles from left and right
		//	in one step, if gaining a left particle first still has the right particle being higher than this texel's stack
		if (row != (DIM - 1))
		{
			if (ref[index + 3] <= ref[index + rowOffset + 3] && ref[index + rowOffset + 3] > 0)
			{
				//same process as above, but checking row below texel
				if (ref[index + rowOffset] == 1)
				{
					//Acquire particle moving leftward
					ref[index + rowOffset + 2] = 1;
					ref[index + 1] = 1;
				}
			}
		}
	}
}

void DynamicTexture::updateTexture(const glm::vec3& axis, const float angle)
{
	if (glm::dot(axis, axis) > 0.5f) model = glm::rotate(model, angle, axis);
	glm::vec4 grav = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
	
	
	//Create a templated dispatch function for the thread pool class as well to pass owned data
	if (!vertical)
	{
		glm::vec4 tangent = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);//Test for gravity in right tangent of plane
		tangent = model * tangent;
		float aoi = glm::dot(tangent, grav);
		std::uint8_t aoInt = static_cast<std::uint8_t>((aoi * 100.0f + 100.0f));
		//[[TO DO]] Friction and gradient analysis will need to go here..
		//	Temp, just apply a velocity if gravity aoi is between [0, 40] (left) and [160, 200]
		if (aoInt < 41) texels[0] = 1;
		else if (aoInt > 159) texels[0] = 2;
		else texels[0] = 0;
		//ThreadsPool::pool().dispatch(1024 * 1024, &(test), static_cast<void*>(texels), true);
		ThreadsPool::pool().dispatch(w * h, &(integrate), static_cast<void*>(texels), true);
		ThreadsPool::pool().dispatch(w * h, &(kernelHorizontal), static_cast<void*>(texels), true);
	}
	else
	{
		glm::vec4 tangent = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);//Test for gravity in vertical (forward) tangent of plane
		tangent = model * tangent;
		float aoi = -glm::dot(tangent, grav);
		std::uint8_t aoInt = static_cast<std::uint8_t>((aoi * 100.0f + 100.0f));
		//[[TO DO]] Friction and gradient analysis will need to go here..
		//	Temp, just apply a velocity if gravity aoi is between [0, 40] (left) and [160, 200]
		if (aoInt < 41) texels[0] = 1;
		else if (aoInt > 159) texels[0] = 2;
		else texels[0] = 0;
		//ThreadsPool::pool().dispatch(1024 * 1024, &(test), static_cast<void*>(texels), true);
		ThreadsPool::pool().dispatch(w * h, &(integrate), static_cast<void*>(texels), true);
		ThreadsPool::pool().dispatch(w * h, &(kernelVertical), static_cast<void*>(texels), true);
	}
	vertical = !vertical;
}

void DynamicTexture::uploadTexture(GLuint texID)
{
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI, w, h,
		0, GL_RGBA_INTEGER, GL_UNSIGNED_INT_8_8_8_8_REV,
		texels);

	glBindTexture(GL_TEXTURE_2D, 0);
}