#pragma once

namespace Hourglass
{
	class ISpatial
	{
	public:
		virtual ~ISpatial() {}
		virtual void DebugRender() {};
	};
}