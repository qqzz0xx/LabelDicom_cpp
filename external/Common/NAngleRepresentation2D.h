#pragma once
#include "vtkAngleRepresentation2D.h"
#include "vtkSetGet.h"
class NAngleRepresentation2D : public vtkAngleRepresentation2D
{
public:
	static NAngleRepresentation2D* New();
	vtkTypeMacro(NAngleRepresentation2D, vtkAngleRepresentation2D);
	NAngleRepresentation2D();
	~NAngleRepresentation2D();

	void SetColor(double* rgb);

};

