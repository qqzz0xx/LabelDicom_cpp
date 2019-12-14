#include "PolygonTools.h"
#include "FrameComponent.h"
#include "ModelManager.h"
#include "MprPositionModel.h"
#include "AppMain.h"
#include "WinLevelWidthTools.h"
#include "PointActor.h"
#include "CoordHelper.h"
#include "vtkBox.h"
#include "vtkBoundingBox.h"
#include "vtkPolygon.h"
#include "MainWindow.h"
#include "GlobalVal.h"
#include "MaskEditRecordModel.h"
#include "LoadMgr.h"
#include "valsalva_class.h"
#include "Helper.h"

PolygonTools::PolygonTools()
{
	m_Enable = false;
}

void PolygonTools::OnMouseEvent(MouseEventType type)
{
	bool isChanged = false;
	auto slice = m_View;

	auto model = gModelMgr->GetModel<MprPositionModel>();
	auto p = glm::make_vec3(model->m_WorldPos);
	p.z = 1;

	auto istyle = slice->GetRenderEngine()->GetInteractorStyle();

	if (type == MouseEventType_LeftDown)
	{
		if (m_State == State::Start)
		{
			PolygonPoint pp;
			pp.Pos = p;
			m_Actor->AddPoint(p);
			m_Points.emplace_back(pp);

			SwitchState(State::AddPoint);
		}
		else if (m_State == State::AddPoint)
		{
			AddOnePoint(p);
		}
		else if (m_State == State::PickPoint)
		{
		
			if (auto ids = CheckPick(p); !ids.empty())
			{
				for (auto&& id : ids)
				{
					m_Points[id].Selcet = true;
				}

				UpateBox();
				UpdateDraw();
				m_RectangleActor->SetActive(true);

				m_EventStartPos = p;
				SwitchState(State::MoveBox);
			}
			else
			{
				m_SelcetBox[0] = p;
				m_SelcetBox[1] = p;
				SwitchState(State::DragPoint);
			}
		}
		else if (m_State == EditorBox)
		{
			vtkBoundingBox box;
			box.AddPoint(glm::value_ptr(m_SelcetBox[0]));
			box.AddPoint(glm::value_ptr(m_SelcetBox[1]));

			bool hit = box.ContainsPoint(glm::value_ptr(p));
			if (hit)
			{
				m_EventStartPos = p;
				m_RectStartPos = m_SelcetBox[0];

				SwitchState(State::MoveBox);
			}
			else
			{
				if (auto ids = CheckPick(p); !ids.empty())
				{
					ResetAllPoints();
					for (auto&& id : ids)
					{
						m_Points[id].Selcet = true;
					}

					UpateBox();
					UpdateDraw();
					m_RectangleActor->SetActive(true);

					m_EventStartPos = p;
					SwitchState(State::MoveBox);
				}
				else
				{
					m_SelcetBox[0] = p;
					m_SelcetBox[1] = p;
					SwitchState(State::DragPoint);
				}
			}
		}

		isChanged = true;
	}
	else if (type == MouseEventType_LeftUp)
	{
		if (m_State == State::DragPoint)
		{
			bool b = CheckSelect();
			if (b)
			{
				UpateBox();
				UpdateDraw();
				SwitchState(State::EditorBox);
			}
			else
			{
				m_RectangleActor->SetActive(false);
				SwitchState(State::PickPoint);
			}

			isChanged = true;
		}
		else if (m_State == State::MoveBox)
		{
			SwitchState(State::EditorBox);
		}
	}
	else if (type == MouseEventType_Move)
	{
		if (istyle->LeftButtonPress)
		{
			if (m_State == State::DragPoint)
			{
				m_SelcetBox[1] = p;


				m_RectangleActor->SetPoints(m_SelcetBox[0], m_SelcetBox[1]);
				m_RectangleActor->SetActive(true);
				isChanged = true;
			}
			else if (m_State == State::AddPoint)
			{
				if (m_Mode == Curve)
				{
					auto lastPos = m_Points.back().Pos;
					if (CheckClose(p))
					{
						m_IsComplete = true;
						m_Actor->AddLastLine();
						SwitchState(State::PickPoint);

						isChanged = true;
					}
					else
					{
						double d = glm::length(lastPos - p);
						if (d > m_AddPointDragDist * g_GlobalVal.PolyActorZoomFactor)
						{
							PolygonPoint pp;
							pp.Pos = p;
							m_Actor->AddPoint(p);
							m_Points.emplace_back(pp);

							isChanged = true;
						}
					}
				}
			}
			else if (m_State == State::MoveBox)
			{
				auto mv = p - m_EventStartPos;

				for (size_t i = 0; i < m_Points.size(); i++)
				{
					auto& t = m_Points[i];
					if (t.Selcet)
					{
						t.Pos += mv;
						m_Actor->UpdatePoint(i, t.Pos);
					}
				}
				for (size_t i = 0; i < 2; i++)
				{
					m_SelcetBox[i] += mv;
				}

				//m_Actor->UpdateAll(m_Points);
				m_EventStartPos = p;
				m_RectangleActor->SetPoints(m_SelcetBox[0], m_SelcetBox[1]);

				isChanged = true;
			}

		}
	}

	if (isChanged)
	{
		slice->GetRenderEngine()->Render();
	}
}

