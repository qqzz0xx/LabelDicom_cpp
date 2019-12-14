#include "ModelManager.h"
#include "MprPositionModel.h"
#include "ColorLabelTableModel.h"
#include "LandMarkListModel.h"
#include "MeshModel.h"
#include "MaskEditRecordModel.h"
#include "FrameLabelModel.h"
#include "BoundingBoxLabelModel.h"
#include "BoundingBox3DLabelModel.h"
#include "LabelDetailModel.h"
#include "Helper.h"
#include "AppMain.h"
#include "GlobalVal.h"
#include "CodingHelper.h"
#include "JsonHelper.h"

ModelManager::ModelManager()
{
	auto func = [&](std::shared_ptr<ModelBase> model)
	{
		m_Models.emplace(model->GetName(), model);
	};

	func(std::make_shared<MprPositionModel>());
	func(std::make_shared<ColorLabelTableModel>());
	func(std::make_shared<LandMarkListModel>());
	func(std::make_shared<MeshModel>());
	func(std::make_shared<MaskEditRecordModel>());
	func(std::make_shared<FrameLabelModel>());
	func(std::make_shared<BoundingBoxLabelModel>());
	func(std::make_shared<BoundingBox3DLabelModel>());
	func(std::make_shared<LabelDetailModel>());
}

void ModelManager::OnFrameChanged(SliceView* view, int v)
{
	for (auto&& var : m_Models)
	{
		var.second->OnFrameChanged(view, v);
	}
}

void ModelManager::ExportJson(const QString& path)
{
	nlohmann::json j;
	for (auto&& [k, v] : m_Models)
	{
		v->ExportJson(j[k]);
	}

	nlohmann::json js;
	js["Models"] = j;
	g_ConfigVal.Save(js["Config"]);
	
	GetModel<ColorLabelTableModel>()->ExportParentJson(js["LabelGroup"]);

	auto txt = js.dump(4);
	QByteArray bytes(txt.c_str(), txt.size());
	auto encodeTxt = CodingHelper::EncodeBytes(bytes);
	//Helper::SaveJson(path, "Label", js, true);

	qDebug() << "txt size: " << txt.size();
	qDebug() << "encodeTxt size" << encodeTxt.size();

	std::stringstream ss;
	ss << path.toStdString() << "/" << g_GlobalVal.FileName.toStdString() << "_Label" << ".cdat";
	QFile outfile(QString::fromStdString(ss.str()));
	outfile.open(QFile::WriteOnly | QFile::Truncate);
	outfile.write(encodeTxt);
	outfile.flush();
	outfile.close();
}

void ModelManager::ImportJson(const QString& path, bool decode)
{
	auto js = Helper::LoadJson(path, decode);
	g_ConfigVal.Load(js["Config"]);

	auto j = js["Models"];
	if (!j.is_null()) {

		auto colorTableModel = GetModel<ColorLabelTableModel>();
		auto colorTableKey = colorTableModel->GetName();
		auto coloTable = j[colorTableKey];
		colorTableModel->ImportJson(coloTable);

		for (auto it = j.begin(); it != j.end(); ++it) {
			auto k = it.key();
			auto v = it.value();

			if (v.is_null() || k == colorTableKey) continue;

			m_Models[k]->ImportJson(v);
		}

		gMainWindow->RenderAll();
	}

	nlohmann::json j_labelGroup;
	JsonHelper::TryGetValue(js, "LabelGroup", j_labelGroup);
	GetModel<ColorLabelTableModel>()->ImportParentJson(j_labelGroup);
}

void ModelManager::Start()
{
	for (auto&& [k, v] : m_Models)
	{
		v->Start();
	}
}

void ModelManager::Exit()
{
	for (auto&& [k, v] : m_Models)
	{
		v->Exit();
	}
}

