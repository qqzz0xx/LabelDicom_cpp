#pragma once
#include "Header.h"
#include "ModelBase.h"

class ModelManager
{
public:
	ModelManager();
	template<class T>
	T* GetModel();

	void OnFrameChanged(SliceView* view, int v);
	void ExportJson(const QString& path);
	void ImportJson(const QString& path, bool decode);
	void Start();
	void Exit();

private:
	std::map<std::string, std::shared_ptr<ModelBase> > m_Models;
};


template<class T>
inline T* ModelManager::GetModel()
{
	auto hash = typeid(T*).name();
	auto model = m_Models[hash];
	return (T*)model.get();
}
