#include "MeshModel.h"
#include "AppMain.h"
#include "LoadMgr.h"
#include "ColorLabelTableModel.h"
#include "QtHelper.h"


MeshModel::MeshModel()
{
	m_Name = typeid(this).name();
}

void MeshModel::UpdateAll()
{
	auto re = m_RenderEngine;

	auto maskData = gLoadMgr->GetCurMaskData();
	auto num = maskData->GetNumberOfPoints();
	auto ptr = (uint8*)maskData->GetScalarPointer();

	int imin = 0;
	int imax = 0;

	for (size_t i = 0; i < num; i++)
	{
		if (ptr[i] > imax)
		{
			imax = ptr[i];
		}
	}

	std::vector<int> labels;

	for (int ilabel = imin + 1; ilabel <= imax; ilabel++)
	{
		for (int i = 0; i < num; i++)
		{
			if (ptr[i] == ilabel)
			{
				labels.emplace_back(ilabel);
				break;
			}
		}
	}

	for (auto it = m_Meshes.begin(); it != m_Meshes.end(); it++)
	{
		re->GetSceneManager()->RemoveNodeByID(it->second);
	}
	m_Meshes.clear();


	auto colorModel = gModelMgr->GetModel<ColorLabelTableModel>();
	
	for (size_t i = 0; i < labels.size(); i++)
	{
		int labelType = labels[i];
		auto colorLabel = colorModel->GetColorLabel(QString::number(labelType));
		if (colorLabel == nullptr) continue;

		auto imageData = vtkSmartPointer<vtkImageData>::New();
		imageData->DeepCopy(maskData);
		auto ptr = (uint8*)imageData->GetScalarPointer();
		for (size_t i = 0, size = imageData->GetNumberOfPoints(); i < size; i++)
		{
			ptr[i] = ptr[i] == labelType ? labelType : 0;
		}

		auto mesh = std::make_shared<MeshActor>();
		re->GetSceneManager()->AddNodeToGlobleScene(mesh);
		mesh->LoadFromImageData(imageData, labelType);
		
		double rgba[4] = {};
		QtHelper::QColorToVTKRGBA(colorLabel->Color, rgba);
		mesh->SetColor(rgba);
		mesh->SetAlpha(rgba[3]);

		//mesh->ResetCameraToMe();
		m_Meshes.emplace(labelType, mesh->GetUUID());
	}
}

void MeshModel::ShowAll(bool v)
{
	std::for_each(m_Meshes.begin(), m_Meshes.end(), [&](auto p) {
		auto mesh = m_RenderEngine->GetSceneManager()->FindNodeByID(p.second);
		mesh->SetActive(v);
	});
}

void MeshModel::UpdateColor(int labalType, const QColor& color)
{
	auto it = m_Meshes.find(labalType);
	if (it != m_Meshes.end())
	{
		auto p = m_RenderEngine->GetSceneManager()->FindNodeByID(it->second);

		if (p)
		{
			double rgba[4];
			QtHelper::QColorToVTKRGBA(color, rgba);
			p->SetColor(rgba);
			p->SetAlpha(rgba[3]);
			p->GetActor()->Modified();
		}
	}
}
