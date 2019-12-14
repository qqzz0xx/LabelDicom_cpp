#pragma once
#include "Header.h"

template<class T>void SubVec3(const T* v1, const T* v2, T *dst);

template<class T>void AddVec3(const T* v1, const T* v2, T *dst);

template<class T>void DotVec3(const T* v1, const T* v2, T *dst);

template<class T>void CrossVec3(const T* v1, const T* v2, T *dst);

template<class T>void MulVec3(const T* v1, const float factor, T *dst);

template<class T> T Vec3Distance(const T* v1, const T* v2);

template<class T> inline void SubVec3(const T* v1, const T* v2, T *dst)
{
	dst[0] = v1[0] - v2[0];
	dst[1] = v1[1] - v2[1];
	dst[2] = v1[2] - v2[2];
}

template<class T> inline void AddVec3(const T* v1, const T* v2, T *dst)
{
	dst[0] = v1[0] + v2[0];
	dst[1] = v1[1] + v2[1];
	dst[2] = v1[2] + v2[2];
}

template<class T> inline void DotVec3(const T* v1, const T* v2, T *dst)
{
	dst[0] = v1[0] * v2[0];
	dst[1] = v1[1] * v2[1];
	dst[2] = v1[2] * v2[2];
}

template<class T> inline void CrossVec3(const T* v1, const T* v2, T *dst)
{
	dst[0] = v1[1] * v2[2] - v1[2] * v2[1];
	dst[1] = v1[2] * v2[0] - v1[0] * v2[2];
	dst[2] = v1[0] * v2[1] - v1[1] * v2[0];w
}

template<class T> inline void MulVec3(const T* v1, const float factor, T *dst)
{
	dst[0] = v1[0] * factor;
	dst[1] = v1[1] * factor;
	dst[2] = v1[2] * factor;
}

template<class T>
inline T Vec3Distance(const T * v1, const T * v2)
{
    return std::sqrt((v1[0] - v2[0]) * (v1[0] - v2[0]) + (v1[1] - v2[1]) * (v1[1] - v2[1]) + (v1[2] - v2[2]) * (v1[2] - v2[2]));
}


inline void CompDeltaMat(vtkMatrix4x4* newMat, vtkMatrix4x4* oldMat, vtkMatrix4x4* deltaMat)
{
	auto oldMatInv = vtkSmartPointer<vtkMatrix4x4>::New();
	oldMatInv->DeepCopy(oldMat);
	oldMatInv->Invert();

	vtkMatrix4x4::Multiply4x4(newMat, oldMatInv, deltaMat);
}

inline void MakeMatrix(const glm::dvec3& x, const glm::dvec3& y, const glm::dvec3& z, const glm::dvec3& o, vtkMatrix4x4* dst)
{
	dst->Identity();
	dst->SetElement(0, 0, x.x);
	dst->SetElement(1, 0, x.y);
	dst->SetElement(2, 0, x.z);

	dst->SetElement(0, 1, y.x);
	dst->SetElement(1, 1, y.y);
	dst->SetElement(2, 1, y.z);

	dst->SetElement(0, 2, z.x);
	dst->SetElement(1, 2, z.y);
	dst->SetElement(2, 2, z.z);

	dst->SetElement(0, 3, o.x);
	dst->SetElement(1, 3, o.y);
	dst->SetElement(2, 3, o.z);
}

inline void MakeMatrix(double x, double y, double z, double qw, double qx, double qy, double qz, vtkMatrix4x4* dst)
{
	glm::dquat q(qw, qx, qy, qz);
	auto mat = glm::mat4_cast(q);
	mat = glm::inverse(mat);

	auto transform = vtkSmartPointer<vtkTransform>::New();
	transform->Translate(x, y, z);
	transform->Concatenate(glm::value_ptr(mat));
	transform->Update();
	dst->DeepCopy(transform->GetMatrix());
}