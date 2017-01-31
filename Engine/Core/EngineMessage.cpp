#include "pch.h"
#include "EngineMessage.h"
#include "Message.h"
#include "Engine.h"
namespace Hourglass
{
	EngineMessage g_engineMsg;
}
void Hourglass::EngineMessage::Init(Engine * engine)
{
	m_engine = engine;
}
Hourglass::EngineMessage::~EngineMessage()
{
}

void Hourglass::EngineMessage::SendMsg(int msg)
{
	if (msg == MessageType::kResetLevel)
	{
		if (m_engine)
		{
			m_engine->ReloadLevel();
		}
	}
	else
	{
		if (m_engine)
			m_engine->OnMessage(msg);
	}
}
