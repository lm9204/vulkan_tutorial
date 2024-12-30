#pragma once

#include "Skeleton.h"

class Timestep;
class SkeletalAnimation
{
public:
	enum class EPath
	{
		TRANSLATION,
		ROTATION,
		SCALE
	};

	enum class EInterpolationMethod
	{
		LINEAR,
		STEP,
		CUBICSPLINE
	};
	
	struct Channel
	{
		EPath m_Path;
		int m_SamplerIndex;
		int m_Node;
	};

	struct Sampler
	{
		std::vector<float> m_Timestamps;
		std::vector<glm::vec4> m_TobeInterpolatedValue;
		EInterpolationMethod m_Interpolation;
	};

public:
	SkeletalAnimation(std::string const& name);

	void start();
	void stop();
	bool isRunning() const;
	void update();

	std::vector<SkeletalAnimation::Sampler> m_Samplers;
	std::vector<SkeletalAnimation::Channel> m_Channels;

	//TODO
		// setFirst/LastKeyFrame()

private:
	std::string m_Name;
	bool m_Repeat;

	float m_FirstKeyFrameTime;
	float m_LastKeyFrameTime;
	float m_CurrentKeyFrameTime = 0.0f;
};