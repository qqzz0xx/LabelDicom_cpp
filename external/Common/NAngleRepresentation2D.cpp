#include "NAngleRepresentation2D.h"
#include "vtkLeaderActor2D.h"
#include "vtkProperty2D.h"

NAngleRepresentation2D * NAngleRepresentation2D::New()
{
	return new NAngleRepresentation2D;
}

NAngleRepresentation2D::NAngleRepresentation2D()
{
}


NAngleRepresentation2D::~NAngleRepresentation2D()
{
}

void NAngleRepresentation2D::SetColor(double * rgb)
{
	this->Ray1->GetProperty()->SetColor(rgb);
	this->Ray2->GetProperty()->SetColor(rgb);

	this->Arc->GetProperty()->SetColor(rgb);
}