void PolygonTools::SetEnable(bool v)
{
	__super::SetEnable(v);
	if (!v)
	{
		m_View->ui->stackedWidget->setCurrentWidget(m_View->ui->pageNone);
		OnClear();
	}
	else
	{
		if (m_RectangleActor == nullptr)
		{
			m_RectangleActor = std::make_shared<RectangleActor>();
			m_RectangleActor->SetActive(false);
			m_View->GetRenderEngine()->GetSceneManager()->AddNodeToGlobleScene(m_RectangleActor);
		}
		if (m_Actor == nullptr)
		{
			m_Actor = std::make_shared<PolygonActor>();
			m_View->GetRenderEngine()->GetSceneManager()->AddNodeToGlobleScene(m_Actor);

		}

		SwitchState(m_State);

		auto spacing = gLoadMgr->GetCurMaskData()->GetSpacing();
		float factor = (spacing[0] + spacing[1] + spacing[2]) * 0.333;
		m_AddPointDragDist *= factor;
		m_CloseDist *= factor;
		m_PickedDist *= factor;
		m_BoxOffset *= factor;
	}
}

void PolygonTools::OnComplete()
{
	if (m_State == State::AddPoint)
	{
		m_IsComplete = true;
		m_Actor->AddLastLine();
		SwitchState(State::PickPoint);
		m_View->GetRenderEngine()->Render();
	}
}

void PolygonTools::OnPaste()
{
	if (m_IsSaveLast)
	{
		m_IsComplete = true;

		m_Points = m_LastPoints;
		m_Actor->UpdateAll(m_Points);
		m_Actor->SetActive(true);
		SwitchState(State::PickPoint);
		m_View->GetRenderEngine()->Render();
	}
}

void PolygonTools::OnClear()
{
	if (m_Actor && m_RectangleActor)
	{
		m_Points.clear();
		m_Actor->Clear();
		m_RectangleActor->SetActive(false);
		SwitchState(State::Start);
		m_View->GetRenderEngine()->Render();
	}
}

