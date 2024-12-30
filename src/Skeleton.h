#pragma once

#include <iostream>
#include <map>

#include "glm/gtc/quaternion.hpp"
#include "glm/gtx/quaternion.hpp"

#include "Bones.h"

namespace Armature
{
	static constexpr int NO_PARENT = -1;
	static constexpr int ROOT_JOINT = 0;

	struct ShaderData
	{
		std::vector<glm::mat4> m_FinalBonesMatrices;
	};

	// TODO : 최적화 ->  동적/정적 데이터 분리
		// struct BoneTransform
		// {
		// 	// deformed data
		// 	glm::vec3 m_DeformedNodeTranslation{0.0f};
		// 	glm::quat m_DeformedNodeRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		// 	glm::vec3 m_DeformedNodeScale{1.0f};

		// 	glm::mat4 getDeformedBindMatrix()
		// 	{
		// 		return	glm::translate(glm::mat4(1.0f), m_DeformedNodeTranslation) * // T
		// 				glm::mat4(m_DeformedNodeRotation) *                          // R
		// 				glm::scale(glm::mat4(1.0f), m_DeformedNodeScale);            // S
		// 	}
		// };

	struct Bone
	{
		// TODO 동적/정적 데이터 분리사항
			// glm::mat4 m_BindPose{0.0f};
			// glm::mat4 m_InverseBindPose{0.0f};

		std::string m_Name;
		// deformed data
		glm::vec3 m_DeformedNodeTranslation{0.0f};
		glm::quat m_DeformedNodeRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
		glm::vec3 m_DeformedNodeScale{1.0f};

		glm::mat4 getDeformedBindMatrix()
		{
			return	glm::translate(glm::mat4(1.0f), m_DeformedNodeTranslation) *	// T
					glm::mat4(m_DeformedNodeRotation) *								// R
					glm::scale(glm::mat4(1.0f), m_DeformedNodeScale);				// S
		}

		int m_ParentBone;
		std::vector<int> m_Children;
	};

	struct Skeleton
	{
		void update();
		void updateBone();

		bool m_isAnimated = true;
		std::string m_Name;
		std::vector<Bone> m_Bones;
		std::map<int, int> m_GlobalNodeToBoneIndex;
		ShaderData m_ShaderData;
	};
};