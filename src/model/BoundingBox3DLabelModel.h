#pragma once
#include "Header.h"
#include "ModelBase.h"
#include "Types.h"
#include "Structs.h"
#include "ColorLabelTableModel.h"

namespace NRender
{
	class BoundingBox3DActor;
	class BoundingBoxActor;
	class CubeActor;
	class BoxWidgetActor;
	class SliceCutterActor;
}

struct BoundingBox3DLabelItem : public UObject
{
	double Bounds[6];
	std::shared_ptr<ColorLabelItem> LabelType;
	QStandardItem* StandardItem;
};

struct RandomBoxLabelItem : public BoundingBox3DLabelItem
{
	std::weak_ptr<NRender::BoxWidgetActor> BoxActor;
	//std::weak_ptr<NRender::SliceCutterActor> CutterActor;
	std::vector<std::weak_ptr<NRender::SliceCutterActor>> CutterActors;
};


struct BoundingBox3DActorItem : public UObject
{
	std::weak_ptr<NRender::BoundingBox3DActor> Actors[3];
	std::weak_ptr<NRender::BoxWidgetActor> VoxCubeActor;

	bool IsActive;
	bool IsEnable;

	void SetActive(bool v);
	void SetEnable(bool v);
	
	void InitVoxBox(std::shared_ptr<BoundingBox3DLabelItem> label);
};


class BoundingBox3DLabelModel : public ModelBase
{
public:
	BoundingBox3DLabelModel();
	void Add(SliceView* view, std::shared_ptr<BoundingBox3DLabelItem> label);
	void Add(SliceView* view, std::shared_ptr<RandomBoxLabelItem> label);

	void Remove(const QString& id);
	void RemoveSelected();
	void SetDrawEnble(bool v) { m_IsDrawEnble = v; }
	NRender::BoundingBoxActor* CheckPick(SliceView* view, const glm::dvec3& pos);
	NRender::BoundingBoxActor* CheckContain(SliceView* view, const glm::dvec3& p1, const glm::dvec3& p2);
	void OnColorLabelUpdate();

	RandomBoxLabelItem* Find(const QString& id);

	void SetBoundingBoxActorEnable(NRender::BoundingBoxActor* actor);
	void SetBoundingBoxActorEnable(const std::string& id);

	virtual void OnFrameChanged(SliceView* view, int value) override;

	virtual void Clear() override;
	static void ComputeBounds(double* bounds, MprType ty, double* p1, double* p2);
	
	QStandardItemModel* GetModel() { return m_Model; }

	virtual void ExportJson(nlohmann::json& j) override;

private:
	BoundingBox3DActorItem* AddActor(std::shared_ptr<BoundingBox3DLabelItem> label);
	void AddVoxCube(BoundingBox3DActorItem* item, std::shared_ptr<BoundingBox3DLabelItem> label);

	void InitActor(int i, std::shared_ptr<NRender::BoundingBox3DActor> p, 
		std::shared_ptr<BoundingBox3DLabelItem> label, std::shared_ptr<BoundingBox3DActorItem> actorItem);
	void AddModelItem(std::shared_ptr<BoundingBox3DLabelItem> label);
	void RemoveModelItem(QStandardItem* item);

	void RemoveBox3D(const QString& id);
	void RemoveRandomBox(const QString& id);

private:
	std::map<QString, std::shared_ptr<BoundingBox3DLabelItem>> m_LabelMap;
	std::vector<std::shared_ptr<BoundingBox3DActorItem>> m_LabelActorList;

	std::map<QString, std::shared_ptr<RandomBoxLabelItem>> m_RandomLabelMap;
	QStandardItemModel* m_Model;

	bool m_IsDrawEnble = true;
	std::string m_CurEnableID;
};

