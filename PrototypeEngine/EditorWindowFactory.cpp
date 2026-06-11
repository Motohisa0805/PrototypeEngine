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
#include "AboutEnginePanel.h"
#include "HierarchyExplanationPanel.h"
#include "ProjectExplanationPanel.h"
#include "InspectorExplanationPanel.h"

std::unordered_map<string, WindowCreator> EditorWindowFactory::sCreators;

std::unordered_map<string, int>	EditorWindowFactory::sInstanceCounters;

void EditorWindowFactory::RegisterEditorWindow(const string& id, WindowCreator creator)
{
	sCreators[id] = creator;
}

EditorWindow* EditorWindowFactory::CreateEditorWindow(const string& type,Renderer* renderer)
{
	auto it = sCreators.find(type);
	if (it == sCreators.end()) return nullptr;

	EditorWindow* newWindow = it->second(renderer);

	if (newWindow) {
		sInstanceCounters[type]++;
		int currentCount = sInstanceCounters[type];

		newWindow->SetInstanceID(currentCount);
	}

	return newWindow;
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
	sCreators.clear();
}

void RegisterAllEditorWindows() {
	EditorWindowFactory::RegisterEditorWindow("GameView", [](Renderer* r) -> EditorWindow* { return new GameViewPanel(r);});
	EditorWindowFactory::RegisterEditorWindow("SceneView", [](Renderer* r) -> EditorWindow* { return new SceneViewPanel(r);});
	EditorWindowFactory::RegisterEditorWindow("Hierarchy", [](Renderer* r) -> EditorWindow* { return new HierarchyPanel(r);});
	EditorWindowFactory::RegisterEditorWindow("Project", [](Renderer* r) -> EditorWindow* { return new ProjectPanel(r);});
	EditorWindowFactory::RegisterEditorWindow("Inspector", [](Renderer* r) -> EditorWindow* { return new InspectorPanel(r);});
	EditorWindowFactory::RegisterEditorWindow("AboutEngine", [](Renderer* r) -> EditorWindow* { return new AboutEnginePanel(r);});
	EditorWindowFactory::RegisterEditorWindow("HierarchyExplanation", [](Renderer* r) -> EditorWindow* { return new HierarchyExplanationPanel(r);});
	EditorWindowFactory::RegisterEditorWindow("ProjectExplanation", [](Renderer* r) -> EditorWindow* { return new ProjectExplanationPanel(r);});
	EditorWindowFactory::RegisterEditorWindow("InspectorExplanation", [](Renderer* r) -> EditorWindow* { return new InspectorExplanationPanel(r);});
}