void PolygonTools::OnAccpet()
{
	if (m_IsComplete)
	{
		MaskEditRecordItem record;

		auto imageActor = m_View->GetMaskImageActor();

		auto polygon = vtkSmartPointer<vtkPolygon>::New();

		for (size_t i = 0; i < m_Points.size(); i++)
		{
			const auto& p = m_Points[i];
			double wp[3] = { p.Pos.x, p.Pos.y, 0.0 };
			CoordHelper::WorldToImageXY(wp, imageActor, wp);
			polygon->GetPoints()->InsertNextPoint(wp);
		}

		double* point_data = static_cast<double*>(polygon->GetPoints()->GetData()->GetVoidPointer(0));
		int np = polygon->GetPoints()->GetNumberOfPoints();

		double normal[3];
		polygon->ComputeNormal(np, point_data, normal);

		double bounds[6];
		polygon->GetPoints()->GetBounds(bounds);

		auto maskData = gLoadMgr->GetCurMaskData();
		int* voxDim = maskData->GetDimensions();
		double* spacing = maskData->GetSpacing();
		uint8* ptr = (uint8*)maskData->GetScalarPointer();


		int curLabelType = GetCurLabelType();

		bool isInvert = gMainWindow->GetIsInvertPolygon();

		const int findInPolygonVal = isInvert ? 0 : 1;

		auto type = m_View->GetMprType();

		if (type != MprType::None)
		{
			auto mpr = gMainWindow->GetMprComponent();
			auto sc = mpr->GetSliceComponent(type);
			auto resliceMat = sc->GetReslice()->GetResliceMatrix();
			auto imgBds = imageActor->GetActor()->GetBounds();
			auto sliceImgData = imageActor->GetImageData();
			auto sliceSpacing = sliceImgData->GetSpacing();
			auto sliceDim = sliceImgData->GetDimensions();

			double vpos[3] = {};
			int ivpos[3] = {};

			for (double i = imgBds[0]; i < imgBds[1]; i+= sliceSpacing[0])
			{
				for (double j = imgBds[2]; j < imgBds[3]; j+= sliceSpacing[1])
				{
					double x[3] = { i, j, 0.0 };

					if (polygon->PointInPolygon(x, np, point_data, bounds, normal) == findInPolygonVal)
					{
						CoordHelper::ImageXYToVoxPos(x, resliceMat, vpos);
						ivpos[0] = std::round(vpos[0] / spacing[0]);
						ivpos[1] = std::round(vpos[1] / spacing[1]);
						ivpos[2] = std::round(vpos[2] / spacing[2]);
						size_t idx = ivpos[0] + ivpos[1] * voxDim[0] + ivpos[2] * voxDim[0] * voxDim[1];
						
						record.OldValues.emplace_back(ptr[idx]);
						record.Indexs.emplace_back(idx);

						ptr[idx] = curLabelType;
						maskData->Modified();
					}
				}
			}

			auto model = gModelMgr->GetModel<ColorLabelTableModel>();
			auto lookup = model->GetLookupTable();
			imageActor->SetLookupTable(lookup);

			for (int i = 0; i < 3; i++)
			{
				auto sc = mpr->GetSliceComponent((MprType)i);
				auto reslice = sc->GetMaskReslice();
				reslice->GetResliceMatrix()->Modified();
				reslice->Update();
				auto actor = sc->GetSliceView()->GetMaskImageActor();


				actor->Refresh();
				actor->SetActive(true);
			}

			mpr->Render();
		}
		else
		{
			for (size_t i = 0; i < voxDim[0]; i++)
			{
				for (size_t j = 0; j < voxDim[1]; j++)
				{
					double x[3] = { i, j, 0.0 };

				
					if (polygon->PointInPolygon(x, np, point_data, bounds, normal) == findInPolygonVal)
					{
						size_t idx = i + j * voxDim[0];

						record.OldValues.emplace_back(ptr[idx]);
						record.Indexs.emplace_back(idx);

						ptr[idx] = curLabelType;
					}
				}
			}

			//imageActor->Init(maskData);
			auto model = gModelMgr->GetModel<ColorLabelTableModel>();
			auto lookup = model->GetLookupTable();
			imageActor->SetLookupTable(lookup);
			imageActor->Refresh();
			imageActor->SetActive(true);

			m_View->GetRenderEngine()->Render();
		}


		SaveLastPoint();


		record.IsInvert = isInvert;
		record.LabelType = curLabelType;
		record.MaskPtr = ptr;

		auto recordModel = gModelMgr->GetModel<MaskEditRecordModel>();
		recordModel->AddRecord(std::move(record));

		g_GlobalVal.IsMaskChanged = true;
	}
}

