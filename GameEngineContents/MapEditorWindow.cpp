#include "PreCompile.h"
#include "MapEditorWindow.h"
#include "TileMapActor.h"
#include "GameEngineContents/MapEditorLevel.h"

class GameEngineCore;

MapEditorWindow::MapEditorWindow() : 
    Scale{52, 52}, 
    SelectTile(-1), 
    Selected(0) {

}

MapEditorWindow::~MapEditorWindow() {
    for_each(BatchInfos.begin(), BatchInfos.end(), CONTAINER_DELETE());
    BatchInfos.clear();
}


void MapEditorWindow::Initialize(class GameEngineLevel* _Level) {
    // 1. 메타 파일을 읽어서 OnGUI ListBox에 읽힌 몬스터 이름으로 Index를 추출
    // 2. 격자 클릭 시 피킹으로 몬스터 배치
    // 3. 배치 된 데이터를 저장
    if (false == GameEngineInput::GetInst()->IsKey("TileSet")) {
        GameEngineInput::GetInst()->CreateKey("TileSet", VK_LBUTTON);
    }

    std::list<GameEngineActor*> temp = GameEngineCore::GetCurLevel()->GetActors(static_cast<int>(OBJECTORDER::TileMapObject));

    int index = 0;
    for (auto a : temp) {
        // 이름, 텍스쳐, 구분은 Index로
        ObjectBatchInfo* batchInfo = new ObjectBatchInfo();
        batchInfo->Index = index++;
        batchInfo->Name = a->GetNameCopy();
        batchInfo->Renderer = a->GetRenderer();
        BatchInfos.push_back(batchInfo);
    }
}


void MapEditorWindow::OnGUI(GameEngineLevel* _Level, float _DeltaTime) {
    if (BatchInfos.size() > 0) {
        if (ImGui::BeginCombo("Select object", BatchInfos[Selected]->Name.c_str())) {
            for (int n = 0; n < BatchInfos.size(); n++) {
                const bool is_selected = (Selected == n);
                if (ImGui::Selectable(BatchInfos[n]->Name.c_str(), is_selected))
                    Selected = n;

                // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }

    const char* items[] = { "Start", "Room", "Boss", "Shop", "Gold" };
    static int item_current_idx = 0; // Here we store our selection data as an index.
    const char* combo_preview_value = items[item_current_idx];  // Pass in the preview value visible before opening the combo (it could be anything)
    if (ImGui::BeginCombo("combo 1", combo_preview_value)) {
        for (int n = 0; n < IM_ARRAYSIZE(items); n++) {
            const bool is_selected = (item_current_idx == n);
            if (ImGui::Selectable(items[n], is_selected))
                item_current_idx = n;

            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    static char buf2[64] = "0"; ImGui::InputText("Stage Number", buf2, 64, ImGuiInputTextFlags_CharsDecimal);
    //GameRoomType
    if (true == ImGui::Button("Save")) {
        auto BatchData = ObjectBatcher->GetBatchData();
        string Result = "";
        Json::Value Temp;
        Temp["RoomType"] = item_current_idx + 1;
        Temp["Stage"] = buf2;
        for (int i = 0; i < 7 * 16; i++) {
            if (BatchData->find(i) == BatchData->end())
                continue;

            Json::Value Indent;
            Indent["Index"] = i;
            Indent["Type"] = BatchData->at(i)->GetType();
            Temp[to_string(i)] = Indent;
        }

        Json::StyledWriter Writer;
        Result = Writer.write(Temp);
        string RoomType = items[item_current_idx];
        string Path = "../GameEngineRoomData\\" + RoomType + '\\' + GameEngineTime::GetDateTimeString(true) + ".json";
        GameEngineFile file(Path);
        file.Open(OpenMode::Write, FileMode::Binary);
        file.Write((void*)Result.c_str(), Result.length());

        // 

        //static_cast<MapEditorLevel>()

        //GameEngineFile::Write()
    }

    return;

    // ImGui::PushID
	if (true == ImGui::Button("FolderTextureLoad")) {
        GameEngineDirectory Dir;
        Dir.MoveParentToExitsChildDirectory("ContentsResources");
        Dir.Move("ContentsResources");
        Dir.Move("Texture");
        Dir.Move("Map");



        std::string Path =  GameEngineGUI::OpenFolderDlg(GameEngineString::AnsiToUTF8Return("폴더 텍스처 로드"), Dir.GetFullPath());

        if (false == Path.empty())
        {
            SelectFolderTexture = GameEnginePath::GetFileName(Path);

            GameEngineFolderTexture::Load(Path.c_str());
        }
	}

    if (false == SelectFolderTexture.empty())
    {
        ImGui::SameLine();
        ImGui::Text(SelectFolderTexture.c_str());
        ImGui::InputInt2("Scale", Scale);

        if (true == ImGui::Button("MapCreate"))
        {
            TileMap->TileRenderer->CreateIsometricTileMap(Scale[0], Scale[1], {64, 32}, SelectFolderTexture, 32);
        }


        std::string SelectIndex = "Select Index = ";
        int X;
        int Y;
        TileMap->TileRenderer->GetTileIndex(_Level->GetMainCamera()->GetMouseWorldPosition(), X, Y);

        SelectIndex += std::to_string(X);
        SelectIndex += " " + std::to_string(Y);

        ImGui::Text(SelectIndex.c_str());
    }

    GameEngineFolderTexture* Texture = GameEngineFolderTexture::Find(SelectFolderTexture);

    if (nullptr != Texture)
    {
        ImGui::BeginChildFrame(ImGui::GetID("TileSelect"), { 90 * 5, 500 });

        for (size_t i = 0; i < Texture->GetTextureCount(); i++)
        {
            GameEngineTexture* TileImage = Texture->GetTexture(i);
            if (ImGui::ImageButton(reinterpret_cast<ImTextureID>(TileImage->CreateShaderResourceView()), { 64, 32 }))
            {
                SelectTile = i;
            }
            
            if (0 != (i + 1)% 5)
            {
                ImGui::SameLine();
            }
        }
        ImGui::EndChildFrame();
    }

    if (true == GameEngineInput::GetInst()->IsDown("TileSet")
        && nullptr != Texture
        && -1 != SelectTile
        && SelectTile < Texture->GetTextureCount())
    {

        float4 MousePos = _Level->GetMainCamera()->GetMouseWorldPosition();
        // GameEngineInput::

        TileMap->TileRenderer->SetTileIndex(MousePos, SelectTile);
        int a = 0;
    }
    // TileMap->TileRenderer->

}