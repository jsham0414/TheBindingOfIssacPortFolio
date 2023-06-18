#include "PreCompile.h"
#include "ItemInfo.h"

class ItemInfoInit {
public:
	ItemInfoInit() {
	}

	~ItemInfoInit() {
	}
};

ItemInfoInit Inst;

int ItemInfo::Count = 0;

ItemInfo* ItemInfo::Load(const std::string& _Path, const std::string& _Name) {

	return nullptr;
}

ItemInfo::ItemInfo() {
}

ItemInfo::~ItemInfo() {
}