void PolygonTools::OnDelete()
{
	if (m_State != State::EditorBox) return;

	auto it = std::remove_if(m_Points.begin(), m_Points.end(), [](auto p) {return p.Selcet == true; });
	m_Points.erase(it, m_Points.end());

	m_Actor->UpdateAll(m_Points);
	m_View->GetRenderEngine()->Render();
	m_RectangleActor->SetActive(false);

	SwitchState(State::PickPoint);
}

void PolygonTools::OnSplit()
{
	if (m_State != State::EditorBox) return;

	std::vector<PolygonPoint> newPoints;

	for (size_t i = 0, length = m_Points.size(); i < length; i++)
	{
		auto& p = m_Points[i];
		newPoints.emplace_back(p);

		int j = (i + 1) % length;
		auto& p2 = m_Points[j];

		if (p.Selcet && p2.Selcet)
		{
			auto c = (p.Pos + p2.Pos) * 0.5;
			PolygonPoint pp;
			pp.Selcet = true;
			pp.Pos = c;
			newPoints.emplace_back(pp);
		}
	}

	m_Points.swap(newPoints);
	m_Actor->UpdateAll(m_Points);
	m_View->GetRenderEngine()->Render();
}

static std::map<std::string, std::shared_ptr<valsalva>> s_SegModels;

void PolygonTools::OnAutoSeg(const std::string& path)
{

	//auto it = s_SegModels.find(path);
	//if (it == s_SegModels.end())
	//{
	//	s_SegModels[path] = std::make_shared<valsalva>(path);
	//}


	//auto imageActor = m_View->GetSingleImageActor();
	//auto data = imageActor->GetImageData();
	//cv::Mat mat;
	//Helper::VtkImageDataToCVMat(data, mat);

	//cv::cvtColor(mat, mat, cv::COLOR_RGB2GRAY);
	//
	//auto va = s_SegModels[path];

	//std::vector<cv::Point> cvPos;
	//va->getSeg(mat, cvPos);

	//m_Points.clear();

	//for (auto&& p : cvPos)
	//{
	//	PolygonPoint pp;
	//	pp.Pos.x = p.x;
	//	pp.Pos.y = p.y;
	//	pp.Pos.z = 0;

	//	double wp[3] = { 0 };
	//	CoordHelper::ImageXYToWorld(glm::value_ptr(pp.Pos), imageActor, wp);

	//	pp.Pos.x = wp[0];
	//	pp.Pos.y = wp[1];
	//	pp.Pos.z = 1;
	//	
	//	m_Points.emplace_back(pp);
	//}

	//m_IsComplete = true;
	//m_Actor->UpdateAll(m_Points);
	//m_Actor->SetActive(true);
	//SwitchState(State::PickPoint);
	//m_View->GetRenderEngine()->Render();
}


bool PolygonTools::CheckClose(const glm::dvec3& p)
{
	if (m_Points.size() < 3) return false;

	auto first = m_Points.front();

	auto len = glm::length(p - first.Pos);

	return len < m_CloseDist * g_GlobalVal.PolyActorZoomFactor;
}

std::vector<int> PolygonTools::CheckPick(const glm::dvec3& p)
{
	std::vector<int> ret;
	for (int i = 0; i < m_Points.size(); i++)
	{
		auto& pt = m_Points[i];
		auto len = glm::length(p - pt.Pos);

		if (len < m_PickedDist * g_GlobalVal.PolyActorZoomFactor)
		{
			ret.push_back(i);
		}
	}
	if (ret.size() == 1)
	{
		return ret;
	}
	if (ret.size() > 1)
	{
		auto& pt = m_Points[ret[0]];
		double minDist = glm::length(p - pt.Pos);
		int minIdx = 0;
		for (auto&& id : ret)
		{
			auto& pp = m_Points[id];
			auto len = glm::length(p - pp.Pos);

			if (len < minDist)
			{
				minIdx = id;
			}
		}
		std::vector<int> tlist;
		tlist.push_back(minIdx);
		return tlist;
	}

	for (size_t i = 0, length = m_Points.size(); i < length; i++)
	{
		int j = (i + 1) % length;
		auto& p1 = m_Points[i].Pos;
		auto& p2 = m_Points[j].Pos;

		auto line = p2 - p1;
		auto b = p - p1;
		auto n = glm::normalize(line);
		double b_dist = glm::dot(b, n);
		auto a = b_dist * n;

		double d = glm::length(a - b);

		if (d < m_PickedDist * g_GlobalVal.PolyActorZoomFactor && b_dist > 0 && b_dist < glm::length(line))
		{
			ret.push_back(i);
			ret.push_back(j);
			return ret;
		}
	}

	return ret;
}

