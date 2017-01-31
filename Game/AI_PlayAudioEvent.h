#pragma once

class AI_PlayAudioEvent : public Hourglass::IAction
{
public:
	void LoadFromXML(tinyxml2::XMLElement* data);

	bool IsRunningChild() const { return false; }

	void Init(Hourglass::Entity* entity);

	IBehavior::Result Update(Hourglass::Entity* entity);

	IBehavior* MakeCopy() const;

private:
	bool  m_OnDoneReturnFailure;
	StrID m_EventName;
	float m_Delay;
	float m_Timer;
};