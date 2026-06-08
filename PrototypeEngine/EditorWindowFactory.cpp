#include "EditorWindowFactory.h"
#include "EditorTextureManager.h"
#include "Renderer.h"
#include "WindowRenderProperty.h"
#include "GUIMainMenu.h"
#include "ToolbarPanel.h"
#include "GameViewPanel.h"
#include "SceneViewPanel.h"
#include "HierarchyPanel.h"
#include "ProjectPanel.h"
#include "InspectorPanel.h"

std::unordered_map<string, EditorWindow*> EditorWindowFactory::sCreators;

void EditorWindowFactory::RegisterEditorWindow(EditorWindow* creator)
{
	auto it = sCreators.find(creator->GetID());
	if (it == sCreators.end()) {
		sCreators.emplace(creator->GetID(), creator);
	}
	else {
		Debug::Log("This Window already created");
	}
}

EditorWindow* EditorWindowFactory::CreateEditorWindow(const string& type)
{
	auto it = sCreators.find(type);
	if (it != sCreators.end())
	{
		//¶¬ŠÖ”‚ðŽÀs
		return it->second;
	}
	return nullptr;
}

std::vector<string> EditorWindowFactory::GetRegisteredEditorWindowNames()
{
	vector<string> names;
	for (const auto& pair : sCreators)
	{
		names.push_back(pair.first);
	}
	return names;
}

void EditorWindowFactory::UnregisterAllEditorWindows()
{
	for (auto& pair : sCreators) {
		delete pair.second;
		pair.second = nullptr;
	}
	sCreators.clear();
}

void RegisterAllEditorWindows(Renderer* renderer) {
	//EditorWindowFactory::RegisterEditorWindow(new GUIMainMenu(renderer));
	//EditorWindowFactory::RegisterEditorWindow(new ToolbarPanel(renderer));
	EditorWindowFactory::RegisterEditorWindow(new GameViewPanel(renderer));
	EditorWindowFactory::RegisterEditorWindow(new SceneViewPanel(renderer));
	EditorWindowFactory::RegisterEditorWindow(new HierarchyPanel(renderer));
	EditorWindowFactory::RegisterEditorWindow(new ProjectPanel(renderer));
	EditorWindowFactory::RegisterEditorWindow(new InspectorPanel(renderer));
}