bool PolygonTools::CheckSelect()
{
	vtkBoundingBox box;
	box.AddPoint(glm::value_ptr(m_SelcetBox[0]));
	box.AddPoint(glm::value_ptr(m_SelcetBox[1]));

	bool select = false;
	for (size_t i = 0; i < m_Points.size(); i++)
	{
		auto& p = m_Points[i];

		if (box.ContainsPoint(glm::value_ptr(p.Pos)))
		{
			p.Selcet = true;
			select = true;
		}
		else
		{
			p.Selcet = false;
		}
	}

	return select;
}

void PolygonTools::ResetAllPoints()
{
	for (auto& p : m_Points)
	{
		p.Selcet = false;
	}
}

void PolygonTools::UpateBox()
{
	if (m_Points.size() < 3) return;

	double xmin, xmax, ymin, ymax, z;
	for (auto& var : m_Points)
	{
		if (!var.Selcet) continue;
		auto firstPos = var.Pos;
		xmin = firstPos.x;
		xmax = firstPos.x;
		ymin = firstPos.y;
		ymax = firstPos.y;
		z = firstPos.z;
	}

	for (auto& var : m_Points)
	{
		if (!var.Selcet) continue;

		auto p = var.Pos;

		if (p.x < xmin)
		{
			xmin = p.x;
		}
		if (p.x > xmax)
		{
			xmax = p.x;
		}
		if (p.y < ymin)
		{
			ymin = p.y;
		}
		if (p.y > ymax)
		{
			ymax = p.y;
		}
	}

	auto offset = m_BoxOffset * g_GlobalVal.PolyActorZoomFactor;
	m_SelcetBox[0] = glm::dvec3(xmin, ymin, z) - offset;
	m_SelcetBox[1] = glm::dvec3(xmax, ymax, z) + offset;
	m_RectangleActor->SetPoints(m_SelcetBox[0], m_SelcetBox[1]);
}

void PolygonTools::UpdateDraw()
{
	for (size_t i = 0; i < m_Points.size(); i++)
	{
		auto& p = m_Points[i];
		auto actor = m_Actor->GetPoint(i);
		if (p.Selcet)
		{
			actor->SetColor(m_SelectColor);
		}
		else
		{
			actor->SetColor(m_DefualtColor);
		}
	}
}

void PolygonTools::SwitchState(State state)
{
	if (m_State == State::Start)
	{
		m_IsComplete = false;
		m_View->ui->stackedWidget->setCurrentWidget(m_View->ui->pageNone);
	}
	if (m_State == State::Start && state == State::AddPoint)
	{
		m_View->ui->stackedWidget->setCurrentWidget(m_View->ui->pageDraw);
	}
	if (m_State == State::AddPoint && state == State::PickPoint)
	{
		m_View->ui->stackedWidget->setCurrentWidget(m_View->ui->pageEditor);
	}

	m_State = state;

}

void PolygonTools::SaveLastPoint()
{
	m_LastPoints = m_Points;
	for (auto p : m_LastPoints)
	{
		p.Selcet = false;
	}
	m_IsSaveLast = true;
}

void PolygonTools::AddOnePoint(const glm::dvec3& p)
{
	if (CheckClose(p))
	{
		m_IsComplete = true;
		m_Actor->AddLastLine();
		SwitchState(State::PickPoint);
	}
	else
	{
		PolygonPoint pp;
		pp.Pos = p;
		m_Actor->AddPoint(p);
		m_Points.emplace_back(pp);
	}
}

int PolygonTools::GetCurLabelType()
{
	auto str = gMainWindow->ui->comboBox->currentText();
	return str.toInt();
}
