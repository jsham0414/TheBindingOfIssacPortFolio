#pragma once
class ItemInfo : public GameEngineRes<ItemInfo> {
public:
	static ItemInfo* Load(const std::string& _Path) {
		return Load(_Path, GameEnginePath::GetFileName(_Path));
	}

	static ItemInfo* Load(const std::string& _Path, const std::string& _Name);

	static int Count;
public:
	// constrcuter destructer
	ItemInfo();
	~ItemInfo();

	ItemInfo(std::string _Name, std::string _Description, std::string _Icon, std::string _Costume) {
		Index = Count++;
		Name = _Name;
		Description = _Description;
		Icon = _Icon;
		Costume = _Costume;
	}
public:
	int Index;
	std::string Icon;
	std::string Costume;
	std::string Name;
	std::string Description;
};

