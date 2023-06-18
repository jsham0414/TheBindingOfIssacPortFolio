#pragma once
#include <string>

class GameEngineNameObject
{
public:
	// constrcuter destructer
	GameEngineNameObject();
	virtual ~GameEngineNameObject();
	
	bool Compare(const std::string& _Name) const {
		return ObjectName_ == _Name;
	}

	inline void SetName(const std::string& _Name) {
		ObjectName_ = _Name;
	}

	std::string GetNameCopy() const
	{
		return ObjectName_;
	}

	const std::string& GetNameConstRef()
	{
		return ObjectName_;
	}

	const char* GetNameConstPtr()
	{
		return ObjectName_.c_str();
	}


protected:

private:
	std::string ObjectName_;

};

