#pragma once
namespace Hourglass
{
	class Message;
	class Engine;
	class EngineMessage
	{
	public:
		void Init(Engine* engine);
		virtual ~EngineMessage();
		void SendMsg(int msg);
	private:
		Engine* m_engine;
	};
	extern EngineMessage g_engineMsg;
}